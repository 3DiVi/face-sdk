/**
	\file video_recognition_demo/src/Worker.cpp
*/

#include "Database.h"
#include "MAssert.h"
#include "Worker.h"

#include <sstream>

#include <signal.h>


#ifndef CV_AA
#define CV_AA cv::LINE_AA
#endif

Worker::Worker(
	const Database &database,
	const pbio::VideoWorker::Ptr video_worker,
	const cv::Ptr<ImageAndDepthSource> source,
	const int stream_id,
	std::mutex &draw_image_mutex,
	cv::Mat &draw_image,
	const float frame_fps_limit,
	const bool show_metrics) :
_draw_image_mutex(draw_image_mutex),
_draw_image(draw_image),
_video_worker(video_worker),
_source(source),
_frame_min_delay_ms(1000.f / frame_fps_limit),
_stream_id(stream_id),
_database(database),
_is_metrics_collection_mode(show_metrics)
{
	// check arguments
	MAssert(video_worker,);
	MAssert(source,);
	MAssert(stream_id >= 0 && stream_id < video_worker->getStreamsCount(),);

	// set callbacks arguments
	_tracking_callback_id =
		video_worker->addTrackingCallbackU(
			TrackingCallback,
			this);

	_tracking_lost_callback_id =
		video_worker->addTrackingLostCallbackU(
			TrackingLostCallback,
			this);

	_match_found_callback_id =
		video_worker->addMatchFoundCallbackU(
			MatchFoundCallback,
			this);

	_sti_person_outdated_callback_id =
		video_worker->addStiPersonOutdatedCallbackU(
			StiPersonOutdatedCallback,
			this);

	_template_created_callback_id = 
		video_worker->addTemplateCreatedCallbackU(
			TemplateCreatedCallback,
			this);

	// start threads
	_shutdown = false;

	_camera_thread = std::thread(&Worker::CameraThreadFunc, this);
	_drawing_thread = std::thread(&Worker::DrawingThreadFunc, this);

	if(_is_metrics_collection_mode)
		_print_log = std::thread(&Worker::PrintThreadFunc, this);
		this->ts_begin = std::chrono::steady_clock::now();
}

Worker::~Worker()
{
	// remove callbacks
	_video_worker->removeTrackingCallback(_tracking_callback_id);
	_video_worker->removeTrackingLostCallback(_tracking_lost_callback_id);
	_video_worker->removeMatchFoundCallback(_match_found_callback_id);
	_video_worker->removeStiPersonOutdatedCallback(_sti_person_outdated_callback_id);

	// stop threads
	_shutdown = true;

	if(_camera_thread.joinable())
		_camera_thread.join();

	if(_drawing_thread.joinable())
		_drawing_thread.join();
}

void Worker::TemplateCreatedCallback(
	const pbio::VideoWorker::TemplateCreatedCallbackData &data,
	void* const userdata)
{
	const int stream_id = data.stream_id;

	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	// we care only about the worker._stream_id source
	// so just ignore any others
	if(stream_id != worker._stream_id)
		return;

	worker._template_counter += 1;
}

