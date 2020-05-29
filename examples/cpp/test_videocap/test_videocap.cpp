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

int main(int argc, char** argv)
{
	try
	{
		std::cout << "Face tracking demo." << std::endl;
		if(argc != 3)
		{
			std::cout << "Usage: '" << argv[0] << " <dll_path> <conf_dir_path>'" << std::endl;
			std::cout << "Example: '" << argv[0] << " ../lib/libfacerec.so ../conf/facerec'" << std::endl;
			return 0;
		}

		const std::string dll_path = argv[1];
		const std::string conf_dir_path = argv[2];

		// create facerec service
		const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(dll_path, conf_dir_path);

		// create capturer
		const pbio::Capturer::Ptr capturer = service->createCapturer("common_video_capturer.xml");

		// for each tracked face store liveness estimator
		std::map<int, pbio::LivenessEstimator::Ptr> track_id_2_le;

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
				const int track_id = samples[i]->getID();
				if(!track_id_2_le[track_id])
				{
					track_id_2_le[track_id] = service->createLivenessEstimator();
				}

				track_id_2_le[track_id]->addSample(*samples[i]);

				const pbio::LivenessEstimator::Liveness verdict = track_id_2_le[track_id]->estimateLiveness();

				switch(verdict)
				{
					case pbio::LivenessEstimator::NOT_ENOUGH_DATA:
						liveness_res[i] = "?";
						break;
					case pbio::LivenessEstimator::FAKE:
						liveness_res[i] = "fake";
						break;
					case pbio::LivenessEstimator::REAL:
						liveness_res[i] = "real";
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
					cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
					1.5,
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
