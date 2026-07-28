/**
	\file demo.cpp
	Example of face tracking, and all kind of face estimation (ProcessingBlock API).
*/

#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>

#include <facerec/libfacerec.h>

#include "../console_arguments_parser/ConsoleArgumentsParser.h"


static pbio::ProcessingBlock makeBlock(
	const pbio::FacerecService::Ptr& service,
	const std::string& unit_type,
	const std::string& modification = "",
	const std::string& cut_type = "")
{
	pbio::Context config = service->createContext();
	config["unit_type"] = unit_type;
	if (!modification.empty())
		config["modification"] = modification;
	if (!cut_type.empty())
		config["cut_type"] = cut_type;
	return service->createProcessingBlock(config);
}


class Worker
{
public:
	Worker(
		const pbio::FacerecService::Ptr service);

	void work(const cv::Mat& frame);

private:

	const pbio::FacerecService::Ptr _service;

	pbio::ProcessingBlock _detector;
	pbio::ProcessingBlock _fitter;
	pbio::ProcessingBlock _quality_estimator;
	pbio::ProcessingBlock _age_estimator;
	pbio::ProcessingBlock _gender_estimator;
	pbio::ProcessingBlock _emotions_estimator;
	pbio::ProcessingBlock _liveness_estimator;
	pbio::ProcessingBlock _mask_estimator;
	pbio::ProcessingBlock _eye_openness_estimator;
	pbio::ProcessingBlock _face_cut_full;
	pbio::ProcessingBlock _face_cut_token;
	pbio::ProcessingBlock _face_cut_base;

	static constexpr int flags_count = 14;

	bool _flag_positions;
	bool _flag_angles;
	bool _flag_angles_vectors;
	bool _flag_quality;
	bool _flag_face_quality;
	bool _flag_liveness;
	bool _flag_age_gender;
	bool _flag_emotions;
	bool _flag_cutting_base;
	bool _flag_cutting_full;
	bool _flag_cutting_token;
	bool _flag_points;
	bool _flag_masked_face;
	bool _flag_eyes_openness;

	bool& flag(int i);
	static std::string flag_name(int i) ;

	static cv::Rect flag_rect(int i) ;

	static void onMouse(int event, int x, int y, int, void* this_ptr);
};


int main(int argc, char const *argv[])
{
	try
	{
		std::cout << "usage: " << argv[0] <<
			" [--dll_path ../lib/facerec]"
			" [--config_dir ../conf/facerec]"
			" [--license_dir ../license]"
			<< std::endl;

#if defined(_WIN32)
		const std::string default_dll_path = "facerec.dll";
#else
		const std::string default_dll_path = "../lib/libfacerec.so";
#endif

		ConsoleArgumentsParser parser(argc, argv);

		const std::string dll_path           = parser.get<std::string>("--dll_path      ", default_dll_path);
		const std::string config_dir         = parser.get<std::string>("--config_dir    ", "../conf/facerec");
		const std::string license_dir        = parser.get<std::string>("--license_dir   ", "../license");

		const pbio::FacerecService::Ptr service =
			pbio::FacerecService::createService(
				dll_path,
				config_dir,
				license_dir);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		Worker worker(service);

		const int desired_frame_widht = 1280;
		const int desired_frame_height = 720;

		int camera_id = 0;

		cv::VideoCapture camera;

		cv::Size frame_size;

		for(; camera_id < 100 ; ++camera_id)
		{
			camera.open(camera_id);
			camera.set(cv::CAP_PROP_FRAME_WIDTH, desired_frame_widht);
			camera.set(cv::CAP_PROP_FRAME_HEIGHT, desired_frame_height);

			cv::Mat frame;

			for(int i = 0; i < 10; ++i)
				camera >> frame;

			if(frame.empty())
			{
				std::cout << "webcam " << camera_id << " not opened" << std::endl;
				camera.release();
			}
			else
			{
				frame_size = frame.size();
				break;
			}
		}

	for(;;)
		{
			cv::Mat frame;

			if(!camera.read(frame)) {
				break;
			}

			worker.work(frame);

			if(27 == (uchar) cv::waitKey(10)) {
				break;
			}

	}
}
	catch(const std::exception &e)
	{
		std::cout << "! exception catched: '" << e.what() << "' ... exiting" << std::endl;
		return 1;
	}

	return 0;
}