// static
void Worker::TrackingCallback(
	const pbio::VideoWorker::TrackingCallbackData &data,
	void* const userdata)
{
	// check arguments
	MAssert(userdata,);

	const int stream_id = data.stream_id;
	const int frame_id = data.frame_id;
	const std::vector<pbio::RawSample::Ptr> &samples = data.samples;
	const std::vector<bool> &samples_weak = data.samples_weak;
	const std::vector<float> &samples_quality = data.samples_quality;

	MAssert(samples.size() == samples_weak.size(),);
	MAssert(samples.size() == samples_quality.size(),);

	// userdata is supposed to be pointer to this Worker
	// so cast the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	// we care only about the worker._stream_id source
	// so just ignore any others
	if(stream_id != worker._stream_id)
		return;

	for(auto& it : data.samples_track_id)
	{
		if(it > worker._max_track)
		{
			worker._max_track = it;
			worker._track_counter += 1;
		}
	}

	// get the frame with frame_id id
	SharedImageAndDepth frame;

	{
		const std::lock_guard<std::mutex> guard(worker._frames_mutex);

		// searching in worker._frames
		for(;;)
		{
			// there already must be some frames
			MAssert(worker._frames.size() > 0,);

			// this frame_id can't be from future
			MAssert(worker._frames.front().first <= frame_id,);


			if(worker._frames.front().first == frame_id)
			{
				// we found it
				frame = worker._frames.front().second;
				worker._frames.pop();
				break;
			}
			else
			{
				// this (i.e. the worker._frames.front() ) frame was skipeed by tracking
				std::cout << "skiped " << stream_id << ":" << worker._frames.front().first << std::endl;
				worker._frames.pop();
			}
		}
	}

	// update the data
	{
		const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

		// frame
		worker._drawing_data.frame = frame.image;
		worker._drawing_data.depth = frame.depth;
		worker._drawing_data.frame_id = frame_id;
		worker._drawing_data.updated = true;

		worker._face_flag.frame_id = samples.size();
		worker._face_flag.updated = true;

		// and samples
		for(size_t i = 0; i < samples.size(); ++i)
		{
			FaceData &face = worker._drawing_data.faces[samples[i]->getID()];
			face.frame_id = samples[i]->getFrameID();  // it can differ from the frame_id
			face.lost = false;
			face.weak = samples_weak[i];
			face.sample = samples[i];
			face.sti_person_id_set = false;

			face.active_liveness_status = data.samples_active_liveness_status[i];

			if(data.samples_track_emotions_set[i])
			{
				face.emotions_set = true;
				face.emotions = data.samples_track_emotions[i];
			}

			if(data.samples_track_age_gender_set[i])
			{
				face.age_gender_set = true;
				face.age_gender = data.samples_track_age_gender[i];
			}
		}
	}
}


void Worker::StiPersonOutdatedCallback(
	const pbio::VideoWorker::StiPersonOutdatedCallbackData &data,
	void* const userdata)
{
	MAssert(userdata,);

	const int stream_id = data.stream_id;

	// userdata is supposed to be pointer to this Worker
	// so cast the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	// we care only about the worker._stream_id source
	// so just ignore any others
	if(stream_id != worker._stream_id)
		return;

	const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

	// std::cout << "\n\n StiPersonOutdatedCallback: data.sti_person_id: " << data.sti_person_id << "\n\n" << std::endl;

	const std::map<int, StiPersonData>::iterator it = worker._drawing_data.sti_persons_data.find(data.sti_person_id);

	MAssert(it != worker._drawing_data.sti_persons_data.end(),);

	worker._drawing_data.sti_persons_data.erase(it);
}


// static
void Worker::TrackingLostCallback(
	const pbio::VideoWorker::TrackingLostCallbackData &data,
	void* const userdata)
{
	MAssert(userdata,);

	const int stream_id = data.stream_id;
	const int track_id = data.track_id;
	const pbio::RawSample::Ptr &best_quality_sample = data.best_quality_sample;

	// userdata is supposed to be pointer to this Worker
	// so cast the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	// we care only about the worker._stream_id source
	// so just ignore any others
	if(stream_id != worker._stream_id)
		return;

	// here we just remember the time when the face was lost
	// but the real purpose of this callback if to store faces
	// for futher matching or processing

	{
		const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

		FaceData &face = worker._drawing_data.faces[track_id];

		MAssert(!face.lost,);

		face.lost = true;
		face.lost_time = std::chrono::steady_clock::now();
		if (best_quality_sample)
			face.sample = best_quality_sample;

		face.sti_person_id_set = data.sti_person_id_set;
		face.sti_person_id = data.sti_person_id;

		if(data.sti_person_id_set)
		{
			MAssert(best_quality_sample, );

			StiPersonData &sti_data = worker._drawing_data.sti_persons_data[data.sti_person_id];

			if(!sti_data.first_sample)
			{
				MAssert(track_id == data.sti_person_id, );
				sti_data.first_sample = best_quality_sample;
			}
			else
			{
				MAssert(track_id != data.sti_person_id, );
			}
		}
	}
}


