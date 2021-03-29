/**
	\file test_filecap.cpp
	\brief Example of face capturing, saving and loading samples, and face cutting.
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include <facerec/import.h>
#include <facerec/libfacerec.h>
#include <pbio/example/CVRawImage.h>

#ifndef CV_LOAD_IMAGE_COLOR
#define CV_LOAD_IMAGE_COLOR cv::IMREAD_COLOR
#endif

#ifndef CV_AA
#define CV_AA cv::LINE_AA
#endif

int main(int argc, char** argv)
{
	try
	{
		std::cout << "Face capturer demo." << std::endl;
		if(argc != 5)
		{
			std::cout << "Usage: '" << argv[0] << " <dll_path> <conf_dir_path> <capturer_config_filename> <image_path>'" << std::endl;
			std::cout << "Example: '" << argv[0] << " ../lib/libfacerec.so ../conf/facerec common_capturer4.xml ~/Pictures/1.png'" << std::endl;
			return 0;
		}

		const std::string dll_path = argv[1];
		const std::string conf_dir_path = argv[2];
		const std::string capturer_config_filename = argv[3];
		const std::string image_path = argv[4];

		std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
		std::cout.precision(1);

		// create facerec service
		const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(dll_path, conf_dir_path);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		// create capturer
		const pbio::Capturer::Ptr capturer = service->createCapturer(capturer_config_filename);

		// open image file via OpenCV
		pbio::CVRawImage image;
		image.mat() = cv::imread(image_path, CV_LOAD_IMAGE_COLOR);

		if(image.mat().empty())
		{
			std::cout << "can't open image file '" << image_path << "'" << std::endl;
			return 0;
		}

		// capture faces from image via pbio::CVRawImage
		std::vector<pbio::RawSample::Ptr> samples = capturer->capture(image);

		std::cout << "captured " << samples.size() << " faces" << std::endl;

		for(size_t i = 0; i < samples.size(); ++i)
		{
			// get sample info
			const pbio::RawSample::Rectangle rect = samples[i]->getRectangle();
			const pbio::RawSample::Point left_eye = samples[i]->getLeftEye();
			const pbio::RawSample::Point right_eye = samples[i]->getRightEye();
			const pbio::RawSample::Angles angles = samples[i]->getAngles();
			const std::vector<pbio::RawSample::Point> all_points = samples[i]->getLandmarks();
			const std::vector<pbio::RawSample::Point> all_iris_points = samples[i]->getIrisLandmarks();

			// print sample info
			std::cout << "\nsample " << i << ":\n" <<
				" rect: " << rect.width << "x" << rect.height << " from point (" << rect.x << "; " << rect.y << ")\n" <<
				" left_eye: (" << left_eye.x << "; " << left_eye.y << ")\n" <<
				" right_eye: (" << right_eye.x << "; " << right_eye.y << ")\n" <<
				" angles:\n  yaw: " << angles.yaw << "\n  pitch: " << angles.pitch << "\n  roll: " << angles.roll << "\n" <<
				" all landmarks count: " << all_points.size() << "\n" <<
				" face score " << samples[i]->getScore() << "\n" <<
				" all iris landmarks count: " << all_iris_points.size() << std::endl;

			// draw sample info
			cv::Mat sample_image = image.mat().clone();

			// draw green points
			for(size_t j = 0; j < all_points.size(); ++j)
			{
				cv::circle(
					sample_image,
					cv::Point2f(all_points[j].x, all_points[j].y),
					2, cv::Scalar(0, 255, 0), -1);
			}

			// draw iris points
			for(size_t j = 0; j < all_iris_points.size(); ++j)
			{
				float ms = 1;
				auto color = cv::Scalar(0, 255, 255);
				int oi = j - 20 * (j >= 20);
				pbio::RawSample::Point pt1 = all_iris_points[j];
				pbio::RawSample::Point pt2 = all_iris_points[(oi < 19 ? j : j - 15) + 1];

				if(oi < 5)
				{
					color = cv::Scalar(0, 165, 255);
					if (oi == 0)
					{
						double radius = cv::norm(cv::Point2f(pt1.x, pt1.y) - cv::Point2f(pt2.x, pt2.y));
						cv::circle(sample_image, cv::Point2f(pt1.x, pt1.y), radius, color, ms);
					}
				}else
					cv::line(
						sample_image,
						cv::Point2f(pt1.x, pt1.y),
						cv::Point2f(pt2.x, pt2.y),
						color,
						ms);

				cv::circle(sample_image, cv::Point2f(pt1.x, pt1.y), ms, color, -1);
			}

			if(rect.width > 400)
			{
				// draw points indexes
				for(size_t j = 0; j < all_points.size(); ++j)
				{
					std::ostringstream ss;
					ss << j;
					cv::putText(
						sample_image,
						ss.str(),
						cv::Point(all_points[j].x, all_points[j].y),
						cv::FONT_HERSHEY_DUPLEX,
						0.4,
						cv::Scalar(0, 0, 255),
						1,
						CV_AA);
				}
			}
			else
			{
				// draw green rectangle
				cv::rectangle(
					sample_image,
					cv::Rect(rect.x, rect.y, rect.width, rect.height),
					cv::Scalar(0, 255, 0), 2);
			}

			// draw cut rectangle
			const std::vector<pbio::Point> cut_rectangle_points =
				samples[i]->getFaceCutRectangle(pbio::RawSample::FACE_CUT_FULL_FRONTAL);

			for(int k = 0; k < (int) cut_rectangle_points.size(); ++k)
			{
				const int j = (k + 1) % cut_rectangle_points.size();

				cv::line(
					sample_image,
					cv::Point2f(cut_rectangle_points[k].x, cut_rectangle_points[k].y),
					cv::Point2f(cut_rectangle_points[j].x, cut_rectangle_points[j].y),
					cv::Scalar(0, 255, 100),
					2,
					CV_AA);
			}


			// draw blue left eye
			cv::circle(
				sample_image,
				cv::Point2f(left_eye.x, left_eye.y),
				2, cv::Scalar(255, 0, 0), -1);

			// draw red right eye
			cv::circle(
				sample_image,
				cv::Point2f(right_eye.x, right_eye.y),
				2, cv::Scalar(0, 0, 255), -1);

			cv::imshow("sample", sample_image);
			cv::waitKey();


			// save drawed image
			std::ostringstream image_save_path;
			image_save_path << image_path << ".face_" << i << ".png";

			const bool write_success = cv::imwrite(image_save_path.str(), sample_image);

			if(write_success)
			{
				std::cout << "sample draw saved in '" << image_save_path.str() << "'" << std::endl;
			}
			else
			{
				std::cout << "failed to save sample draw in '" << image_save_path.str() << "'" << std::endl;
			}


			// save raw sample
			std::ostringstream sample_save_path_stream;
			sample_save_path_stream << image_path << ".face_" << i << ".sample.bin";

			const std::string sample_save_path = sample_save_path_stream.str();

			std::ofstream out_file(sample_save_path.c_str(), std::ios_base::binary);

			samples[i]->save(out_file, pbio::RawSample::IMAGE_FORMAT_TIF);

			if(out_file.is_open() && out_file.good())
			{
				std::cout << "sample saved in '" << sample_save_path << "'" << std::endl;
			}
			else
			{
				std::cout << "failed to save sample in '" << sample_save_path << "'" << std::endl;
			}
			out_file.close();


			// load raw sample
			std::ifstream in_file(sample_save_path.c_str(), std::ios_base::binary);

			if(!in_file.is_open() || !in_file.good())
			{
				std::cout << "in_file is not opened, so loading raw sample will throw an exception." << std::endl;
			}

			const pbio::RawSample::Ptr loaded_sample = service->loadRawSample(in_file);

			in_file.close();

			std::cout << "loaded sample from '" << sample_save_path << "'" << std::endl;


			// compare loaded sample with the original
			// (just for fun)

			const std::vector<pbio::RawSample::Point> all_points_loaded = loaded_sample->getLandmarks();

			if(all_points_loaded.size() != all_points.size())
			{
				// this should never happen
				std::cout << "wrong loaded sample!" << std::endl;
			}
			else
			{
				float error = 0;
				for(size_t i = 0; i < all_points.size(); ++i)
				{
					error += std::abs<float>(all_points[i].x - all_points_loaded[i].x);
					error += std::abs<float>(all_points[i].y - all_points_loaded[i].y);
				}
				std::cout << "compare error: " << error << " (should be zero)" << std::endl;
			}


			const std::vector<pbio::RawSample::Point> all_iris_points_loaded = loaded_sample->getIrisLandmarks();

			if(all_iris_points_loaded.size() != all_iris_points.size())
			{
				// this should never happen
				std::cout << "wrong loaded sample!" << std::endl;
			}
			else
			{
				float error = 0;
				for(size_t i = 0; i < all_iris_points.size(); ++i)
				{
					error += std::abs<float>(all_iris_points[i].x - all_iris_points_loaded[i].x);
					error += std::abs<float>(all_iris_points[i].y - all_iris_points_loaded[i].y);
				}
				std::cout << "compare error: " << error << " (should be zero)" << std::endl;
			}


			// save cut image
			std::ostringstream cut_save_path_stream;
			cut_save_path_stream << image_path << ".face_" << i << ".cut.jpg";

			std::ofstream cut_file(cut_save_path_stream.str().c_str(), std::ios_base::binary);

			samples[i]->cutFaceImage(
				cut_file,
				pbio::RawSample::IMAGE_FORMAT_JPG,
				pbio::RawSample::FACE_CUT_FULL_FRONTAL);

			cut_file.flush();

			std::cout << "saving cut face in '" << cut_save_path_stream.str() << "' " <<
				( (cut_file.is_open() && cut_file.good()) ? "success" : "failed") << std::endl;

			cut_file.close();


			// save cut image in original scale
			std::ostringstream original_cut_save_path_stream;
			original_cut_save_path_stream << image_path << ".face_" << i << ".orignal_scale_cut.jpg";

			std::ofstream original_cut_file(original_cut_save_path_stream.str().c_str(), std::ios_base::binary);

			samples[i]->cutFaceImage(
				image,
				original_cut_file,
				pbio::RawSample::IMAGE_FORMAT_JPG,
				pbio::RawSample::FACE_CUT_FULL_FRONTAL);

			original_cut_file.flush();

			std::cout << "saving origianl scale cut face in '" << original_cut_save_path_stream.str() << "' " <<
				( (original_cut_file.is_open() && original_cut_file.good()) ? "success" : "failed") << std::endl;

			original_cut_file.close();
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