Worker::Worker(
	const pbio::FacerecService::Ptr service) :
_service(service),
_detector(makeBlock(service, "FACE_TRACKING_MODULE", "fda")),
_fitter(makeBlock(service, "FACE_FITTER", "fda")),
_quality_estimator(makeBlock(service, "QUALITY_ASSESSMENT_ESTIMATOR", "assessment")),
_age_estimator(makeBlock(service, "AGE_ESTIMATOR")),
_gender_estimator(makeBlock(service, "GENDER_ESTIMATOR")),
_emotions_estimator(makeBlock(service, "EMOTION_ESTIMATOR")),
_liveness_estimator(makeBlock(service, "LIVENESS_ESTIMATOR", "2d")),
_mask_estimator(makeBlock(service, "MASK_ESTIMATOR")),
_eye_openness_estimator(makeBlock(service, "EYE_OPENNESS_ESTIMATOR")),
_face_cut_full(makeBlock(service, "UTILITY_MODULE", "face_cut", "FACE_CUT_FULL_FRONTAL")),
_face_cut_token(makeBlock(service, "UTILITY_MODULE", "face_cut", "FACE_CUT_TOKEN_FRONTAL")),
_face_cut_base(makeBlock(service, "UTILITY_MODULE", "face_cut", "FACE_CUT_BASE")),

_flag_positions( true ),
_flag_angles( true ),
_flag_angles_vectors( true ),
_flag_quality( false ),
_flag_face_quality( false ),
_flag_liveness( false ),
_flag_age_gender( false ),
_flag_emotions( false ),
_flag_cutting_base( false ),
_flag_cutting_full( false ),
_flag_cutting_token( false ),
_flag_points( true ),
_flag_masked_face( false ),
_flag_eyes_openness( false )

{
}


bool& Worker::flag(const int i)
{
	switch(i)
	{
		case 0: return _flag_positions;
		case 1: return _flag_angles;
		case 2: return _flag_quality;
		case 3: return _flag_liveness;
		case 4: return _flag_age_gender;
		case 5: return _flag_cutting_base;
		case 6: return _flag_cutting_full;
		case 7: return _flag_cutting_token;
		case 8: return _flag_points;
		case 9: return _flag_face_quality;
		case 10: return _flag_angles_vectors;
		case 11: return _flag_emotions;
		case 12: return _flag_masked_face;
		case 13: return _flag_eyes_openness;
		default: return _flag_points;
	}
}


std::string Worker::flag_name(const int i)
{
	switch(i)
	{
		case 0: return "rectangles";
		case 1: return "angles";
		case 2: return "quality";
		case 3: return "liveness";
		case 4: return "age and gender";
		case 5: return "base cut";
		case 6: return "full cut";
		case 7: return "token cut";
		case 8: return "points";
		case 9: return "face quality";
		case 10: return "angles vectors";
		case 11: return "emotions";
		case 12: return "masked face";
		case 13: return "eyes openness";
		default: return "";
	}
}


cv::Rect Worker::flag_rect(const int i)
{
	return {16, 16 + 24 * i, 16, 16};
}


inline
void puttext(
	cv::Mat image,
	const std::string& text,
	const cv::Point2f& position)
{
	cv::putText(
		image,
		text,
		position,
		cv::FONT_HERSHEY_DUPLEX,
		0.7,
		cv::Scalar::all(255),
		5,
		cv::LINE_AA);

	cv::putText(
		image,
		text,
		position,
		cv::FONT_HERSHEY_DUPLEX,
		0.7,
		cv::Scalar::all(0),
		1,
		cv::LINE_AA);
}