// static
void Worker::MatchFoundCallback(
	const pbio::VideoWorker::MatchFoundCallbackData &data,
	void* const userdata)
{
	MAssert(userdata,);

	const int stream_id = data.stream_id;
	const pbio::RawSample::Ptr &sample = data.sample;
	const pbio::Template::Ptr &templ = data.templ;
	const std::vector<pbio::VideoWorker::SearchResult> &search_result = data.search_results;

	// userdata is supposed to be pointer to this Worker
	// so cast the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	// we care only about the worker._stream_id source
	// so just ignore any others
	if(stream_id != worker._stream_id)
		return;

	MAssert(sample,);
	MAssert(templ,);
	MAssert(!search_result.empty(),);

	// just print distances in the console
	// std::cout << "stream " << stream_id << " match track " << sample->getID() << " : \n";

	for(size_t i = 0; i < search_result.size(); ++i)
	{
		const uint64_t element_id = search_result[i].element_id;

		if(element_id == pbio::VideoWorker::MATCH_NOT_FOUND_ID)
		{
			MAssert(i == 0,);
			// std::cout << "  " << i << ":  MATCH NOT FOUND" << std::endl;
		}
		else
		{
			MAssert(element_id < worker._database.names.size(), );

			// std::cout << "  " << i << ": "
			// 	" with '" << worker._database.names[element_id] <<
			// 	"'  distance: " << search_result[i].match_result.distance << "\n";
		}
	}
	// std::cout << std::endl;

	const uint64_t element_id = search_result[0].element_id;

	if(element_id != pbio::VideoWorker::MATCH_NOT_FOUND_ID)
	{
		MAssert(element_id < worker._database.thumbnails.size(), );

		// set the match info in the worker._drawing_data.faces
		// with the best search result
		const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

		FaceData &face = worker._drawing_data.faces[sample->getID()];

		MAssert(!face.lost,);

		face.match_database_index = element_id;

		worker._match_counter += 1;
	}
}

void Worker::CameraThreadFunc()
{
	try
	{
		std::chrono::steady_clock::time_point prev_capture_time = std::chrono::steady_clock::now();

		for(;;)
		{
			// check for stop
			if(_shutdown)
				break;

			// sleep some time to get stable frequency
			if (_frame_min_delay_ms > 0)
			{
				std::this_thread::sleep_until(prev_capture_time + std::chrono::milliseconds(_frame_min_delay_ms));

				prev_capture_time = std::chrono::steady_clock::now();
			}


			ImageAndDepth data;
			_source->get(data);

			SharedImageAndDepth frame;

#ifdef DEPTH_LIVENESS
			if (!data.depth_image.empty())
			{
				const pbio::DepthMapRaw& depth_opts = data.make_dmr();

				_video_worker->addDepthFrame(depth_opts, _stream_id, data.depth_timestamp_microsec);

				frame.depth = cv::Mat(
					depth_opts.depth_map_rows,
					depth_opts.depth_map_cols,
					CV_16UC1,
					(void*)data.depth_image.data(),
					depth_opts.depth_data_stride_in_bytes)
				.clone();

				_last_depth_image = frame.depth;
			}
#endif
			// put a frame in the VideoWorker
			if (!data.color_image.empty())
			{
				frame.image = cv::Mat(
					data.color_height,
					data.color_width,
					CV_8UC3,
					(void*)data.color_image.data(),
					data.color_stride_in_bytes)
				.clone();

				if (data.color_format != pbio::IRawImage::FORMAT_BGR)
				{
					cv::cvtColor(frame.image, frame.image, cv::COLOR_RGB2BGR);
				}

				// get the frame
				pbio::CVRawImage cvri;
				cvri.mat() = frame.image;

				const std::lock_guard<std::mutex> guard(_frames_mutex);

#ifndef DEPTH_LIVENESS
				const int frame_id = _video_worker->addVideoFrame(cvri, _stream_id);
#else
				const int frame_id = _video_worker->addVideoFrame(cvri, _stream_id, data.image_timestamp_microsec);

				if (data.depth_image.empty())
					frame.depth = _last_depth_image;
#endif

				// and store it here for further drawing
				_frames.push(std::make_pair(frame_id, frame));
			}
			else if(_is_metrics_collection_mode)
				::raise(SIGINT);
		}
	}
	catch(const std::exception &e)
	{
		std::cerr << ("\n Worker::CameraThreadFunc std::exception: '" +
			std::string( e.what() ) + "'\n") << std::endl;
	}
	catch(...)
	{
		std::cerr << "\n Worker::CameraThreadFunc ... exception.\n" << std::endl;
	}
}



