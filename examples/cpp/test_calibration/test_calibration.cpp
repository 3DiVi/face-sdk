/**
	\file test_calibration.cpp
	\brief Example of camera calibration.
*/


#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include <facerec/import.h>
#include <facerec/libfacerec.h>
#include <pbio/example/CVRawImage.h>


#ifndef CV_CAP_PROP_FRAME_WIDTH
#define CV_CAP_PROP_FRAME_WIDTH cv::CAP_PROP_FRAME_WIDTH
#endif

#ifndef CV_CAP_PROP_FRAME_HEIGHT
#define CV_CAP_PROP_FRAME_HEIGHT cv::CAP_PROP_FRAME_HEIGHT
#endif

#ifndef CV_AA
#define CV_AA cv::LINE_AA
#endif

void print_usage(const std::string name)
{
	std::cout <<
		"\n"
		"Tool for interactive camera calibration \n"
		"There are two modes: calibrate and view \n"
		"\n" "run in calibrate mode: \n  "
		"" << name << " <path to facerec lib> <path to config dir> <webcam id or stream url> <frame width> <frame height> "
		" calibrate <pattern type> <pattern width> <pattern height> <save camera params filepath> \n"
		"  where <pattern type> can be: \n"
		"    'chessboard' - PATTERN_CHESSBOARD \n"
		"    'circles'    - PATTERN_CIRCLES_GRID \n"
		"    'acircles'   - PATTERN_ASYMMETRIC_CIRCLES_GRID \n"
		"\n" "run in view mode: \n "
		"" << name << " <path to facerec lib> <path to config dir> <webcam id or stream url> <frame width> <frame height> "
		" view <load camera params filepath> \n"
		"\n" "in both modes use q and w keys to change alpha value \n "
		<< std::endl;
}

int int_from_string(const std::string s)
{
	std::istringstream ss(s);
	int result = 0;
	ss >> result;
	return result;
}

std::string float_to_string(const float f)
{
	std::ostringstream ss;
	ss << std::setprecision(4) << f;
	return ss.str();
}

cv::Ptr<cv::VideoCapture> openSource(
	const std::string source,
	const std::string frame_width_str,
	const std::string frame_height_str)
{
	const int frame_width = int_from_string(frame_width_str);
	const int frame_height = int_from_string(frame_height_str);

	bool source_only_numbers = true;

	for(size_t i = 0; i < source.length(); ++i)
		source_only_numbers =
			source_only_numbers &&
			source[i] >= '0' &&
			source[i] <= '9';

	cv::Ptr<cv::VideoCapture> result;

	if(source_only_numbers)
	{
		const int id = int_from_string(source);

		std::cout << "opening webcam " << id << std::endl;

		result = new cv::VideoCapture(id);
	}
	else
	{
		std::cout << "opening stream " << source << std::endl;

		result = new cv::VideoCapture(source);
	}

	if(frame_width > 0)
		result->set(CV_CAP_PROP_FRAME_WIDTH, frame_width);

	if(frame_height > 0)
		result->set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);

	return result;
}


void run_view_mode(
	const pbio::FacerecService &service,
	cv::VideoCapture &source,
	const std::string load_filepath)
{
	const pbio::CameraCalibrator::Ptr calibrator =
		service.createCameraCalibrator();

	{
		std::ifstream file(load_filepath.c_str(), std::ios_base::binary);

		if(!file.is_open() || !file.good())
		{
			std::cout << "can't open file '" << load_filepath << "' for reading" << std::endl;
			return;
		}

		calibrator->loadCameraParameters(file);
	}

	float alpha = 1;

	for(;;)
	{
		pbio::CVRawImage original;

		source >> original.mat();

		if(original.mat().empty())
		{
			std::cout << "warning: got empty frame, skip" << std::endl;
			continue;
		}

		const pbio::IRawImage::Ptr undistorted =
			calibrator->undistort(
				original,
				alpha);

		const cv::Mat cv_undistorted(
			original.mat().size(),
			original.mat().type(),
			(void*) undistorted->data());

		cv::imshow("original", original.mat());
		cv::imshow("cv_undistorted", cv_undistorted);

		const uchar key = cv::waitKey(30);

		if(key == 27)
			return;

		if(key == 'q')
			alpha -= 0.01;

		if(key == 'w')
			alpha += 0.01;

		if(alpha < 0)
			alpha = 0;

		if(alpha > 1)
			alpha = 1;

		std::cout << "alpha = " << alpha << " use keys 'q' and 'w' to change" << std::endl;
	}
}

