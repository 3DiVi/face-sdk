import argparse
from sys import platform

from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.modules import video_worker, active_liveness

from src import OpenCVSource, ImageAndDepth, Database, Mutex, Worker

import cv2
import numpy as np
import threading


def parse_args():
    if platform == "win32":
        default_dll_path = "facerec.dll"
    else:
        default_dll_path = "../../../lib/libfacerec.so"

    parser = argparse.ArgumentParser(description='Video Recognition Demo')
    parser.add_argument('--dll_path', default=default_dll_path, type=str, help='Face SDK library path')
    parser.add_argument('--config_dir', default="../../../conf/facerec", type=str, help='Face SDK config dir')
    parser.add_argument('--license_dir', default="", type=str, help='Face SDK license dir')
    parser.add_argument('--database_list_filepath', type=str, required=True)
    parser.add_argument('--method_config', type=str, required=True)
    parser.add_argument('--fullscreen', default="yes", type=str)
    parser.add_argument('--vw_config_file', default="video_worker_fdatracker.xml", type=str)
    parser.add_argument('--frame_fps_limit', default=25, type=float)
    parser.add_argument('--recognition_far_threshold', type=float, required=True)
    parser.add_argument("sources_names", nargs="+")

    return parser.parse_args()


def main():
    try:
        args = parse_args()

        assert args.recognition_far_threshold > 0

        sources = []

        for sname in args.sources_names:
            sources.append(OpenCVSource(sname))

        service = FacerecService.create_service(args.dll_path, args.config_dir)

        print("Library version: ", service.get_version(), "\n")

        recognizer = service.create_recognizer(args.method_config, True, True)
        recognition_distance_threshold = recognizer.get_roc_curve_point_by_far(args.recognition_far_threshold).distance

        capturer_config = Config("common_capturer4_fda_singleface.xml")
        capturer = service.create_capturer(capturer_config)

        # create database
        database = Database(
            args.database_list_filepath,
            recognizer,
            capturer,
            recognition_distance_threshold
        )

        # create one VideoWorker
        vw_config = Config(args.vw_config_file)
        vw_config.override_parameter("search_k", 10)
        vw_config.override_parameter("not_found_match_found_callback", 1)
        vw_config.override_parameter("downscale_rawsamples_to_preferred_size", 0)

        vw_params = video_worker.Params()
        vw_params.video_worker_config = vw_config
        vw_params.recognizer_ini_file = args.method_config
        vw_params.streams_count = len(sources)
        vw_params.processing_threads_count = len(sources)
        vw_params.matching_threads_count = len(sources)
        #####
        vw_params.age_gender_estimation_threads_count = len(sources)
        vw_params.emotions_estimation_threads_count = len(sources)
        #####
        # vw_params.active_liveness_checks_order = [
        #     active_liveness.CheckType.SMILE,
        #     active_liveness.CheckType.TURN_RIGHT,
        #     active_liveness.CheckType.TURN_LEFT]
        #####

        vw = service.create_video_worker(vw_params)

        # set database
        vw.set_database(database.vw_elements)

        for sname in args.sources_names:
            if args.fullscreen == "yes":
                cv2.namedWindow(
                    sname,
                    cv2.WINDOW_NORMAL
                )
                cv2.setWindowProperty(
                    sname,
                    cv2.WND_PROP_FULLSCREEN,
                    cv2.WINDOW_FULLSCREEN
                )

            cv2.imshow(sname, np.zeros([100, 100, 3], dtype=np.uint8))

        # prepare buffers for store drawed results
        draw_images_mutex = Mutex(threading.Lock())
        draw_images = [np.array([])] * len(sources)

        # create one worker per one source
        workers = list()

        for i in range(len(sources)):
            workers.append(
                Worker(
                    database,
                    vw,
                    sources[i],
                    i,      # stream_id
                    draw_images_mutex,
                    draw_images[i],
                    args.frame_fps_limit
                )
            )

        # draw results until escape pressed
        while True:
            draw_images_mutex.wait_one()

            i = 0
            for worker in workers:
                if worker._shutdown:
                    worker.dispose()
                    workers.remove(worker)
                    cv2.destroyWindow(args.sources_names[i])
                    args.sources_names.remove(args.sources_names[i])
                    i -= 1
                elif worker.draw_image.size:
                    cv2.imshow(
                        args.sources_names[i],
                        worker.draw_image
                    )
                    worker.draw_image = np.array([])
                i += 1

            draw_images_mutex.release_mutex()

            key = cv2.waitKey(20)

            if 27 == key or not workers:
                cv2.destroyAllWindows()
                break

            if ord('t') == key:
                print("resetTrackerOnStream")
                vw.reset_tracker_on_stream(0)

            if ord('r') == key:
                track_id_threshold = vw.reset_stream(0)
                print("resetStream return track_id_threshold: ", track_id_threshold)

            if ord("\r") == key:
                vw.disable_processing_on_stream(0)
                print("disable_processing_on_stream: ")

            if ord(' ') == key:
                vw.enable_processing_on_stream(0)
                print("enable_processing_on_stream: ")

            if ord('n') == key:
                vw.disable_age_gender_estimation_on_stream(0)
                print("disable_age_gender_estimation_on_stream: ")

            if ord('h') == key:
                vw.enable_age_gender_estimation_on_stream(0)
                print("disable_age_gender_estimation_on_stream: ")

            if ord('m') == key:
                vw.disable_emotions_estimation_on_stream(0)
                print("disable_emotions_estimation_on_stream: ")

            if ord('j') == key:
                vw.enable_emotions_estimation_on_stream(0)
                print("enable_emotions_estimation_on_stream: ")

            # check exceptions in callbacks
            vw.check_exception()

        for w in workers:
            w.dispose()

    except Exception as ex:
        print('\n video_recognition_demo exception catched: "{}"'.format(ex))


if __name__ == '__main__':
    main()