// drawing text with opencv
//   return text line height
inline
float puttext(
	cv::Mat image,
	const std::string text,
	const cv::Point2f position,
	const float size = 1.0f)
{
	// twice - for better reading
	// since we are drawing on the frame from webcam

	const float line_test_height = 6.f + size * 16.f;

	// white background
	cv::putText(
		image,
		text,
		position + cv::Point2f(0, line_test_height),
		cv::FONT_HERSHEY_DUPLEX,
		0.7 * size,
		cv::Scalar::all(255),
		5,
		CV_AA);

	// black text
	cv::putText(
		image,
		text,
		position + cv::Point2f(0, line_test_height),
		cv::FONT_HERSHEY_DUPLEX,
		0.7 * size,
		cv::Scalar::all(0),
		1,
		CV_AA);

	return line_test_height;
}

// static
cv::Mat Worker::Draw(
	const DrawingData &data,
	const Database &database)
{
	// drawing params of stripe with current faces
	const int stripe_width = thumbnail_size * 2 + draw_border * 2;

	const int stripe_height =
		(thumbnail_size + draw_border) * max_count_in_stripe - draw_border;

	// image for draw the frame and the stripe
	cv::Mat result(
		std::max<int>(data.frame.rows + data.depth.rows, stripe_height),
		std::max<int>(data.frame.cols, data.depth.cols) + stripe_width,
		CV_8UC3,
		cv::Scalar::all(0));

	// copy the frame
	const int frame_y_offset = (result.rows - data.frame.rows - data.depth.rows) / 2;
	const int depth_y_offset = frame_y_offset + data.frame.rows;

	data.frame.copyTo(
		result(
			cv::Rect(
				0,
				frame_y_offset,
				data.frame.cols,
				data.frame.rows)));

	{
		for(int i = 0; i < data.depth.rows; ++i)
		{
			cv::Vec3b* const dst = result.ptr<cv::Vec3b>(depth_y_offset + i);
			uint16_t const* const src = data.depth.ptr<uint16_t>(i);

			for(int j = 0; j < data.depth.cols; ++j)
			{
				const uint16_t depth = src[j];
				cv::Vec3b &bgr = dst[j];

				bgr[0] = bgr[1] = bgr[2] = 0;

				if(depth == 0)
					continue;

				if(depth < 256)
					bgr[2] = 256 - depth;

				if(depth < 256 * 2)
					bgr[1] = depth / 2;
				else
					bgr[1] = 255;

				if(depth < 256 * 4)
					bgr[0] = depth / 4;
				else
					bgr[0] = 255;
			}
		}
	}

	// clone faces data for random access
	std::vector<std::pair<int, FaceData> > faces(data.faces.begin(), data.faces.end());

	// make order with recognized first
	// pair<match_database_index, index in faces>
	std::vector<std::pair<int, int> > order(faces.size());
	for(size_t i = 0; i < faces.size(); ++i)
	{
		order[i].first = faces[i].second.match_database_index;
		order[i].second = i;
	}

	std::sort(order.rbegin(), order.rend());

	// draw alive faces
	for(size_t order_i = 0; order_i < order.size(); ++order_i)
	{
		const FaceData &face = faces[order[order_i].second].second;

		MAssert(face.sample,);

		// draw circles of faces appeared on this frame
		if(face.frame_id == data.frame_id && !face.lost)
		{
			// get points
			const std::vector<pbio::RawSample::Point> points =
				face.sample->getLandmarks();
			const std::vector<pbio::RawSample::Point> all_iris_points =
				face.sample->getIrisLandmarks();

			// compute center
			cv::Point2f center(0, 0);

			for(size_t j = 0; j < points.size(); ++j)
			{
				center.x += points[j].x;
				center.y += points[j].y;
			}
			center *= 1.f / points.size();


			// compute radius
			float radius = 0;

			for(size_t j = 0; j < points.size(); ++j)
				radius += cv::norm(cv::Point2f(points[j].x , points[j].y) - center);

			radius /= points.size();

			if(points.size() == 31)  // lbf
				radius *= 2.5f;

			if(points.size() == 21)  // fda
				radius *= 2.1f;

			if(points.size() == 47)  // esr
				radius *= 1.9f;

			if(points.size() == 470)  // mesh
				radius *= 2.f;

			// choose color
			const cv::Scalar color =
				face.match_database_index < 0 ?
					cv::Scalar(0, 0, 255) :  // red color for unrecognized
					cv::Scalar(0, 255, 0);   // green color for recognizerd

			for (int k = 0; k < (data.depth.empty() ? 1 : 2); ++k)
			{
				const int y_offset = (k == 0 ? frame_y_offset : depth_y_offset);

				// dashed circle for weak face samples
				if(face.weak)
				{
					// draw dashed cirle for weak samples
					const int n = 8;
					for(int i = 0; i < n; ++i)
						cv::ellipse(
							result,
							center + cv::Point2f(0.f, y_offset),
							cv::Size(radius, radius),
							(face.frame_id * 2) % 360,
							(i * 2 + 0) * 180 / n,
							(i * 2 + 1) * 180 / n,
							color,
							3,
							CV_AA);
				}
				else
				{
					cv::circle(
						result,
						center + cv::Point2f(0.f, y_offset),
						radius,
						color,
						3,
						CV_AA);
				}
			}


//			// draw iris points
//			for(size_t j = 0; j < all_iris_points.size(); ++j)
//			{
//				float ms = 1;
//				auto color = cv::Scalar(0, 255, 255);
//				int oi = j - 20 * (j >= 20);
//				pbio::RawSample::Point pt1 = all_iris_points[j];
//				pbio::RawSample::Point pt2 = all_iris_points[(oi < 19 ? j : j - 15) + 1];
//				cv::Point2f cv_pt1 = cv::Point2f(pt1.x, pt1.y + frame_y_offset);
//				cv::Point2f cv_pt2 = cv::Point2f(pt2.x, pt2.y + frame_y_offset);
//
//				if(oi < 5)
//				{
//					color = cv::Scalar(0, 165, 255);
//					if (oi == 0)
//					{
//						double radius = cv::norm(cv_pt1 - cv_pt2);
//						cv::circle(result, cv_pt1, radius, color, ms);
//					}
//				}else
//					cv::line(result, cv_pt1, cv_pt2, color, ms);
//
//				cv::circle(result, cv_pt1, ms, color, -1);
//			}

			// draw facial points
			/*for(size_t i = 0; i < points.size(); ++i)
			{
				cv::circle(
					result,
					cv::Point2f(points[i].x, points[i].y + frame_y_offset),
					2,
					cv::Scalar(0, 0, 255),
					-1, CV_AA);
			}*/

			{
				cv::Point2f text_position;
				text_position.x = center.x + radius * 0.7;
				text_position.y = frame_y_offset + center.y - radius * 0.5;

				std::vector<std::string> verdict{
					"all_checks_passed",
					"current_check_passed",
					"check_fail",
					"waiting_face_align",
					"in_progress",
					"not_computed"};

				if(face.active_liveness_status.verdict != pbio::ActiveLiveness::Liveness::NOT_COMPUTED) {
					std::string active_liveness = "";
					if (face.active_liveness_status.verdict == pbio::ActiveLiveness::WAITING_FACE_ALIGN ||
						face.active_liveness_status.verdict == pbio::ActiveLiveness::ALL_CHECKS_PASSED)
						active_liveness += verdict[face.active_liveness_status.verdict];
					else {
						active_liveness += pbio::ActiveLiveness::CheckTypeToString(face.active_liveness_status.check_type);
						active_liveness += ": ";
						active_liveness += verdict[face.active_liveness_status.verdict];
						active_liveness += " " + std::to_string(face.active_liveness_status.progress_level);
					}
					text_position.y += puttext(result, active_liveness, text_position);
				}

				if(face.age_gender_set)
				{
					text_position.y += puttext(
						result,
						face.age_gender.gender == pbio::AgeGenderEstimator::GENDER_FEMALE ? "female" :
						face.age_gender.gender == pbio::AgeGenderEstimator::GENDER_MALE   ? "male" :
						"bad gender",
						text_position);

					std::ostringstream age_text;

					age_text << "age: ";

					switch(face.age_gender.age)
					{
						case pbio::AgeGenderEstimator::AGE_KID    : age_text << "kid    "; break;
						case pbio::AgeGenderEstimator::AGE_YOUNG  : age_text << "young  "; break;
						case pbio::AgeGenderEstimator::AGE_ADULT  : age_text << "adult  "; break;
						case pbio::AgeGenderEstimator::AGE_SENIOR : age_text << "senior "; break;
						default                                   : age_text << "BadAge "; break;
					}

					age_text << "years: " << std::setprecision(3) << face.age_gender.age_years;

					text_position.y += puttext(
						result,
						age_text.str(),
						text_position);
				}

				if(face.emotions_set)
				{
					for(size_t i = 0; i < face.emotions.size(); ++i)
						text_position.y += puttext(
							result,
							face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_NEUTRAL  ? "neutral" :
							face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_HAPPY    ? "happy" :
							face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_ANGRY    ? "angry" :
							face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_SURPRISE ? "surprise" :
                            face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_DISGUSTED ? "disgusted" :
                            face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_SAD ? "sad" :
                            face.emotions[i].emotion == pbio::EmotionsEstimator::EMOTION_SCARED ? "scared" :
							"bad emotion",
							text_position,
							face.emotions[i].confidence * 0.75 + 0.25);

				}
			}
		}

		// no - draw the stripe
		if(order_i < max_count_in_stripe)
		{
			// place for thumbnail from the frame
			const cv::Rect sample_rect(
				data.frame.cols + draw_border,
				(thumbnail_size + draw_border) * order_i,
				thumbnail_size,
				thumbnail_size);

			// place for thumbnail of sti match
			const cv::Rect sti_sample_rect(
				sample_rect.x + thumbnail_size - thumbnail_size / 3,
				sample_rect.y + thumbnail_size - thumbnail_size / 3,
				thumbnail_size / 3,
				thumbnail_size / 3);

			// place for thumbnail from the database
			const cv::Rect match_rect(
				data.frame.cols + draw_border * 2 + thumbnail_size,
				(thumbnail_size + draw_border) * order_i,
				thumbnail_size,
				thumbnail_size);

			// make thumbnail from the frame
			MAssert(face.sample,);
			Database::makeThumbnail(*face.sample).copyTo(result(sample_rect));

			// draw match from short time identification if available
			const std::map<int, StiPersonData>::const_iterator sti_data_it =
					data.sti_persons_data.find(face.sti_person_id);

			if(face.sti_person_id_set && sti_data_it != data.sti_persons_data.end())
			{

				const StiPersonData &sti_data = sti_data_it->second;

				MAssert(sti_data.first_sample,);
				MAssert(sti_data.first_sample->getID() == face.sti_person_id,);

				if(face.sample->getID() == face.sti_person_id)
				{
					// this must be the same sample, no reason to draw it again
					MAssert(face.sample == sti_data.first_sample,);
				}
				else
				{
					Database::makeThumbnail(*sti_data.first_sample, std::string(), thumbnail_size / 3).copyTo(result(sti_sample_rect));
				}
			}



			// fade if image is lost
			if(face.draw_multilier < 1)
				result(sample_rect) *= face.draw_multilier;

			if(face.match_database_index < 0)
			{
				// gray color for unrecognized persons
				result(match_rect).setTo(128 * face.draw_multilier);
			}
			else
			{
				// thumbnail from the database for recognized persons
				database.thumbnails[face.match_database_index].copyTo(result(match_rect));

				// fade if image is lost
				if(face.draw_multilier < 1)
					result(match_rect) *= face.draw_multilier;
			}
		}
	}

	return result;
}

