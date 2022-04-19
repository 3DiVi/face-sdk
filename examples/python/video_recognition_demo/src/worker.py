#  file video_recognition_demo/src/Worker.cpp

from threading import Thread
import time

from face_sdk_3divi import Point, Capturer
from face_sdk_3divi.modules import video_worker, raw_image, raw_sample, age_gender_estimator, emotions_estimator, recognizer, active_liveness
from face_sdk_3divi.example import CVRawImage

from .image_and_depth_source import ImageAndDepthSource, ImageAndDepth

from io import BytesIO
from queue import Queue
import numpy as np
import cv2
import os

import threading


class Database:

    def __init__(self, database_list_filepath: str,
                 recognizer: recognizer.Recognizer,
                 capturer: Capturer,
                 distance_threshold: float):
        self.vw_elements = list()
        self.samples = list()
        self.thumbnails = list()
        self.names = list()

        with open(database_list_filepath, "r", encoding="utf-8") as file:
            files = [x.strip() for x in file.readlines()]

        files = [x for x in files if not x.endswith(".txt")]
        files = list(filter(None, files))

        files = sorted(files)

        dirs = [os.path.join(
            os.path.dirname(database_list_filepath), os.path.dirname(f)
        ) for f in files]

        files = [os.path.basename(x) for x in files]

        prev_dir = ""
        person_id_counter = 0
        element_id_counter = 0

        for (f, dir) in list(zip(files, dirs)):
            if dir != prev_dir:
                person_id_counter += 1

                # try to open name.txt file in this dir
                try:
                    name_file = open(os.path.join(dir, "name.txt"), 'r')
                    name = name_file.readline().strip()
                except FileNotFoundError:
                    name = ""
                prev_dir = dir

            print(f"processing '{f}' name: '{name}'  person_id: {person_id_counter}")

            # read image with opencv
            image_path = os.path.join(dir, f)
            cv_img = cv2.imdecode(np.fromfile(image_path, dtype=np.uint8), cv2.IMREAD_COLOR)
            assert cv_img is not None, f"Not opened {image_path}"
            image = CVRawImage(cv_img)

            if image.format != raw_image.Format.FORMAT_BGR:
                print(f"\n\nWarning: can't read image '{f}'\n\n")
                continue

            # capture the face
            captured_samples = capturer.capture(image)

            if len(captured_samples) != 1:
                print(f"\n\nWARNING: detected {len(captured_samples)} faces on '{f}' "
                      f"image instead of one, image ignored \n\n")

            sample = captured_samples[0]

            # make template
            templ = recognizer.processing(sample)

            # prepare data for VideoWorker
            vw_element = video_worker.DatabaseElement(element_id_counter, person_id_counter, templ, distance_threshold)
            element_id_counter += 1
            thumbnail = self.make_thumbnail(sample, name)

            self.vw_elements.append(vw_element)
            self.samples.append(sample)
            self.thumbnails.append(thumbnail)
            self.names.append(name)

            assert element_id_counter == len(self.vw_elements)
            assert element_id_counter == len(self.samples)
            assert element_id_counter == len(self.thumbnails)
            assert element_id_counter == len(self.names)

    @staticmethod
    def make_thumbnail(sample: raw_sample.RawSample, name: str = "", thumbnail_size_in_: int = -1):
        thumbnail_size = thumbnail_size_in_ if thumbnail_size_in_ > 0 else Worker.thumbnail_size

        # buffer for the cutted image
        stream = BytesIO()

        # make a cut in bmp format
        # so we don't waste time for encode/decode image
        # just copying it few times, which is irrelevant
        sample.cut_face_image(stream,
                              raw_sample.ImageFormat.IMAGE_FORMAT_BMP,
                              raw_sample.FaceCutType.FACE_CUT_BASE)
        stream.seek(0)
        buffer = np.asarray(bytearray(stream.read()), dtype=np.uint8)
        temp = cv2.imdecode(buffer, 1)

        # so we got an image
        # check it
        assert temp.size and len(temp.shape) == 3 and temp.shape[2] == 3 and temp.dtype == np.uint8

        # and resize to the thumbnail_size
        rows, cols = temp.shape[:2]
        if rows >= cols:
            rh = int(thumbnail_size)
            rw = int(cols * thumbnail_size / rows)
        else:
            rw = int(thumbnail_size)
            rh = int(rows * thumbnail_size / cols)

        rx = int((thumbnail_size - rw) / 2)
        ry = int((thumbnail_size - rh) / 2)

        slice_x = slice(rx, rx + rw)
        slice_y = slice(ry, rx + rh)

        result = np.zeros([thumbnail_size, thumbnail_size, 3], np.uint8)
        result[slice_y, slice_x] = cv2.resize(temp, (rw, rh))

        if name:
            result = np.float32(result)
            result[rh-27:rh] *= 0.5
            result = np.asarray(result, np.uint8)

            cv2.putText(
                result,
                name,
                (0, rh-7),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.7,
                (255, 255, 255),
                1,
                cv2.LINE_AA)

        return result


