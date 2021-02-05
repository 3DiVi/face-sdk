/**
	\file test_videocap.cpp
	\brief Example of face tracking and liveness estimation.
*/


#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

#include <facerec/import.h>
#include <facerec/libfacerec.h>
#include <pbio/example/CVRawImage.h>


#ifndef CV_AA
#define CV_AA cv::LINE_AA
#endif

#include "../console_arguments_parser/ConsoleArgumentsParser.h"

int main(int argc, char** argv)
{
	try
	{
		std::cout << "Face tracking demo." << std::endl;

		#if defined(_WIN32)
			const std::string default_dll_path = "facerec.dll";
		#else
			const std::string default_dll_path = "../lib/libfacerec.so";
		#endif

		// parse named params
		ConsoleArgumentsParser parser(argc, argv);

		const std::string dll_path                = parser.get<std::string>("--dll_path                      ", default_dll_path);
		const std::string conf_dir_path           = parser.get<std::string>("--config_dir                    ", "../conf/facerec");
		const std::string license_dir             = parser.get<std::string>("--license_dir                   ", "../license");
		const std::string video_capturer_config   = parser.get<std::string>("--video_capturer_config         ", "fda_tracker_capturer_blf.xml");

		// create facerec service
		const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(dll_path, conf_dir_path, license_dir);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		// create capturer
		const pbio::Capturer::Ptr capturer = service->createCapturer(video_capturer_config);

		// create liveness2D estimator
		std::string liveness2d_config = "liveness_2d_estimator_v2.xml";
		const pbio::Liveness2DEstimator::Ptr liveness_2d_estimator = service->createLiveness2DEstimator(liveness2d_config);

		// open webcam via OpenCV
		cv::VideoCapture camera(0);

		if(!camera.isOpened())
		{
			throw std::runtime_error("can't open webcam");
		}

		for(;;)
		{
			// get frame
			pbio::CVRawImage image;
			camera >> image.mat();

			if(image.mat().empty())
				break;

			// track faces
			const std::vector<pbio::RawSample::Ptr> samples = capturer->capture(image);

			// estimate liveness
			std::vector<std::string> liveness_res(samples.size(), "*");
			for(size_t i = 0; i < samples.size(); ++i)
			{
				const pbio::Liveness2DEstimator::LivenessAndScore liveness_2d_result = liveness_2d_estimator->estimate(*samples[i]);

				std::stringstream ss;
				ss << std::fixed << std::setprecision(3) << liveness_2d_result.score;
				std::string score_str = ss.str();
				switch(liveness_2d_result.liveness)
				{
					case pbio::Liveness2DEstimator::NOT_ENOUGH_DATA:
						liveness_res[i] = "?";
						break;
					case pbio::Liveness2DEstimator::FAKE:
						liveness_res[i] = score_str + " - fake";
						break;
					case pbio::Liveness2DEstimator::REAL:
						liveness_res[i] = score_str + " - real";
						break;
				}
			}

			// draw faces and liveness
			cv::Mat draw = image.mat().clone();
			for(size_t i = 0; i < samples.size(); ++i)
			{
				// get sample info
				const pbio::RawSample::Rectangle rect = samples[i]->getRectangle();
				const std::vector<pbio::RawSample::Point> all_points = samples[i]->getLandmarks();

				// draw red points
				for(size_t j = 0; j < all_points.size(); ++j)
				{
					cv::circle(
						draw,
						cv::Point2f(all_points[j].x, all_points[j].y),
						2, cv::Scalar(0, 0, 255), -1);
				}

				// draw green rectangle
				cv::rectangle(
					draw,
					cv::Rect(rect.x, rect.y, rect.width, rect.height),
					cv::Scalar(0, 255, 0), 2);

				// print liveness
				cv::putText(
					draw,
					liveness_res[i],
					cv::Point2i(rect.x, rect.y),
					cv::FONT_HERSHEY_DUPLEX,
					0.8,
					cv::Scalar(0, 0, 255),
					2,
					CV_AA);
			}


			// show drawed image
			cv::imshow("draw", draw);

			// exit on esc
			if(27 == (uchar)cv::waitKey(15))
				break;
		}
	}
	catch(const pbio::Error &e)
	{
		std::cerr << "facerec exception catched: '" << e.what() << "' code: " << std::hex << e.code() << std::endl;
	}
	catch(const std::exception &e)
	{
		std::cerr << "exception catched: '" << e.what() << "'" << std::endl;
	}

	return 0;
}