void Worker::PrintThreadFunc()
{
	int total_face = 0;
	for(;;)
	{
		if(_shutdown)
			break;
		
		if(_face_flag.updated)
		{
			auto end = std::chrono::steady_clock::now();
			total_face += _face_flag.frame_id;
			double timeDiff = std::chrono::duration_cast<std::chrono::microseconds>(end - Worker::ts_begin).count();
			Worker::ts_begin = end;

			std::stringstream sout;
			sout << " fps " << Worker::_stream_id << ":" << std::to_string(1000000 / std::max<double>(timeDiff, 1.));
			sout << " face_total " << Worker::_stream_id << ":" << total_face;
			sout << " face " << Worker::_stream_id << ":" << _face_flag.frame_id;
			sout << " match_all " << Worker::_stream_id << ":" << Worker::_match_counter;
			sout << " templates_all " << Worker::_stream_id << ":" << Worker::_template_counter;
			sout << " tracks_all " << Worker::_stream_id << ":" << Worker::_track_counter << "\n";

			std::cout << sout.str() << std::flush;
			_face_flag.updated = false;
		}
	}
}

void Worker::DrawingThreadFunc()
{
	for(;;)
	{
		// check for stop
		if(_shutdown)
			break;

		// we dont need to redraw too often
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		// get the data
		DrawingData data;

		{
			const std::lock_guard<std::mutex> guard(_drawing_data_mutex);


			// remove lost faces after 5 seconds
			{
				const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

				const float timeout = 5000;  // ms - i.e. 5 seconds

				for(std::map<int, FaceData>::iterator it = _drawing_data.faces.begin();
					it != _drawing_data.faces.end();)
				{
					const std::map<int, FaceData>::iterator i = it;
					++it; // we can erase the i, so increment it beforehand

					FaceData &face = i->second;

					// ignore alive faces
					if(!face.lost)
						continue;

					_drawing_data.updated = true;

					const int delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - face.lost_time).count();

					if(delay < timeout)
					{
						// lost faces will fade while this timeout
						face.draw_multilier = 1 - delay / timeout;
					}
					else
					{
						// erase it
						_drawing_data.faces.erase(i);
					}
				}
			}

			data = _drawing_data;
			_drawing_data.updated = false;
		}

		// if data is the same - no reason to redraw
		if(!data.updated)
		{
			continue;
		}

		// draw
		const cv::Mat drawed = Draw(
			data,
			_database);

		// and store it (it will be used in imshow in the main)
		{
			const std::lock_guard<std::mutex> guard(_draw_image_mutex);
			_draw_image = drawed;
		}
	}
}


template<typename T>
void clear_map(
	const int min_key_value,
	std::map<int, T> &map)
{
	std::vector<std::pair<int, T> > map_was(map.begin(), map.end());
	std::vector<std::pair<int, T> > map_new;

	for(size_t i = 0; i < map_was.size(); ++i)
	{
		if(map_was[i].first >= min_key_value )
			map_new.push_back(map_was[i]);
	}

	std::map<int, T> new_map(map_new.begin(), map_new.end());

	std::swap(map, new_map);
}

void Worker::clearAfterStreamReset(
	const int track_id_threshold)
{
	const std::lock_guard<std::mutex> guard(_drawing_data_mutex);

	clear_map(track_id_threshold, _drawing_data.faces);

	clear_map(track_id_threshold, _drawing_data.sti_persons_data);

	_drawing_data.updated = true;
}