class Pair:
    def __init__(self, first, second):
        self.first = first
        self.second = second


# data for each person that currently in process
class FaceData:
    def __init__(self):
        self.sample = None
        self.lost = True
        self.weak = False
        self.frame_id = False
        self.lost_time = 0
        self.match_database_index = -1

        self.draw_multilier = 1

        self.sti_person_id_set = False
        self.sti_person_id = 0

        self.age_gender_set = False
        self.age_gender = None 

        self.emotions_set = False
        self.emotions = None

        self.active_liveness_status = None


# data for drawing
class DrawingData:
    def __init__(self):
        self.updated = False

        self.frame = np.array([])
        self.depth = np.array([])
        self.frame_id = 0

        self.faces = dict()
        self.sti_persons_data = list()


class Mutex:
    def __init__(self, mutex):
        self._mutex = mutex

    def wait_one(self):
        self._mutex.acquire()

    def release_mutex(self):
        self._mutex.release()


class QueueExt(Queue):
    def peek(self):
        deq = self.queue
        if deq:
            return deq[0]


def get_enum_name(wnum_val):
    return wnum_val.name.lower().split('_')[1]


class Worker:
    thumbnail_size = 150

    def __init__(self, database: Database,
                 video_worker: video_worker.VideoWorker,
                 source: ImageAndDepthSource,
                 stream_id: int,
                 draw_image_mutex: Mutex,
                 draw_image,
                 frame_fps_limit: float):
        # check arguments
        assert video_worker
        assert source
        assert 0 <= stream_id < video_worker.get_streams_count()

        self.database = database
        self.video_worker = video_worker
        self.source = source
        self.stream_id = stream_id
        self.frame_min_delay_ms = 1000 / frame_fps_limit

        self.drawing_data = DrawingData()
        self.drawing_data_mutex = Mutex(threading.Lock())
        self.frames_mutex = Mutex(threading.Lock())
        self.frames = QueueExt()

        # constants for drawing
        self.draw_border = 5
        self.max_count_in_stripe = 6

        self.draw_image_mutex = draw_image_mutex
        self.draw_image = draw_image

        # set callbacks arguments
        self._tracking_callback_id = video_worker.add_tracking_callback_u(self.tracking_callback, self)

        self._tracking_lost_callback_id = video_worker.add_tracking_lost_callback_u(self.tracking_lost_callback, self)

        self._match_found_callback_id = video_worker.add_match_found_callback_u(self.match_found_callback, self)

        self._sti_person_outdated_callback_id =\
            video_worker.add_sti_person_outdated_callback_u(self.sti_person_outdated_callback, self)

        # start threads
        self._shutdown = False

        self._camera_thread = Thread(target=self.camera_thread_func)
        self._drawing_thread = Thread(target=self.drawing_thread_func)
        self._camera_thread.start()
        self._drawing_thread.start()

    def dispose(self):
        # remove callbacks
        self.video_worker.remove_tracking_callback(self._tracking_callback_id)
        self.video_worker.remove_tracking_lost_callback(self._tracking_lost_callback_id)
        self.video_worker.remove_match_found_callback(self._match_found_callback_id)
        self.video_worker.remove_sti_person_outdated_callback(self._sti_person_outdated_callback_id)

        # stop_threads
        self._shutdown = True
        self._camera_thread.join()
        self._drawing_thread.join()

    @staticmethod
    def tracking_callback(
                data: video_worker.TrackingCallbackData,
                userdata):
        # check arguments
        assert userdata

        stream_id = data.stream_id
        frame_id = data.frame_id
        samples = data.samples
        samples_weak = data.samples_weak
        samples_quality = data.samples_quality

        assert len(samples) == len(samples_weak)
        assert len(samples) == len(samples_quality)

        worker = userdata

        # we care only about the worker._stream_id source
        # so just ignore any others
        if stream_id != worker.stream_id:
            return

        # get the frame with frame_id id
        frame = None
        worker.frames_mutex.wait_one()

        # searching in worker.frames
        while True:
            # there already must be some frames
            assert not worker.frames.empty()

            # this frame_id can't be from future
            assert worker.frames.peek().first <= frame_id

            if worker.frames.peek().first == frame_id:
                # we found it
                frame = worker.frames.get().second
                break
            else:
                # frame was skipped by tracking
                print(f"skipped {stream_id}: {worker.frames.get().first}")

        worker.frames_mutex.release_mutex()

        assert frame is not None

        # update the data
        worker.drawing_data_mutex.wait_one()

        # frame
        worker.drawing_data.frame_id = frame_id
        worker.drawing_data.updated = True
        if len(frame.depth_image):
            worker.drawing_data.depth = frame.depth_image
        worker.drawing_data.frame = np.reshape(frame.color_image, [frame.color_height, frame.color_width, 3])

        # and samples
        for i in range(len(samples)):
            key = samples[i].get_id()
            if key not in worker.drawing_data.faces.keys():
                worker.drawing_data.faces[key] = FaceData()

            face = worker.drawing_data.faces[key]
            face.frame_id = samples[i].get_frame_id()  # it can differ from the frame_id
            face.lost = False
            face.weak = samples_weak[i]
            face.sample = samples[i]
            face.sti_person_id_set = False
            face.active_liveness_status = data.samples_active_liveness_status[i]

            if data.samples_track_emotions_set[i]:
                face.emotions_set = True
                face.emotions = data.samples_track_emotions[i]

            if data.samples_track_age_gender[i]:
                face.age_gender_set = True
                face.age_gender = data.samples_track_age_gender[i]

        worker.drawing_data_mutex.release_mutex()

    @staticmethod
    def tracking_lost_callback(
            data: video_worker.TrackingLostCallbackData,
            userdata):
        assert userdata

        stream_id = data.stream_id
        track_id = data.track_id
        best_quality_sample = data.best_quality_sample

        # userdata is supposed to be pointer to this Worker
        # so cast the pointer
        worker = userdata

        # we care only about the worker._stream_id source
        # so just ignore any others
        if stream_id != worker.stream_id:
            return

        # here we just remember the time when the face was lost
        # but the real purpose of this callback if to store faces
        # for futher matching or processing
        worker.drawing_data_mutex.wait_one()
        face = worker.drawing_data.faces[track_id]

        assert not face.lost

        face.lost = True
        face.lost_time = int(time.time() * 1000)
        if best_quality_sample:
            face.sample = best_quality_sample

        worker.drawing_data_mutex.release_mutex()

    @staticmethod
    def match_found_callback(
            data: video_worker.MatchFoundCallbackData,
            userdata):
        assert userdata

        stream_id = data.stream_id
        sample = data.sample
        templ = data.templ
        search_result = data.search_results

        # userdata is supposed to be pointer to this Worker
        # so cast the pointer
        worker = userdata

        # we care only about the worker._stream_id source
        # so just ignore any others
        if stream_id != worker.stream_id:
            return

        assert sample
        assert templ
        assert search_result

        for i in range(len(search_result)):
            element_id = search_result[i].element_id

            if element_id == video_worker.MATCH_NOT_FOUND_ID:
                assert i == 0, "element_id == video_worker.MATCH_NOT_FOUND_ID and i != 0"
            else:
                assert element_id < len(worker.database.names), f"wrong element_id: {element_id}"

        element_id = search_result[0].element_id

        if element_id != video_worker.MATCH_NOT_FOUND_ID:
            assert element_id < len(worker.database.thumbnails)

            # set the match info in the worker._drawing_data.faces
            # with the best search result
            worker.drawing_data_mutex.wait_one()

            face = worker.drawing_data.faces[sample.get_id()]

            assert not face.lost

            face.match_database_index = element_id

            worker.drawing_data_mutex.release_mutex()

    @staticmethod
    def sti_person_outdated_callback(
            data: video_worker.StiPersonOutdatedCallbackData,
            userdata):
        assert userdata

        stream_id = data.stream_id

        # userdata is supposed to be pointer to this Worker
        # so cast the pointer
        worker = userdata

        # we care only about the worker._stream_id source
        # so just ignore any others
        if stream_id != worker.stream_id:
            return

        worker.drawing_data_mutex.wait_one()

        it = None
        i = 0
        for i, it in enumerate(worker.drawing_data.sti_persons_data):
            if it.first == data.sti_person_id:
                break

        assert it != worker.drawing_data.sti_persons_data[-1]

        worker.drawing_data.sti_persons_data.pop(i)

        worker.drawing_data_mutex.release_mutex()

    def camera_thread_func(self):
        try:
            prev_capture_time = time.time() * 1000

            while True:
                # check for stop
                if self._shutdown:
                    break

                # sleep some time to get stable frequency
                if self.frame_min_delay_ms > 0:
                    now = time.time() * 1000
                    sleep_time_ms = max(self.frame_min_delay_ms - (now - prev_capture_time), 0)
                    time.sleep(sleep_time_ms / 1000)

                    prev_capture_time = time.time() * 1000

                data = ImageAndDepth()
                self.source.get(data)

                if not len(data.color_image):
                    self._shutdown = True
                elif data.color_format == raw_image.Format.FORMAT_BGR:
                    image = np.reshape(data.color_image, [data.color_height, data.color_width, 3])
                    cvri = CVRawImage(image)

                    frame_id = self.video_worker.add_video_frame(cvri, self.stream_id)

                    self.frames_mutex.wait_one()

                    new_queue_element = Pair(frame_id, data)
                    self.frames.put(new_queue_element)

                    self.frames_mutex.release_mutex()

        except Exception as ex:
            print('\n Worker.CameraThreadFunc exception: "{}"'.format(ex))

    @staticmethod
    def puttext(image,
                text: str,
                position: Point,
                size=1.0):
        # twice - for better reading
        # since we are drawing on the frame from webcam

        line_test_height = 6.0 + size * 16

        # white background
        org = np.int32(position + Point(0, line_test_height))[:2]
        org = tuple(org)

        cv2.putText(image,
                    text,
                    org,
                    cv2.FONT_HERSHEY_COMPLEX,
                    0.7 * size,
                    (255, 255, 255),
                    5,
                    cv2.LINE_AA)

        # black text
        cv2.putText(image,
                    text,
                    org,
                    cv2.FONT_HERSHEY_COMPLEX,
                    0.7 * size,
                    (0, 0, 0),
                    1,
                    cv2.LINE_AA)

        return line_test_height

    def draw(self, data: DrawingData, database: Database):
        stripe_width = self.thumbnail_size * 2 + self.draw_border * 2
        stripe_height = (self.thumbnail_size + self.draw_border) * self.max_count_in_stripe - self.draw_border

        frame_rows, frame_cols = data.frame.shape[:2]
        depth_rows, depth_cols = data.depth.shape[:2] if data.depth.size else (0, 0)

        # image for draw the frame and the stripe
        result = np.zeros([max(frame_rows + depth_rows, stripe_height), max(frame_cols + depth_cols, stripe_width), 3], dtype=np.uint8)

        # copy the frame
        result_rows, result_cols = result.shape[:2]

        frame_y_offset = (result_rows - frame_rows - depth_rows) // 2
        depth_y_offset = frame_y_offset + frame_rows

        slice_y = slice(frame_y_offset, frame_y_offset + frame_rows)
        slice_x = slice(0, frame_cols)

        result[slice_y, slice_x] = data.frame

        if data.depth.size:
            # TODO optimize it cycle with numpy
            for i in range(data.depth.shape[0]):
                for j in range(data.depth.shape[1]):
                    result[i + depth_y_offset, j] *= 0
                    depth_pixel = data.depth[i, j]

                    if depth_pixel == 0:
                        continue

                    if depth_pixel < 255:
                        result[i + depth_y_offset, j, 2] = 256 - depth_pixel

                    if depth_pixel < 255 * 2:
                        result[i + depth_y_offset, j, 1] = depth_pixel // 2
                    else:
                        result[i + depth_y_offset, j, 1] = 255

                    if depth_pixel < 255 * 4:
                        result[i + depth_y_offset, j, 0] = depth_pixel // 4
                    else:
                        result[i + depth_y_offset, j, 0] = 255

        # clone faces data for random access
        faces = []
        for key, value in data.faces.items():
            faces.append((key, value))

        # make order with recognized first
        # List[ tuple<match_database_index, index in faces> ]
        order = list()
        for i in range(len(faces)):
            order.append((faces[i][1].match_database_index, i))
        order.sort()

        # draw alive faces
        for oi in range(len(order)):
            face = faces[order[oi][1]][1]
            assert face

            # draw circles of faces appeared on this frame
            if face.frame_id == data.frame_id and (not face.lost):
                # get points
                points = face.sample.get_landmarks()

                # compute center
                center = Point(0, 0)
                for pt in points:
                    center += pt
                center *= 1.0 / len(points)

                # compute radius
                radius = 0
                for pt in points:
                    radius += cv2.norm(np.array(pt - center))
                radius /= len(points)

                switch_dict = {
                    31: 2.5,    # lbf
                    21: 2.1,    # fda
                    47: 1.9,    # esr
                    470: 2.0,   # mesh
                }

                radius *= switch_dict[len(points)]

                # choose color
                # green color for recognized, red color for unrecognized
                color = (0, 0, 255) if face.match_database_index < 0 else (0, 255, 0)

                for k in range(2 if data.depth.size else 1):
                    y_offset = frame_y_offset if k == 0 else depth_y_offset

                    circ_center = (center + Point(0, y_offset))[:2]
                    circ_center = tuple(np.int32(list(circ_center)))
                    circ_radius = int(radius)
                    circ_axes = (circ_radius, circ_radius)
                    circ_thick = 3

                    # for pt in points:
                    #     pt += Point(0, y_offset)
                    #     cv2.circle(result,
                    #                (int(pt.x), int(pt.y)),
                    #                1,
                    #                (0, 255, 0),
                    #                cv2.LINE_AA)

                    # dashed circle for weak face samples
                    if face.weak:
                        # draw dashed circle for weak samples
                        n = 8
                        for i in range(n):
                            cv2.ellipse(result,
                                        circ_center,
                                        circ_axes,
                                        (face.frame_id * 2) % 360,
                                        (i * 2 + 0) * 180 / n,
                                        (i * 2 + 1) * 180 / n,
                                        color,
                                        thickness=circ_thick,
                                        lineType=cv2.LINE_AA)
                    else:
                        cv2.circle(result,
                                   circ_center,
                                   circ_radius,
                                   color,
                                   thickness=circ_thick,
                                   lineType=cv2.LINE_AA)

                text_position = Point(
                    center.x + radius * 0.7,
                    frame_y_offset + center.y - radius * 0.5,
                )
                if face.active_liveness_status.verdict != active_liveness.Liveness.NOT_COMPUTED:
                    active_liveness_str = ""
                    if face.active_liveness_status.verdict == active_liveness.Liveness.WAITING_FACE_ALIGN:
                        active_liveness_str += active_liveness.Liveness.WAITING_FACE_ALIGN.name.lower()
                    elif face.active_liveness_status.verdict == active_liveness.Liveness.ALL_CHECKS_PASSED:
                        active_liveness_str += active_liveness.Liveness.ALL_CHECKS_PASSED.name.lower()
                    elif face.active_liveness_status.verdict == active_liveness.Liveness.CHECK_FAIL:
                        active_liveness_str += active_liveness.Liveness.CHECK_FAIL.name.lower()
                    else:
                        active_liveness_str += face.active_liveness_status.check_type.name.lower()
                        active_liveness_str += ": "
                        active_liveness_str += face.active_liveness_status.verdict.name.lower()
                        active_liveness_str += " " + str(face.active_liveness_status.progress_level)
                    text_position.y += self.puttext(result, active_liveness_str, text_position)

                if face.age_gender_set:
                    text_position.y += self.puttext(
                        result,
                        get_enum_name(face.age_gender.gender),
                        text_position
                    )

                    text_position.y += self.puttext(
                        result,
                        get_enum_name(face.age_gender.age),
                        text_position
                    )

                    age_years_text = "years: " + str(round(face.age_gender.age_years, 3))

                    text_position.y += self.puttext(
                        result,
                        age_years_text,
                        text_position
                    )

                if face.emotions_set:
                    for face_emotion in face.emotions:
                        text_position.y += self.puttext(
                            result,
                            get_enum_name(face_emotion.emotion),
                            text_position,
                            face_emotion.confidence * 0.75 + 0.25
                        )

                # iris_points = face.sample.get_iris_landmarks()
                # for j in range(len(iris_points)):
                #     ms = 1
                #     color = (0, 255, 255)
                #     oi = j - 20 * (j >= 20)
                #     pt1 = Point(0, frame_y_offset) + iris_points[j]
                #     pt2 = Point(0, frame_y_offset) + iris_points[(j if oi < 19 else j - 15) + 1]
                #
                #     if oi < 5:
                #         color = (0, 165, 255)
                #         if oi == 0:
                #             radius = np.sqrt((pt1.x - pt2.x)**2 + (pt1.y - pt2.y)**2)
                #             cv2.circle(result, (int(pt1.x), int(pt2.y)), int(radius), color, ms)
                #     else:
                #         cv2.line(result, (int(pt1.x), int(pt1.y)), (int(pt2.x), int(pt2.y)), color, ms)
                #     cv2.circle(result, (int(pt1.x), int(pt1.y)), ms, color, -1)

            # no - draw the stripe
            if oi < self.max_count_in_stripe:
                # place for thumbnail from the frame
                shift = 2 * self.thumbnail_size + 15
                sample_rect_slice = (
                    slice((self.thumbnail_size + self.draw_border) * oi,
                          (self.thumbnail_size + self.draw_border) * oi + self.thumbnail_size),
                    slice(data.frame.shape[1] - shift + self.draw_border,
                          data.frame.shape[1] - shift + self.draw_border + self.thumbnail_size),
                )

                # place for thumbnail of sti match
                sti_sample_rect_slice = (
                    slice(sample_rect_slice[0].start + self.thumbnail_size - self.thumbnail_size // 3,
                          sample_rect_slice[0].start + self.thumbnail_size),
                    slice(sample_rect_slice[1].start + self.thumbnail_size - self.thumbnail_size // 3,
                          sample_rect_slice[1].start + self.thumbnail_size)
                )

                # place for thumbnail from the database
                match_rect_slice = (
                    slice((self.thumbnail_size + self.draw_border) * oi,
                          (self.thumbnail_size + self.draw_border) * oi + self.thumbnail_size),
                    slice(data.frame.shape[1] - shift + self.draw_border * 2 + self.thumbnail_size,
                           data.frame.shape[1] - shift + (self.draw_border + self.thumbnail_size) * 2),
                )

                # make thumbnail from the frame
                assert face.sample
                thumb = Database.make_thumbnail(face.sample)

                result[sample_rect_slice[0], sample_rect_slice[1]] = thumb

                # draw match from short time identification if available
                if face.sti_person_id_set:
                    sti_data_it = None
                    i = 0
                    for i, pair in enumerate(data.sti_persons_data):
                        if pair.first == face.sti_person_id:
                            sti_data_it = pair
                            break

                    assert sti_data_it != data.sti_persons_data[i]
                    sti_data = sti_data_it.second
                    assert sti_data.first_sample
                    assert sti_data.first_sample.get_id() == face.sti_person_id

                    if face.sample.get_id() == face.sti_person_id:
                        # this must be the same sample, no reason to draw it again
                        assert face.sample == sti_data.first_sample
                    else:
                        thumb = Database.make_thumbnail(sti_data.first_sample, "", self.thumbnail_size // 3)
                        result[sti_sample_rect_slice] = thumb

                # fade if image is lost
                if face.draw_multilier < 1:
                    result[sample_rect_slice] = result[sample_rect_slice] * face.draw_multilier

                if face.match_database_index < 0:
                    # gray color for unrecognized persons
                    result[match_rect_slice] = 128 * face.draw_multilier
                else:
                    # thumbnail from the database for recognized persons
                    result[match_rect_slice] = database.thumbnails[face.match_database_index]

                    # fade if image is lost
                    if face.draw_multilier < 1:
                        result[match_rect_slice] = result[match_rect_slice] * face.draw_multilier
        return result

    # here we draw results
    def drawing_thread_func(self):
        while True:
            # check for stop
            if self._shutdown:
                break

            # we don't need to redraw too often
            time.sleep(20 / 1000)

            # get the data
            self.drawing_data_mutex.wait_one()

            # remove lost faces after 5 seconds
            now = int(time.time() * 1000)
            timeout = 5000      # ms - i.e. 5 seconds

            erase_list = []
            for key, face in self.drawing_data.faces.items():
                # ignore alive faces
                if not face.lost:
                    continue

                self.drawing_data.updated = True

                delay = now - face.lost_time

                if delay < timeout:
                    # lost faces will fade while this timeout
                    face.draw_multilier = 1 - delay / timeout
                else:
                    erase_list.append(key)

            for key in erase_list:
                self.drawing_data.faces.pop(key)

            data = self.drawing_data
            self.drawing_data.updated = False

            self.drawing_data_mutex.release_mutex()

            # draw
            if not data.frame.size:
                continue

            drawed = self.draw(data, self.database)

            # and store it (it will be used in imshow in the main)
            self.drawing_data_mutex.wait_one()
            self.draw_image = drawed
            self.drawing_data_mutex.release_mutex()
