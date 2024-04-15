/**
	\file video_recognition_demo/src/main.cpp
*/

#include <iostream>
#include <stdexcept>
#include <vector>
#include <queue>

#include <opencv2/opencv.hpp>

#include <facerec/import.h>
#include <facerec/libfacerec.h>

#include "../../console_arguments_parser/ConsoleArgumentsParser.h"


#include "MAssert.h"
#include "Database.h"
#include "Worker.h"


#if defined(WITH_OPENNI2)
#include "OpenniSource.h"
#elif defined(WITH_REALSENSE)
#include "RealSenseSource.h"
#else
#include "OpencvSource.h"
#endif

#ifndef CV_WND_PROP_FULLSCREEN
#define CV_WND_PROP_FULLSCREEN cv::WND_PROP_FULLSCREEN
#endif
#ifndef CV_WINDOW_FULLSCREEN
#define CV_WINDOW_FULLSCREEN cv::WINDOW_FULLSCREEN
#endif


int main(int argc, char const *argv[])
{
	try
	{
		// print usage
		std::cout << "usage: " << argv[0] << " "
			"<rtsp or rtmp sream url 1 | webcamera id 1> "
			"[ ... <rtsp or rtmp sream url N | webcamera id N>] "
			" -option1 value1 ..." << std::endl;

		#if defined(_WIN32)
				const std::string default_dll_path = "facerec.dll";
		#else
				const std::string default_dll_path = "../lib/libfacerec.so";
		#endif

		// parse named params
		ConsoleArgumentsParser parser(argc, argv);

		const std::string dll_path                       = parser.get<std::string>("--dll_path                      ", default_dll_path);
		const std::string config_dir                     = parser.get<std::string>("--config_dir                    ", "../conf/facerec");
		const std::string license_dir                    = parser.get<std::string>("--license_dir                   ", "../license");
		const std::string database_list_filepath         = parser.get<std::string>("--database_list_filepath        ");
		const std::string method_config                  = parser.get<std::string>("--method_config                 ");
		const std::string fullscreen                     = parser.get<std::string>("--fullscreen                    ", "yes");
		const std::string enable_active_liveness         = parser.get<std::string>("--enable_active_liveness        ", "no");
		const std::string vw_config_file                 = parser.get<std::string>("--vw_config_file                ", "video_worker_fdatracker_blf_fda.xml");
		const float       frame_fps_limit                = parser.get<float      >("--frame_fps_limit               ", 25);
		const float       recognition_far_threshold      = parser.get<float      >("--recognition_far_threshold");
		const bool		  output_metrics				 = parser.get<std::string>("--output_metrics                ", "no") == "yes" ? true : false;
		const bool 		  show_windows                   = parser.get<std::string>("--show_windows                  ", "yes") == "yes" ? true : false;
		const bool		  use_attributes				 = parser.get<std::string>("--use_attributes                ", "yes") == "yes" ? true : false;

		// cehck params
		MAssert(recognition_far_threshold > 0,);

		std::vector<cv::Ptr<ImageAndDepthSource> > sources;
		std::vector<std::string> sources_names;

#ifdef DEPTH_LIVENESS
		#ifdef WITH_OPENNI2
			sources_names.push_back("OpenNI2 source");
			sources.push_back(new OpenniSource());
		#else // WITH_REALSENSE
			sources_names.push_back("RealSense  source");
			sources.push_back(new RealSenseSource());
		#endif

		MAssert(sources_names.size() == 1,);
#else
		sources_names = parser.get();
		for(size_t i = 0; i < sources_names.size(); ++i)
		{
			const std::string src = sources_names[i];

			sources.push_back(new OpencvSource(src, output_metrics));

			std::ostringstream ss;
			ss << i << " : " << src;
			sources_names[i] = ss.str();
		}

		MAssert(sources_names.size() > 0,);
#endif

		MAssert(sources_names.size() == sources.size(),);

		// print sources
		std::cout << "\n " << sources.size() << " sources: " << std::endl;
		for(size_t i = 0; i < sources_names.size(); ++i)
			std::cout << "  " << sources_names[i] << std::endl;
		std::cout << std::endl;

		// create facerec servcie
		const pbio::FacerecService::Ptr service =
			pbio::FacerecService::createService(
				dll_path,
				config_dir,
				license_dir);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		const pbio::Recognizer::Ptr recognizer = service->createRecognizer(method_config, true, false);
		const float recognition_distance_threshold = recognizer->getROCCurvePointByFAR(recognition_far_threshold).distance;

		// create database
		const Database database(
			database_list_filepath,
			*recognizer,
			*service->createCapturer("common_capturer_blf_fda_front.xml"),
			recognition_distance_threshold);
		int active_liveness = enable_active_liveness == "yes" ? 1 : 0;
		//std::vector<pbio::ActiveLiveness::CheckType> checks{
		//		pbio::ActiveLiveness::CheckType::TURN_UP,
		//		pbio::ActiveLiveness::CheckType::SMILE,
		//		pbio::ActiveLiveness::CheckType::TURN_DOWN
		//};

		// create one VideoWorker
		const pbio::VideoWorker::Ptr video_worker =
			service->createVideoWorker(
				pbio::VideoWorker::Params()
					.video_worker_config(
						pbio::Config(vw_config_file)
							// .overrideParameter("single_match_mode", 1)
							.overrideParameter("search_k", 10)
							.overrideParameter("not_found_match_found_callback", 1)
							.overrideParameter("downscale_rawsamples_to_preferred_size", 0)
							.overrideParameter("enable_active_liveness", active_liveness)
#ifdef DEPTH_LIVENESS
							.overrideParameter("depth_data_flag", 1)
							.overrideParameter("good_light_dark_threshold", 1)
							.overrideParameter("good_light_range_threshold", 1)
							// .overrideParameter("good_blur_threshold", 0.2)
#endif
						)
					.recognizer_ini_file(method_config)
					.streams_count(sources.size())
					.processing_threads_count(sources.size())
					.matching_threads_count(sources.size())

					.short_time_identification_enabled(true)
					.short_time_identification_distance_threshold(recognition_distance_threshold)
					.short_time_identification_outdate_time_seconds(30)

					.age_gender_estimation_threads_count(use_attributes ? sources.size() : 0)
					.emotions_estimation_threads_count(use_attributes ? sources.size() : 0)
					//.active_liveness_checks_order(checks)
			);

		// set database
		video_worker->setDatabase(database.vw_elements);

		if(show_windows)
			for(size_t i = 0; i < sources_names.size(); ++i)
			{
				if(fullscreen == "yes")
				{
					cv::namedWindow(
						sources_names[i],
						cv::WINDOW_NORMAL);
					cv::setWindowProperty(
						sources_names[i],
						CV_WND_PROP_FULLSCREEN,
						CV_WINDOW_FULLSCREEN);
				}

				imshow(sources_names[i], cv::Mat(100, 100, CV_8UC3, cv::Scalar::all(0)));
			}

		// prepare buffers for store drawed results
		std::mutex draw_images_mutex;
		std::vector<cv::Mat> draw_images(sources.size());


		// create one worker per one source
		std::vector<cv::Ptr<Worker> > workers;

		for(size_t i = 0; i < sources.size(); ++i)
		{
			workers.push_back(cv::Ptr<Worker>(new Worker(
				database,
				video_worker,
				sources[i],
				i,  // stream_id
				draw_images_mutex,
				draw_images[i],
				frame_fps_limit,
				output_metrics)));
		}

		// draw results until escape presssed
		for(;;)
		{
			{
				const std::lock_guard<std::mutex> guard(draw_images_mutex);

				for(size_t i = 0; i < draw_images.size(); ++i)
					if(!draw_images[i].empty())
					{
						if(show_windows)
							cv::imshow(
								sources_names[i],
								draw_images[i]);
						draw_images[i] = cv::Mat();
					}

			}

			const uchar key = cv::waitKey(20);
			if(27 == key) {
				break;
			}

			if(' ' == key)
			{
				std::cout << "enable processing 0" << std::endl;
				video_worker->enableProcessingOnStream(0);
			}

			if('\n' == key)
			{
				std::cout << "disable processing 0" << std::endl;
				video_worker->disableProcessingOnStream(0);
			}


			if('h' == key)
			{
				std::cout << "enable AgeGenderEstimation 0" << std::endl;
				video_worker->enableAgeGenderEstimationOnStream(0);
			}

			if('n' == key)
			{
				std::cout << "disable AgeGenderEstimation 0" << std::endl;
				video_worker->disableAgeGenderEstimationOnStream(0);
			}

			if('j' == key)
			{
				std::cout << "enable EmotionsEstimation 0" << std::endl;
				video_worker->enableEmotionsEstimationOnStream(0);
			}

			if('m' == key)
			{
				std::cout << "disable EmotionsEstimation 0" << std::endl;
				video_worker->disableEmotionsEstimationOnStream(0);
			}



			if('t' == key)
			{
				video_worker->resetTrackerOnStream(0);
			}


			if('r' == key)
			{
				const int track_id_threshold = video_worker->resetStream(0);
				std::cout << "resetStream return track_id_threshold: " << track_id_threshold << std::endl;
				workers[0]->clearAfterStreamReset(track_id_threshold);
			}

			// check exceptions in callbacks
			video_worker->checkExceptions();
		}

	}
	catch(const std::exception &e)
	{
		std::cout << "video_recognition_demo exception catched: '" << e.what() << "'" << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cout << "video_recognition_demo ... exception catched" << std::endl;
		return 1;
	}

	return 0;
}