void putText(
	cv::Mat image,
	const std::vector<std::string> &text)
{
	image.rowRange(0, std::min<int>(image.rows, 5 + 25 * text.size())) *= 0.5f;

	for(size_t i = 0; i < text.size(); ++i)
	{
		cv::putText(
			image,
			text[i],
			cv::Point2i(5, 20 + 25 * i),
			cv::FONT_HERSHEY_DUPLEX,
			0.7,
			cv::Scalar::all(255),
			1,
			CV_AA);
	}
}


void run_calibrate_mode(
	const pbio::FacerecService &service,
	cv::VideoCapture &source,
	const cv::Size frame_size,
	const std::string pattern_type,
	const cv::Size pattern_size,
	const std::string save_filepath)
{
	const pbio::CameraCalibrator::Ptr calibrator =
		service.createCameraCalibrator();

	pbio::CameraCalibrator::CalibrationSettings settings;

	settings.image_width  = frame_size.width;
	settings.image_height = frame_size.height;

	settings.pattern_type =
		pattern_type == "chessboard" ? pbio::CameraCalibrator::CalibrationSettings::PATTERN_CHESSBOARD :
		pattern_type == "circles"    ? pbio::CameraCalibrator::CalibrationSettings::PATTERN_CIRCLES_GRID :
		pattern_type == "acircles"   ? pbio::CameraCalibrator::CalibrationSettings::PATTERN_ASYMMETRIC_CIRCLES_GRID :
		(pbio::CameraCalibrator::CalibrationSettings::PatternType) (-1);

	if(settings.pattern_type == -1)
	{
		std::cout << "wrong pattern_type '" << pattern_type << "'" << std::endl;
		return;
	}

	settings.pattern_width  = pattern_size.width;
	settings.pattern_height = pattern_size.height;

	settings.fix_aspect_ratio          = false;
	settings.calib_zero_tangent_dist   = false;
	settings.calib_fix_principal_point = false;
	settings.rational_distortion_model = true;

	calibrator->initCalibration(settings);


	float alpha = 1;

	bool flip = false;
	bool hide_text = false;
	bool do_undistort = true;

	bool camera_params_init = false;

	int accepted_count = 0;

	bool was_calibration = false;
	bool calibration_success = false;
	float reprojection_error = 0;


	for(;;)
	{
		pbio::CVRawImage original;

		source >> original.mat();

		if(original.mat().empty())
		{
			std::cout << "warning: got empty frame, skip" << std::endl;
			continue;
		}

		std::vector<pbio::Point> pattern;
		bool pattern_found;
		bool pattern_accepted;


		calibrator->addImage(
			original,
			&pattern_found,
			&pattern_accepted,
			&pattern);


		if(pattern_accepted)
			++accepted_count;

		if(pattern_found)
		{
			std::vector<cv::Point2f> cv_pattern(pattern.size());
			for(size_t i = 0; i < pattern.size(); ++i)
			{
				cv_pattern[i].x = pattern[i].x;
				cv_pattern[i].y = pattern[i].y;
			}

			cv::drawChessboardCorners(
				original.mat(),
				pattern_size,
				cv_pattern,
				pattern_accepted);
		}


		const std::vector<pbio::Point> tip_pattern =
			calibrator->getTip();


		if(!tip_pattern.empty())
		{
			std::vector<cv::Point2f> cv_tip_pattern(tip_pattern.size());

			for(size_t i = 0; i < tip_pattern.size(); ++i)
			{
				cv_tip_pattern[i].x = tip_pattern[i].x;
				cv_tip_pattern[i].y = tip_pattern[i].y;
			}

			cv::drawChessboardCorners(
				original.mat(),
				pattern_size,
				cv_tip_pattern,
				true);
		}

		cv::Mat draw_image;
		if(camera_params_init && do_undistort)
		{
			const pbio::IRawImage::Ptr undistorted =
				calibrator->undistort(
					original,
					alpha);

			draw_image = cv::Mat(
				original.mat().size(),
				original.mat().type(),
				(void*) undistorted->data()).clone();
		}
		else
		{
			draw_image = original.mat();
		}

		if(flip)
			cv::flip(draw_image, draw_image, 1);

		if(!hide_text)
		{
			std::vector<std::string> text;

			text.push_back("press 'f' key to toggle flip, 'h' to hide the text");

			const float pattern_space_cover_progress =
				calibrator->getPatternSpaceCoverProgress();

			text.push_back(
				"pattern_space_cover_progress = " +
				float_to_string(pattern_space_cover_progress * 100) +
				"%");

			if(accepted_count > 2)
				text.push_back("press 'c' key to begin calibration");

			if(camera_params_init)
			{
				text.push_back("press 'u' key to toggle undistorton");

				if(do_undistort)
					text.push_back("alpha = " + float_to_string(alpha) +
						" press 'q' and 'w' to change alpha");
			}

			if(was_calibration && calibration_success)
				text.push_back("calibration success with reprojection error: " +
					float_to_string(reprojection_error));

			if(was_calibration && !calibration_success)
				text.push_back("calibration fail");

			if(camera_params_init)
				text.push_back("press 's' key to save camera parameters in '" + save_filepath + "'");

			putText(
				draw_image,
				text);
		}

		cv::imshow("frame", draw_image);

		const uchar key = cv::waitKey(10);

		if(key == 27)
			return;

		if(key == 'u')
			do_undistort = !do_undistort;

		if(key == 'f')
			flip = !flip;

		if(key == 'h')
			hide_text = !hide_text;

		if(key == 'q')
			alpha -= 0.01;

		if(key == 'w')
			alpha += 0.01;

		if(key == 's' && camera_params_init)
		{
			std::ofstream file(save_filepath.c_str(), std::ios_base::binary);

			if(!file.is_open() || !file.good())
				std::cout << "\n" "error: cant open '" << save_filepath << "' file for writing" "\n" << std::endl;
			else
				calibrator->saveCameraParameters(file);
		}

		if(key == 'c' && accepted_count > 2)
		{
			std::cout << "start calibration" << std::endl;

			calibration_success = calibrator->calibrate(
				50,  // max_used_patterns_count
				&reprojection_error);

			std::cout << "end calibration" << std::endl;

			was_calibration = true;

			if(calibration_success)
				camera_params_init = true;
		}

		if(alpha < 0)
			alpha = 0;

		if(alpha > 1)
			alpha = 1;
	}

}
int main(int argc, char const *argv[])
{
	try
	{

		print_usage(argv[0]);

		// there can be 10 arguments for the calibrate mode
		// or 7 arguments for view mode

		const bool calibrate_mode =
			argc == 11 &&
			argv[6] == std::string("calibrate");

		const bool view_mode =
			argc == 8 &&
			argv[6] == std::string("view");

		if( !calibrate_mode && !view_mode )
		{
			return 1;
		}


		const std::string facerec_lib_path    = argv[1];
		const std::string facerec_config_path = argv[2];
		const std::string source_str          = argv[3];
		const std::string frame_width_str     = argv[4];
		const std::string frame_height_str    = argv[5];

		const pbio::FacerecService::Ptr service =
			pbio::FacerecService::createService( facerec_lib_path, facerec_config_path );

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		cv::Ptr<cv::VideoCapture> source =
			openSource(
				source_str,
				frame_width_str,
				frame_height_str);

		if(!source->isOpened())
		{
			std::cout << "source not opened" << std::endl;
		}

		cv::Size frame_size;

		// sometimes first few frames are empty even if the acmera is ok
		{
			cv::Mat frame;
			for(int t = 0; t < 5; ++t)
			{
				(*source) >> frame;
				frame_size = frame.size();
			}
		}

		std::cout << "frame_size: " << frame_size << std::endl;

		if(frame_size.width <= 0 || frame_size.height <= 0)
		{
			std::cout << "error: empty frames" << std::endl;

			return 1;
		}


		if(calibrate_mode)
		{
			const std::string pattern_type = argv[7];

			const int pattern_width = int_from_string(argv[8]);
			const int pattern_height = int_from_string(argv[9]);

			const std::string save_filepath = argv[10];

			run_calibrate_mode(
				*service,
				*source,
				frame_size,
				pattern_type,
				cv::Size(
					pattern_width,
					pattern_height),
				save_filepath);
		}
		else if(view_mode)
		{
			const std::string load_filepath = argv[7];

			run_view_mode(
				*service,
				*source,
				load_filepath);
		}
	}
	catch( const pbio::Error &e )
	{
		std::cerr << "\n" "facerec exception catched: '" << e.what() <<
			"' code: " << std::hex << e.code() << std::endl;
	}
	catch( const std::exception &e )
	{
		std::cerr << "\n" "exception catched: '" << e.what() << "'" << std::endl;
	}
	catch( ... )
	{
		std::cerr << "\n" "... exception catched" << std::endl;
	}

	return 0;
}