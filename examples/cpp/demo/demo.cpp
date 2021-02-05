/**
	\file demo.cpp
	Example of face tracking, and all kind of face estimation.
*/

#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>


#include <facerec/import.h>
#include <pbio/example/CVRawImage.h>
#include <facerec/libfacerec.h>

#include "../console_arguments_parser/ConsoleArgumentsParser.h"

// calss that will do all the work
class Worker
{
public:
	Worker(
		const pbio::FacerecService::Ptr service,
		const std::string capturer_conf);

	void work(pbio::InternalImageBuffer::Ptr frame);

private:

	// one service
	const pbio::FacerecService::Ptr _service;

	// one tracker (i.e. common_video_capturer for work with video)
	pbio::Capturer::Ptr _tracker;

	// one quality estimator
	const pbio::QualityEstimator::Ptr _quality_estimator;

	// one age and gender estimator
	const pbio::AgeGenderEstimator::Ptr _age_geder_estimator;

	// one emotions estimator
	const pbio::EmotionsEstimator::Ptr _emotions_estimator;

	// one face quality estimator
	const pbio::FaceQualityEstimator::Ptr _face_quality_estimator;

	// one face attributes estimator
	pbio::FaceAttributesEstimator::Ptr _face_attributes_estimator;


	// flags for enable / disable drawing and comuting of the features
	static const int flags_count = 13;

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

	// liveness estimator
	pbio::Liveness2DEstimator::Ptr liveness2d;
	const std::string liveness_config = "liveness_2d_estimator_v2.xml";

	bool& flag(int i);
	std::string flag_name(int i) const;

	cv::Rect flag_rect(int i) const;

	// mouse click callback for flags controll
	static void onMouse(int event, int x, int y, int, void* this_ptr);
};


int main(int argc, char const *argv[])
{
	try
	{
		// print usage
		std::cout << "usage: " << argv[0] <<
			" [--dll_path ../lib/facerec]"
			" [--config_dir ../conf/facerec]"
			" [--capturer_conf fda_tracker_capturer_blf.xml]"
			" [--license_dir ../license]"
			<< std::endl;

#if defined(_WIN32)
		const std::string default_dll_path = "facerec.dll";
#else
		const std::string default_dll_path = "../lib/libfacerec.so";
#endif
		// parse named params
		ConsoleArgumentsParser parser(argc, argv);

		const std::string dll_path           = parser.get<std::string>("--dll_path      ", default_dll_path);
		const std::string config_dir         = parser.get<std::string>("--config_dir    ", "../conf/facerec");
		const std::string capturer_conf      = parser.get<std::string>("--capturer_conf ", "fda_tracker_capturer_blf_front.xml");
		const std::string license_dir        = parser.get<std::string>("--license_dir   ", "");


		const pbio::FacerecService::Ptr service =
			pbio::FacerecService::createService(
				dll_path,
				config_dir,
				license_dir);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		// create worker
		// (we are expecting run from bin folder)
		Worker worker(
			service,
			capturer_conf);


		// here we trying camera open with opencv

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

			// sometimes first few frames can be empty even if camera is good
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
			const pbio::InternalImageBuffer::Ptr frame =
				service->createInternalImageBuffer(
					frame_size.width,
					frame_size.height,
					pbio::RawImage::Format::FORMAT_BGR);

			if(!camera.read(cv::Mat(frame->height, frame->width, CV_8UC3, frame->data)))
				break;

			// give a frame to the worker
			worker.work(frame);

			if(27 == (uchar) cv::waitKey(10))
				break;
		}

	}
	catch(const std::exception &e)
	{
		std::cout << "! exception catched: '" << e.what() << "' ... exiting" << std::endl;
		return 1;
	}

	return 0;
}


// worker contructor
// (creating few facerec objects)
Worker::Worker(
	const pbio::FacerecService::Ptr service,
	const std::string capturer_conf) :
_service(service),
_tracker(
	_service->createCapturer(
		pbio::FacerecService::Config(capturer_conf)
			.overrideParameter("downscale_rawsamples_to_preferred_size", 0))),