void Worker::work(const cv::Mat& frame)
{
	pbio::Context ioData = _service->createContextFromFrame(
		const_cast<uint8_t*>(frame.data),
		frame.cols, frame.rows,
		pbio::Context::FORMAT_BGR);

	_detector(ioData);

	if (ioData["objects"].size() == 0)
	{
		cv::Mat draw_image = frame.clone();

		for(int i = 0; i < flags_count; ++i)
		{
			const cv::Rect rect = flag_rect(i);
			const cv::Rect rect2 (rect.x + 5, rect.y + 5, rect.width - 10, rect.height - 10);

			cv::rectangle(draw_image, rect, cv::Scalar::all(255), -1);
			cv::rectangle(draw_image, rect, cv::Scalar::all(0), 2, cv::LINE_AA);

			if( flag(i) )
			{
				cv::rectangle(draw_image, rect2, cv::Scalar::all(0), -1, cv::LINE_AA);
			}

			puttext(
				draw_image,
				flag_name(i),
				cv::Point2f(rect.x + rect.width + 3, rect.y + rect.height - 3));
		}

		cv::imshow("demo", draw_image);
		cv::setMouseCallback("demo", onMouse, this);
		return;
	}

	_fitter(ioData);

	if (_flag_age_gender)
	{
		_age_estimator(ioData);
		_gender_estimator(ioData);
	}

	if (_flag_emotions)
		_emotions_estimator(ioData);

	if (_flag_quality || _flag_face_quality)
		_quality_estimator(ioData);

	if (_flag_liveness && ioData["objects"].size() == 1)
		_liveness_estimator(ioData);

	if (_flag_masked_face)
		_mask_estimator(ioData);

	if (_flag_eyes_openness)
		_eye_openness_estimator(ioData);

	pbio::Context cut_full_ctx = _service->createContext();
	if (_flag_cutting_full)
	{
		cut_full_ctx = ioData;
		_face_cut_full(cut_full_ctx);
	}

	pbio::Context cut_token_ctx = _service->createContext();
	if (_flag_cutting_token)
	{
		cut_token_ctx = ioData;
		_face_cut_token(cut_token_ctx);
	}

	pbio::Context cut_base_ctx = _service->createContext();
	if (_flag_cutting_base)
	{
		cut_base_ctx = ioData;
		_face_cut_base(cut_base_ctx);
	}

	cv::Mat draw_image = frame.clone();

	const long img_w = ioData["image"]["shape"][1].getLong();
	const long img_h = ioData["image"]["shape"][0].getLong();

	for(size_t i = 0; i < ioData["objects"].size(); ++i)
	{
		const auto obj = ioData["objects"][i];

		const auto bbox = obj["bbox"];
		const double x1 = bbox[0].getDouble();
		const double y1 = bbox[1].getDouble();
		const double x2 = bbox[2].getDouble();
		const double y2 = bbox[3].getDouble();

		const cv::Rect rect(
			static_cast<int>(x1 * img_w),
			static_cast<int>(y1 * img_h),
			static_cast<int>((x2 - x1) * img_w),
			static_cast<int>((y2 - y1) * img_h));

		cv::Point2f text_point(
			rect.x + rect.width + 3,
			rect.y + 10);

		constexpr float text_line_height = 22;

		if(_flag_points)
		{
			if (obj.contains("keypoints"))
			{
				const auto pts = obj["keypoints"]["points"];
				for (const auto& pt : pts)
				{
					const double kx = pt["proj"][0].getDouble() * img_w;
					const double ky = pt["proj"][1].getDouble() * img_h;
					cv::circle(
						draw_image,
						cv::Point2f(kx, ky),
						2,
						cv::Scalar(50, 50, 255),
						-1,
						cv::LINE_AA);
				}
			}
		}

		if(_flag_positions)
		{
			cv::rectangle(
				draw_image,
				rect,
				cv::Scalar(50, 50, 255),
				2,
				cv::LINE_AA);
		}

		if(_flag_age_gender)
		{
			if (obj.contains("age"))
			{
				const long age = obj["age"].getLong();
				std::string gender = obj.contains("gender")
					? obj["gender"].getString() : "?";

				std::ostringstream age_text;
				age_text << "age: " << age;
				puttext(draw_image, age_text.str(), text_point);
				text_point.y += text_line_height;

				puttext(
					draw_image,
					gender == "FEMALE" ? "gender: female" :
					gender == "MALE" ? "gender: male" : "gender: ?",
					text_point);
				text_point.y += text_line_height;

				text_point.y += text_line_height / 3;
			}
		}

		if(_flag_emotions)
		{
			if (obj.contains("emotions"))
			{
				for (const auto& em : obj["emotions"])
				{
					const std::string emotion = em["emotion"].getString();
					const double confidence = em["confidence"].getDouble();

					cv::rectangle(
						draw_image,
						cv::Rect(
							text_point.x,
							text_point.y - text_line_height / 2,
							100 * confidence,
							text_line_height),
						emotion == "NEUTRAL"  ? cv::Scalar(255, 0, 0) :
						emotion == "HAPPY"    ? cv::Scalar(0, 255, 0) :
						emotion == "ANGRY"    ? cv::Scalar(0, 0, 255) :
						emotion == "SURPRISED" ? cv::Scalar(0, 255, 255) :
						emotion == "DISGUSTED" ? cv::Scalar(255, 255, 0) :
						emotion == "SAD"      ? cv::Scalar(255, 0, 255) :
						emotion == "SCARED"   ? cv::Scalar(0, 255, 127) :
							cv::Scalar(0, 0, 0),
						-1);

					puttext(
						draw_image,
						emotion == "NEUTRAL"  ? "neutral" :
						emotion == "HAPPY"    ? "happy" :
						emotion == "ANGRY"    ? "angry" :
						emotion == "SURPRISED" ? "surprised" :
						emotion == "DISGUSTED" ? "disgusted" :
						emotion == "SAD"      ? "sad" :
						emotion == "SCARED"   ? "scared" : "?",
						text_point + cv::Point2f(100, 0));

					text_point.y += text_line_height;

					text_point.y += text_line_height / 3;
				}
			}
		}

		if(_flag_angles)
		{
			if (obj.contains("pose"))
			{
				const double yaw   = obj["pose"]["yaw"].getDouble();
				const double pitch = obj["pose"]["pitch"].getDouble();
				const double roll  = obj["pose"]["roll"].getDouble();

				std::ostringstream yaw_str, pitch_str, roll_str;
				yaw_str   << "yaw: "   << yaw;
				pitch_str << "pitch: " << pitch;
				roll_str  << "roll: "  << roll;

				puttext(draw_image, yaw_str.str(), text_point);
				text_point.y += text_line_height;

				puttext(draw_image, pitch_str.str(), text_point);
				text_point.y += text_line_height;

				puttext(draw_image, roll_str.str(), text_point);
				text_point.y += text_line_height;

				text_point.y += text_line_height / 3;
			}
		}

		if(_flag_angles_vectors)
		{
			if (obj.contains("pose"))
			{
				const double yaw   = obj["pose"]["yaw"].getDouble();
				const double pitch = obj["pose"]["pitch"].getDouble();
				const double roll  = obj["pose"]["roll"].getDouble();

				const float cos_yaw   = cos( yaw   * CV_PI / 180 );
				const float sin_yaw   = sin( yaw   * CV_PI / 180 );

				const float cos_pitch = cos( pitch * CV_PI / 180 );
				const float sin_pitch = sin( pitch * CV_PI / 180 );

				const float cos_roll  = cos( roll  * CV_PI / 180 );
				const float sin_roll  = sin( roll  * CV_PI / 180 );

				const cv::Point3f xyz[3] = {
					cv::Point3f(
						cos_yaw * cos_roll - sin_yaw * sin_pitch * sin_roll,
						sin_yaw * sin_pitch * cos_roll + cos_yaw * sin_roll,
						sin_yaw * cos_pitch),

					cv::Point3f(
						cos_pitch * sin_roll,
						-cos_pitch * cos_roll,
						sin_pitch),

					cv::Point3f(
						sin_yaw * cos_roll + cos_yaw * sin_pitch * sin_roll,
						-cos_yaw * sin_pitch * cos_roll + sin_yaw * sin_roll,
						-cos_yaw * cos_pitch),
				};

				cv::Point2f center(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f);

				const float length = static_cast<float>(rect.width + rect.height) * 0.3f;

				for(int c = 0; c < 3; ++c)
				{
					cv::line(
						draw_image,
						center,
						center + cv::Point2f(xyz[c].x, xyz[c].y) * length,
						c == 0 ? cv::Scalar(50, 255, 255) :
						c == 1 ? cv::Scalar(50, 255, 50) :
						c == 2 ? cv::Scalar(50, 50, 255) :
						cv::Scalar(),
						2,
						cv::LINE_AA);
				}
			}
		}

		if(_flag_quality)
		{
			if (obj.contains("quality"))
			{
				const auto q = obj["quality"];

				std::ostringstream lighting, noise, sharpness, flare;

				if (q.contains("illumination_score"))
				{
					lighting << "lighting: " << q["illumination_score"].getDouble();
					puttext(draw_image, lighting.str(), text_point);
					text_point.y += text_line_height;
				}

				if (q.contains("noise_score"))
				{
					noise << "noise: " << q["noise_score"].getDouble();
					puttext(draw_image, noise.str(), text_point);
					text_point.y += text_line_height;
				}

				if (q.contains("sharpness_score"))
				{
					sharpness << "sharpness: " << q["sharpness_score"].getDouble();
					puttext(draw_image, sharpness.str(), text_point);
					text_point.y += text_line_height;
				}

				if (q.contains("flare_score"))
				{
					flare << "flare: " << q["flare_score"].getDouble();
					puttext(draw_image, flare.str(), text_point);
					text_point.y += text_line_height;
				}

				text_point.y += text_line_height / 3;
			}
		}

		if(_flag_liveness)
		{
			if (obj.contains("liveness"))
			{
				const auto liveness = obj["liveness"];
				const std::string value = liveness["value"].getString();
				const double confidence = liveness["confidence"].getDouble();

				puttext(
					draw_image,
					"Liveness: " + value,
					text_point);
				text_point.y += text_line_height;
				text_point.y += (text_line_height / 3);

				std::stringstream ss;
				ss << "score: " << std::fixed << std::setprecision(3) << confidence;
				puttext(draw_image, ss.str(), text_point);
				text_point.y += text_line_height;
				text_point.y += (text_line_height / 3);
			}
		}

		if(_flag_masked_face)
		{
			if (obj.contains("has_medical_mask"))
			{
				const bool has_mask = obj["has_medical_mask"]["value"].getBool();
				const double mask_conf = obj["has_medical_mask"]["confidence"].getDouble();

				std::stringstream ss;
				ss << std::fixed << std::setprecision(3) << mask_conf;
				std::string score_str = ss.str();

				puttext(
					draw_image,
					std::string("masked: ") + (has_mask ? "true - " : "false - ") + score_str,
					text_point);

				text_point.y += text_line_height;
				text_point.y += text_line_height / 3;
			}
		}

		if(_flag_eyes_openness)
		{
			if (obj.contains("is_left_eye_open") || obj.contains("is_right_eye_open"))
			{
				if (obj.contains("is_left_eye_open"))
				{
					const bool left_open = obj["is_left_eye_open"]["value"].getBool();
					const double left_conf = obj["is_left_eye_open"]["confidence"].getDouble();

					std::stringstream ssl;
					ssl << std::fixed << std::setprecision(3) << left_conf;

					puttext(
						draw_image,
						std::string("left eye: ") + (left_open ? "true " : "false ") + ssl.str(),
						text_point);

					text_point.y += text_line_height;
					text_point.y += text_line_height / 3;
				}

				if (obj.contains("is_right_eye_open"))
				{
					const bool right_open = obj["is_right_eye_open"]["value"].getBool();
					const double right_conf = obj["is_right_eye_open"]["confidence"].getDouble();

					std::stringstream ssr;
					ssr << std::fixed << std::setprecision(3) << right_conf;

					puttext(
						draw_image,
						std::string("right eye: ") + (right_open ? "true " : "false ") + ssr.str(),
						text_point);

					text_point.y += text_line_height;
					text_point.y += text_line_height / 3;
				}
			}
		}

		if(_flag_face_quality)
		{
			if (obj.contains("quality") && obj["quality"].contains("total_score"))
			{
				const double quality = obj["quality"]["total_score"].getDouble();

				std::ostringstream ss;
				ss << "face quality: " << quality;
				puttext(draw_image, ss.str(), text_point);
				text_point.y += text_line_height;
				text_point.y += text_line_height / 3;
			}
		}

		for(int cut_i = 0; cut_i < 3; ++cut_i)
		{
			if((cut_i == 0 && !_flag_cutting_base) ||
				(cut_i == 1 && !_flag_cutting_full) ||
				(cut_i == 2 && !_flag_cutting_token))
			{
				continue;
			}

			pbio::Context* cut_ctx = (cut_i == 1) ? &cut_full_ctx : (cut_i == 2) ? &cut_token_ctx : &cut_base_ctx;

			const auto cut_obj = (*cut_ctx)["objects"][i];
			if (!cut_obj.contains("face_crop"))
				continue;

			puttext(
				draw_image,
				cut_i == 1 ? "full cut:" :
				cut_i == 2 ? "token cut:" :
				cut_i == 0 ? "base cut:" : "?? cut",
				text_point);
			text_point.y += text_line_height / 2;

			const auto face_crop = cut_obj["face_crop"];
			uint8_t* crop_data = face_crop["blob"].getDataPtr();
			const long crop_h = face_crop["shape"][0].getLong();
			const long crop_w = face_crop["shape"][1].getLong();
			const long crop_c = face_crop["shape"][2].getLong();

			const int crop_type = crop_c == 1 ? CV_8UC1 : CV_8UC3;
			cv::Mat img(crop_h, crop_w, crop_type, crop_data);
			cv::cvtColor(img, img, cv::COLOR_RGB2BGR);

			cv::resize(img, img, cv::Size(), 0.3, 0.3);

			cv::Rect img_rect;

			img_rect.x = std::max<int>(0, -text_point.x);
			img_rect.y = std::max<int>(0, -text_point.y);

			img_rect.width = std::min<int>(
				img.cols - img_rect.x,
				draw_image.cols - std::max<int>(0, text_point.x));

			img_rect.height = std::min<int>(
				img.rows - img_rect.y,
				draw_image.rows - std::max<int>(0, text_point.y));

			if(img_rect.width <= 0 || img_rect.height <= 0)
				continue;

			img(img_rect).copyTo(
				draw_image(cv::Rect(
					std::max<int>(0, text_point.x),
					std::max<int>(0, text_point.y),
					img_rect.width,
					img_rect.height)));

			text_point.y += text_line_height / 2;
			text_point.y += img.rows;

			text_point.y += text_line_height / 3;
		}

	}

	for(int i = 0; i < flags_count; ++i)
	{
		const cv::Rect rect = flag_rect(i);
		const cv::Rect rect2 (rect.x + 5, rect.y + 5, rect.width - 10, rect.height - 10);

		cv::rectangle(draw_image, rect, cv::Scalar::all(255), -1);
		cv::rectangle(draw_image, rect, cv::Scalar::all(0), 2, cv::LINE_AA);

		if( flag(i) )
		{
			cv::rectangle(draw_image, rect2, cv::Scalar::all(0), -1, cv::LINE_AA);
		}

		puttext(
			draw_image,
			flag_name(i),
			cv::Point2f(rect.x + rect.width + 3, rect.y + rect.height - 3));
	}

	cv::imshow("demo", draw_image);

	cv::setMouseCallback("demo", onMouse, this);
}


void Worker::onMouse(const int event, const int x, const int y, int, void* this_ptr_)
{
	auto* const this_ptr = static_cast<Worker*>( this_ptr_ );

	if(event != cv::EVENT_LBUTTONDOWN &&
		event != cv::EVENT_RBUTTONDOWN &&
		event != cv::EVENT_MBUTTONDOWN)
	{
		return;
	}

	for(int i = 0; i < Worker::flags_count; ++i)
	{
		const cv::Rect r = Worker::flag_rect(i);

		if(x >= r.x && y >= r.y && x <= r.x + r.width && y <= r.y + r.height)
			this_ptr->flag(i) = !this_ptr->flag(i);
	}
}