_quality_estimator( _service->createQualityEstimator("quality_estimator_iso.xml") ),
_age_geder_estimator( _service->createAgeGenderEstimator("age_gender_estimator.xml") ),
//_age_geder_estimator( _service->createAgeGenderEstimator("age_gender_estimator_v2.xml") ),
_emotions_estimator( _service->createEmotionsEstimator("emotions_estimator.xml") ),
_face_quality_estimator( _service->createFaceQualityEstimator("face_quality_estimator.xml")),

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
_flag_masked_face( false )

{
	// nothing else
}

// for get/set flag value
bool& Worker::flag(int i)
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
	}

	return _flag_points;
}

// for draw a text with the flag checkbox
std::string Worker::flag_name(int i) const
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
		case 12: return "masked_face";
	}

	return "";
}

// cehckbox rectanble, corresponsing to the flag
cv::Rect Worker::flag_rect(int i) const
{
	return cv::Rect(16, 16 + 24 * i, 16, 16);
}


// drawing text with opencv
inline
void puttext(
	cv::Mat image,
	const std::string text,
	const cv::Point2f position)
{
	// twice - for better reading
	// since we are drawing on the frame from webcam

	// white background
	cv::putText(
		image,
		text,
		position,
		cv::FONT_HERSHEY_DUPLEX,
		0.7,
		cv::Scalar::all(255),
		5,
		cv::LINE_AA);

	// black text
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

// work with new frame
void Worker::work(const pbio::InternalImageBuffer::Ptr frame)
{
	const std::vector<pbio::RawSample::Ptr> samples = _tracker->capture(frame);

	// clone the frome for drawing on it
	cv::Mat draw_image = cv::Mat(frame->height, frame->width, CV_8UC3, frame->data).clone();

	// handle each face on the frame separately
	for(size_t i = 0; i < samples.size(); ++i)
	{
		const pbio::RawSample &sample = *samples[i];

		// get a face rectangle
		const pbio::RawSample::Rectangle rectangle = sample.getRectangle();

		// set a point to place information for this face
		cv::Point2f text_point(
			rectangle.x + rectangle.width + 3,
			rectangle.y + 10);

		const float text_line_height = 22;

		// draw facial points
		// red color for all points
		// green for left eye
		// yellow for right eye
		// (yes, there is a mess with left and right eyes in face_sdk api,
		// but if we fix it now we will lose compatibility with previous versions)
		if(_flag_points)
		{
			const std::vector<pbio::RawSample::Point> points = sample.getLandmarks();

			for(int j = -2; j < (int) points.size(); ++j)
			{
				const pbio::RawSample::Point p =
					j == -2 ?
						sample.getLeftEye() :
					j == -1 ?
						sample.getRightEye() :
					points[j];

				const cv::Scalar color =
					j == -2 ?
						cv::Scalar(50, 255, 50) :
					j == -1 ?
						cv::Scalar(50, 255, 255) :
					cv::Scalar(50, 50, 255);


				cv::circle(
					draw_image,
					cv::Point2f(p.x, p.y),
					j < 0 ? 4 : 2,
					color,
					-1,
					cv::LINE_AA);
			}
		}

		// draw rectangle
		if(_flag_positions)
		{

			cv::rectangle(
				draw_image,
				cv::Rect(
					rectangle.x,
					rectangle.y,
					rectangle.width,
					rectangle.height),
				cv::Scalar(50, 50, 255),
				2,
				cv::LINE_AA);
		}

		// draw age and gender
		if( _flag_age_gender )
		{
			const pbio::AgeGenderEstimator::AgeGender age_gender =
				_age_geder_estimator->estimateAgeGender(sample);

			std::ostringstream age_text;

			age_text << "age: ";

			switch(age_gender.age)
			{
				case pbio::AgeGenderEstimator::AGE_KID    : age_text << "kid    "; break;
				case pbio::AgeGenderEstimator::AGE_YOUNG  : age_text << "young  "; break;
				case pbio::AgeGenderEstimator::AGE_ADULT  : age_text << "adult  "; break;
				case pbio::AgeGenderEstimator::AGE_SENIOR : age_text << "senior "; break;
			}

			age_text << "years: " << std::setprecision(3) << age_gender.age_years;

			puttext(
				draw_image,
				age_text.str(),
				text_point);
			text_point.y += text_line_height;

			puttext(
				draw_image,
				age_gender.gender == pbio::AgeGenderEstimator::GENDER_FEMALE ? "gender: female" :
				age_gender.gender == pbio::AgeGenderEstimator::GENDER_MALE ? "gender: male" : "?",
				text_point);
			text_point.y += text_line_height;

			text_point.y += text_line_height / 3;
		}

		// draw emotions
		if( _flag_emotions )
		{
			const std::vector<pbio::EmotionsEstimator::EmotionConfidence> emotions =
				_emotions_estimator->estimateEmotions(sample);

			for(size_t i = 0; i < emotions.size(); ++i)
			{
				const pbio::EmotionsEstimator::Emotion emotion = emotions[i].emotion;
				const float confidence = emotions[i].confidence;

				cv::rectangle(
					draw_image,
					cv::Rect(
						text_point.x,
						text_point.y - text_line_height / 2,
						100 * confidence,
						text_line_height),
					emotion == pbio::EmotionsEstimator::EMOTION_NEUTRAL  ? cv::Scalar(255, 0, 0) :
					emotion == pbio::EmotionsEstimator::EMOTION_HAPPY    ? cv::Scalar(0, 255, 0) :
					emotion == pbio::EmotionsEstimator::EMOTION_ANGRY    ? cv::Scalar(0, 0, 255) :
					emotion == pbio::EmotionsEstimator::EMOTION_SURPRISE ? cv::Scalar(0, 255, 255) :
						cv::Scalar(0, 0, 0),
					-1);

				puttext(
					draw_image,
					emotion == pbio::EmotionsEstimator::EMOTION_NEUTRAL  ? "neutral" :
					emotion == pbio::EmotionsEstimator::EMOTION_HAPPY    ? "happy" :
					emotion == pbio::EmotionsEstimator::EMOTION_ANGRY    ? "angry" :
					emotion == pbio::EmotionsEstimator::EMOTION_SURPRISE ? "surprise" : "?",
					text_point + cv::Point2f(100, 0));

				text_point.y += text_line_height;

				text_point.y += text_line_height / 3;
			}
		}


		// draw angles text
		if( _flag_angles )
		{
			std::ostringstream yaw, pitch, roll;
			yaw << "yaw: " << (0.1f * int(10 * sample.getAngles().yaw + 0.5f));
			pitch << "pitch: " << (0.1f * int(10 * sample.getAngles().pitch + 0.5f));
			roll << "roll: " << (0.1f * int(10 * sample.getAngles().roll + 0.5f));

			puttext(draw_image, yaw.str(), text_point);
			text_point.y += text_line_height;

			puttext(draw_image, pitch.str(), text_point);
			text_point.y += text_line_height;

			puttext(draw_image, roll.str(), text_point);
			text_point.y += text_line_height;

			text_point.y += text_line_height / 3;
		}

		// draw angles vectors
		if( _flag_angles_vectors )
		{
			const pbio::RawSample::Angles angles = sample.getAngles();

			const float cos_yaw   = cos( angles.yaw   * CV_PI / 180 );
			const float sin_yaw   = sin( angles.yaw   * CV_PI / 180 );

			const float cos_pitch = cos( angles.pitch * CV_PI / 180 );
			const float sin_pitch = sin( angles.pitch * CV_PI / 180 );

			const float cos_roll  = cos( angles.roll  * CV_PI / 180 );
			const float sin_roll  = sin( angles.roll  * CV_PI / 180 );


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

			const cv::Point2f center(
				(sample.getLeftEye().x + sample.getRightEye().x) * 0.5f,
				(sample.getLeftEye().y + sample.getRightEye().y) * 0.5f);

			const float length = (rectangle.width + rectangle.height) * 0.3f;

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

		// draw quality text
		if( _flag_quality )
		{
			const pbio::QualityEstimator::Quality q =
				_quality_estimator->estimateQuality(sample);

			std::ostringstream lighting, noise, sharpness, flare;

			lighting << "lighting: " << q.lighting;
			puttext(draw_image, lighting.str(), text_point);
			text_point.y += text_line_height;

			noise << "noise: " << q.noise;
			puttext(draw_image, noise.str(), text_point);
			text_point.y += text_line_height;

			sharpness << "sharpness: " << q.sharpness;
			puttext(draw_image, sharpness.str(), text_point);
			text_point.y += text_line_height;

			flare << "flare: " << q.flare;
			puttext(draw_image, flare.str(), text_point);
			text_point.y += text_line_height;

			text_point.y += text_line_height / 3;
		}

		// draw liveness text
		if( _flag_liveness )
		{
			// here we get/create the liveness estimator that work with this face
			const int id = sample.getID();
			if(!liveness2d)
			{
				liveness2d = _service->createLiveness2DEstimator(liveness_config);
			}
			pbio::Liveness2DEstimator::LivenessAndScore liveness_2d_result;
			liveness_2d_result = liveness2d->estimate(sample);
			std::stringstream ss;
			ss << std::fixed << std::setprecision(3) << liveness_2d_result.score;
			std::string score_str = ss.str();
			puttext(
				draw_image,
				std::string("liveness: ") + (
					liveness_2d_result.liveness == pbio::Liveness2DEstimator::REAL ? score_str + " - real" :
					liveness_2d_result.liveness == pbio::Liveness2DEstimator::FAKE ? score_str + " - fake" :
					liveness_2d_result.liveness == pbio::Liveness2DEstimator::NOT_ENOUGH_DATA ? "not enough data" : "??"),
				text_point);

			text_point.y += text_line_height;
			text_point.y += text_line_height / 3;
		}

		// draw face attribute (masked_face)
		if(_flag_masked_face)
		{
			if(!_face_attributes_estimator)
			{
				_face_attributes_estimator = _service->createFaceAttributesEstimator("face_mask_estimator.xml");
			}
			pbio::FaceAttributesEstimator::Attribute attr = _face_attributes_estimator->estimate(sample);
			std::stringstream ss;
			ss << std::fixed << std::setprecision(3) << attr.score;
			std::string score_str = ss.str();
			puttext(
				draw_image,
				std::string("masked: ") + std::string(attr.verdict ? "true - " : "false - ") + score_str,
				text_point);

			text_point.y += text_line_height;
			text_point.y += text_line_height / 3;
		}

		// draw face quality
		if( _flag_face_quality )
		{
			const float quality = _face_quality_estimator->estimateQuality(sample);

			std::ostringstream ss;

			ss << "face quality: " << quality;
			puttext(draw_image, ss.str(), text_point);
			text_point.y += text_line_height;
			text_point.y += text_line_height / 3;
		}

		// draw face cuts
		for(int cut_i = 0; cut_i < 3; ++cut_i)
		{
			if( (cut_i == 0 && ! _flag_cutting_base) ||
				(cut_i == 1 && ! _flag_cutting_full) ||
				(cut_i == 2 && ! _flag_cutting_token) )
			{
				continue;
			}


			puttext(
				draw_image,
				cut_i == 0 ? "base cut:" :
				cut_i == 1 ? "full cut:" :
				cut_i == 2 ? "token cut:" : "?? cut",
				text_point);
			text_point.y += text_line_height / 2;

			std::ostringstream obuf;
			sample.cutFaceImage(
				obuf,
				pbio::RawSample::IMAGE_FORMAT_BMP,
				cut_i == 0 ? pbio::RawSample::FACE_CUT_BASE :
				cut_i == 1 ? pbio::RawSample::FACE_CUT_FULL_FRONTAL :
				cut_i == 2 ? pbio::RawSample::FACE_CUT_TOKEN_FRONTAL :
				(pbio::RawSample::FaceCutType) -1);

			const std::string sbuf = obuf.str();

			const cv::Mat_<uchar> cvbuf(1, sbuf.length(), (uchar*) sbuf.c_str());

			cv::Mat img = cv::imdecode(cvbuf, -1);

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

	// draw checkboxes
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

	// show image with drawed information
	cv::imshow("demo", draw_image);

	// register callback on mouse events
	cv::setMouseCallback("demo", onMouse, this);
}


void Worker::onMouse(int event, int x, int y, int, void* this_ptr_)
{
	Worker* const this_ptr = static_cast<Worker*>( this_ptr_ );

	if(event != cv::EVENT_LBUTTONDOWN &&
		event != cv::EVENT_RBUTTONDOWN &&
		event != cv::EVENT_MBUTTONDOWN)
	{
		return;
	}

	// if mouse click is in some flag's rectangle
	// change flag state
	for(int i = 0; i < this_ptr->flags_count; ++i)
	{
		const cv::Rect r = this_ptr->flag_rect(i);

		if(x >= r.x && y >= r.y && x <= r.x + r.width && y <= r.y + r.height)
			this_ptr->flag(i) = !this_ptr->flag(i);
	}
}
