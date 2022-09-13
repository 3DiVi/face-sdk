#if defined(_WIN32)
#define NOMINMAX
#endif

#include <map>
#include <set>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <facerec/libfacerec.h>

#include "../console_arguments_parser/ConsoleArgumentsParser.h"

static const std::vector<std::pair<std::string,std::string>> bone_map = {
		{"right_ankle","right_knee"},
		{"right_knee","right_hip"},
		{"left_hip","right_hip"},
		{"left_shoulder","left_hip"},
		{"right_shoulder","right_hip"},
		{"left_shoulder","right_shoulder"},
		{"left_shoulder","left_elbow"},
		{"right_shoulder","right_elbow"},
		{"left_elbow","left_wrist"},
		{"right_elbow","right_wrist"},
		{"left_eye","right_eye"},
		{"nose","left_eye"},
		{"left_knee", "left_hip"},
		{"right_ear", "right_shoulder"},
		{"left_ear", "left_shoulder"},
		{"right_eye", "right_ear"},
		{"left_eye", "left_ear"},
		{"nose", "right_eye"},
		{"left_ankle", "left_knee"}
};

void putTextWithRightExpansion(cv::Mat& img, const std::string& text, cv::Point org, int fontFace, double fontScale, cv::Scalar color,
							int thickness = 1, int lineType = cv::LINE_8, bool bottomLeftOrigin = false, int borderType = cv::BORDER_REPLICATE)
{
	int* baseLine = nullptr;
	cv::Size text_sz = cv::getTextSize(text, fontFace, fontScale, thickness, baseLine);
	auto right_border = text_sz.width + org.x - img.cols;

	if(right_border > 0)
		cv::copyMakeBorder(img, img, 0, 0, 0, right_border+2, borderType);

	cv::putText(img, text, org, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
}

void drawObjects(const pbio::Context& data, cv::Mat& image, std::string class_filter=std::string())
{
	const auto width = image.size[1];
	const auto heigth = image.size[0];
	for(const auto& obj : data.at("objects"))
	{
		std::string class_name = obj.at("class").getString();
		if(!class_filter.empty() && class_name.compare(class_filter))
			continue;
		const auto& rectCtx = obj.at("bbox");
		cv::Rect rect(cv::Point{static_cast<int>(rectCtx[0].getDouble()*width), static_cast<int>(rectCtx[1].getDouble()*heigth)},
					  cv::Point{static_cast<int>(rectCtx[2].getDouble()*width), static_cast<int>(rectCtx[3].getDouble()*heigth)});
		cv::rectangle(image, rect, {0, 255, 0}, 1);
		if(class_filter.empty())
			putTextWithRightExpansion(image, class_name, cv::Point{rect.x, rect.y+20}, cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),1,false);
		if(class_filter=="body" && obj.contains("keypoints"))
		{
			const auto& posesCtx = obj["keypoints"];

			for (auto ptr = posesCtx.begin(); ptr != posesCtx.end(); ++ptr){
				std::string key_name = ptr.key();
				auto proj = (*ptr)["proj"];
				cv::circle(image, cv::Point{
							static_cast<int>(proj[0].getDouble() * width),
							static_cast<int>(proj[1].getDouble() * heigth)},
							3, cv::Scalar(0, 255, 0), -1,false);
			}

			// visual skeleton
			for(auto &bone: bone_map){
				std::string key1 = bone.first;
				std::string key2 = bone.second;
				int x1 = posesCtx[key1]["proj"][0].getDouble() * width;
				int y1 = posesCtx[key1]["proj"][1].getDouble() * heigth;
				int x2 = posesCtx[key2]["proj"][0].getDouble() * width;
				int y2 = posesCtx[key2]["proj"][1].getDouble() * heigth;
				cv::line(image, cv::Point{x1, y1}, cv::Point{x2,y2},cv::Scalar(0, 255, 0), 1,
						 cv::LINE_4);
			}
		}
	}
}

void drawEmotions(const pbio::Context& data, cv::Mat& image)
{
	const auto width = image.size[1];
	const auto heigth = image.size[0];
	drawObjects(data, image, std::string("face"));
	for(const auto& obj : data.at("objects"))
	{
		std::string class_name = obj.at("class").getString();
		if(class_name.compare("face"))
				continue;

		const pbio::Context& emotions = obj.at("emotions");
		cv::Point text_point{static_cast<int>(obj.at("bbox")[2].getDouble()*width),
							 static_cast<int>(obj.at("bbox")[1].getDouble()*heigth)};

		std::set<std::pair<double, std::string>> results;
		for(size_t i = 0; i < emotions.size(); ++i)
			results.insert({emotions[i]["confidence"].getDouble(), emotions[i]["emotion"].getString()});
		auto top_shift = results.rbegin()->first;
		double lenght_coef = 50;
		float font_height = 12;
		for(auto emotion_iter = results.rbegin(); emotion_iter != results.rend(); ++emotion_iter)
		{
			const std::string& emotion = emotion_iter->second;
			const double confidence = emotion_iter->first;
			const float text_line_height = font_height * std::max(0.5, confidence/top_shift);
			cv::rectangle(
				image,
				cv::Rect( text_point.x, text_point.y - text_line_height / 2, confidence * lenght_coef, text_line_height),
				emotion == "NEUTRAL"   ? cv::Scalar(255, 0, 0) :
				emotion == "HAPPY"     ? cv::Scalar(0, 255, 0) :
				emotion == "ANGRY"     ? cv::Scalar(0, 0, 255) :
				emotion == "SURPRISED" ? cv::Scalar(0, 255, 255) :
				emotion == "DISGUSTED" ? cv::Scalar(255, 255, 0) :
				emotion == "SAD"       ? cv::Scalar(255, 0, 255) :
				emotion == "SCARED"    ? cv::Scalar(127, 127, 127) :
					cv::Scalar(0, 0, 0),
				-1);

			putTextWithRightExpansion(image, emotion, text_point + cv::Point(top_shift * lenght_coef, 0), cv::FONT_HERSHEY_DUPLEX, text_line_height / (font_height*2), cv::Scalar(255,255,0),1,false);

			text_point.y += text_line_height;
			text_point.y += text_line_height / 3;
		}
	}
};

void drawAgeGenderMaskQuality(const pbio::Context& data, cv::Mat& image, const std::string& className)
{
	const auto width = image.size[1];
	const auto heigth = image.size[0];
	drawObjects(data, image, std::string("face"));
	for(const auto& obj : data.at("objects"))
	{
		if(obj.at("class").getString().compare("face"))
			continue;
		cv::Point text_point{static_cast<int>(obj.at("bbox")[2].getDouble()*width),
							 static_cast<int>(obj.at("bbox")[1].getDouble()*heigth)};
		if(!className.compare("gender"))
			putTextWithRightExpansion(image, obj.at("gender").getString(), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
		else if(!className.compare("age"))
			putTextWithRightExpansion(image, "Age: " + std::to_string(obj.at(className).getLong()), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
		else if(!className.compare("mask"))
			putTextWithRightExpansion(image, "Mask: " + std::to_string(obj.at(className).at("value").getBool()), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
		else if(!className.compare("quality"))
		{
			putTextWithRightExpansion(image, "Quality Score:", cv::Point{text_point.x, text_point.y+20}, cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),1,false);
			putTextWithRightExpansion(image, std::to_string(obj["quality"]["qaa"]["totalScore"].getLong()), cv::Point{text_point.x, text_point.y+50}, cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),1,false);
		}
	}
};

void drawLiveness(const pbio::Context& data, cv::Mat& image)
{
	const auto width = image.size[1];
	const auto heigth = image.size[0];
	drawObjects(data, image, std::string("face"));
	for(const auto& obj : data.at("objects"))
	{
		if(obj.at("class").getString().compare("face"))
			continue;
		cv::Point text_point{static_cast<int>(obj.at("bbox")[2].getDouble()*width),
							 static_cast<int>(obj.at("bbox")[1].getDouble()*heigth)};
		if(obj.contains("liveness"))
		{
			const pbio::Context& liveness = obj.at("liveness");
			putTextWithRightExpansion(image, liveness.at("value").getString(), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
			text_point.y += 12;
			putTextWithRightExpansion(image, std::to_string(liveness.at("confidence").getDouble()), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
			if(liveness.contains("info"))
			{
				text_point.y += 12;
				putTextWithRightExpansion(image, "info: " + liveness.at("info").getString(), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
			}
		}
	}
};

const std::map<std::string, std::pair<std::string, std::string>> unitTypes {
	{"body",{"HUMAN_BODY_DETECTOR", "/bodydetectors/body.enc"}},
	{"face",{"FACE_DETECTOR", "/facedetectors/face.enc"}},
	{"objects",{"OBJECT_DETECTOR", "/objectdetectors/det-object.enc"}},
	{"emotions",{"EMOTION_ESTIMATOR", "/faceanalysis/emotion.enc"}},
	{"age",{"AGE_ESTIMATOR", "/faceanalysis/age_heavy.enc"}},
	{"gender",{"GENDER_ESTIMATOR", "/faceanalysis/gender_heavy.enc"}},
	{"mask",{"MASK_ESTIMATOR", "/faceattributes/mask.enc"}},
	{"liveness",{"LIVENESS_ESTIMATOR", ""}},
	{"quality",{"QUALITY_ASSESSMENT_ESTIMATOR", ""}},
	{"pose",{"HUMAN_POSE_ESTIMATOR", "/humanpose/hpe-td.enc"}},
};

int main(int argc, char **argv)
{
	// print usage
	std::cout << "usage: " << argv[0] <<
		" [--input_image <path to image>]"
		" [--unit_type body|pose|objects|emotions|age|gender|mask|liveness|quality]"
		" [--sdk_path ..]"
		" [--use_cuda]"
		<< std::endl;

#if defined(_WIN32)
		const std::string default_dll_path = "/bin/facerec.dll";
#else
		const std::string default_dll_path = "/lib/libfacerec.so";
#endif

	// parse named params
	ConsoleArgumentsParser parser(argc, argv);

	const std::string input_image_path   = parser.get<std::string>("--input_image");
	const std::string unit_type          = parser.get<std::string>("--unit_type", "objects");
	const std::string sdk_dir            = parser.get<std::string>("--sdk_path", "..");

	auto args = parser.get();
	bool use_cuda = std::find(args.begin(), args.end(), std::string("--use_cuda")) != args.end();

	const std::string dll_path{sdk_dir + default_dll_path};
	const std::string config_dir{sdk_dir + "/conf/facerec"};
	const std::string license_dir{sdk_dir + "/license"};
	const std::string model_dir{sdk_dir + "/share"};
#if defined(_WIN32)
	const std::string lib_dir{sdk_dir + "/bin"};
#else
	const std::string lib_dir{sdk_dir + "/lib"};
#endif

	try
	{
		const pbio::FacerecService::Ptr service =
			pbio::FacerecService::createService(
				dll_path,
				config_dir,
				license_dir);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		auto configCtx = service->createContext();

		if(unitTypes.find(unit_type) == unitTypes.end())
			throw pbio::Error(0x917ca17f, "unit_type not found");

		configCtx["unit_type"] = unitTypes.at(unit_type).first;
		configCtx["model_path"] = model_dir + unitTypes.at(unit_type).second;
		configCtx["ONNXRuntime"]["library_path"] = lib_dir; // optional, no default value, os-specific default search order if not set
		configCtx["use_cuda"] = use_cuda;

		if(!unit_type.compare("quality")) {
			configCtx["config_name"] = "quality_assessment.xml";
			configCtx["sdk_path"] = sdk_dir;
		}

		if(!unit_type.compare("pose"))
			configCtx["label_map"] = model_dir + "/humanpose/label_map_keypoints.txt";

		pbio::ProcessingBlock processingBlock = service->createProcessingBlock(configCtx);

		cv::Mat image;
		image = cv::imread(input_image_path, cv::IMREAD_COLOR);
		if(image.empty())
		{
			std::cerr << "OpenCV Error! Cannot read image file: " << input_image_path << std::endl;
			std::exit(1);
		}
		// RGB image is required
		cv::Mat input_image;
		cv::cvtColor(image, input_image, cv::COLOR_BGR2RGB);
		pbio::RawImage input_rawimg(input_image.cols, input_image.rows, pbio::RawImage::Format::FORMAT_RGB, input_image.data);

		auto ioData = service->createContext();

		if(!unit_type.compare("emotions") || !unit_type.compare("gender") || !unit_type.compare("age") || !unit_type.compare("mask"))
		{
			auto faceCtx = service->createContext();
			faceCtx["unit_type"] = unitTypes.at("face").first;
			faceCtx["model_path"] = model_dir + unitTypes.at("face").second;
			faceCtx["ONNXRuntime"]["library_path"] = lib_dir;
			faceCtx["use_cuda"] = use_cuda;
			faceCtx["confidence_threshold"] = 0.4;
			pbio::ProcessingBlock faceBlock = service->createProcessingBlock(faceCtx);

			auto faceData = service->createContext();
			auto faceImageCtx = faceData["image"];
			pbio::context_utils::putImage(faceImageCtx, input_rawimg);
			faceBlock(faceData);

			auto result = service->createContext();
			for(const auto& obj : faceData.at("objects"))
			{
				const auto& rectCtx = obj.at("bbox");

				int x = static_cast<int>(rectCtx[0].getDouble()*image.size[1]);
				int y = static_cast<int>(rectCtx[1].getDouble()*image.size[0]);
				int width = static_cast<int>(rectCtx[2].getDouble()*image.size[1]) - x;
				int height = static_cast<int>(rectCtx[3].getDouble()*image.size[0]) - y;
				pbio::RawSample::Rectangle rect(x, y, width, height);
				pbio::RawImage raw_image_crop = input_rawimg.crop(rect);

				auto imgCtx = ioData["image"];	// A shallow copy (reference), auto is deduced to pbio::Context::Ref
												// to make a deep copy define pbio::Context imgCtx = ioData["image"];
				pbio::context_utils::putImage(imgCtx, raw_image_crop);
				processingBlock(ioData);

				ioData["objects"][0]["bbox"] = rectCtx;
				result["objects"].push_back(ioData["objects"][0]);
			}
			ioData = std::move(result);
		}
		else  // just put the whole image to the Context
		{
			auto imgCtx = ioData["image"];
			pbio::context_utils::putImage(imgCtx, input_rawimg);
			ioData["image"] = imgCtx;
			if(!unit_type.compare("pose")) // first extract body bboxes
			{
				auto modelDetectorCtx = service->createContext();
				modelDetectorCtx["unit_type"] = unitTypes.at("body").first;
				modelDetectorCtx["model_path"] =  model_dir + unitTypes.at("body").second;
				modelDetectorCtx["ONNXRuntime"]["library_path"] = lib_dir;
				modelDetectorCtx["confidence_threshold"] = 0.4;
				modelDetectorCtx["iou_threshold"] = 0.45;
				modelDetectorCtx["use_cuda"] = use_cuda;
				pbio::ProcessingBlock  bodyDetector = service->createProcessingBlock(modelDetectorCtx);
				bodyDetector(ioData);
			}
			processingBlock(ioData);
		}

	if(ioData.isNone() || !ioData.contains("objects"))
			std::cerr << "No objects have been detected!" << std::endl;
		else if(!unit_type.compare("body") || !unit_type.compare("pose"))
			drawObjects(ioData, image, "body");
		else if(!unit_type.compare("face"))
			drawObjects(ioData, image, "face");
		else if(!unit_type.compare("objects"))
			drawObjects(ioData, image);
		else if(!unit_type.compare("emotions"))
			drawEmotions(ioData, image);
		else if(!unit_type.compare("age") || !unit_type.compare("gender") || !unit_type.compare("mask") || !unit_type.compare("quality"))
			drawAgeGenderMaskQuality(ioData, image, unit_type);
		else if(!unit_type.compare("liveness"))
			drawLiveness(ioData, image);

		cv::imshow("image", image);
		cv::waitKey();

	}
	catch (cv::Exception& e )
	{
		const char* err_msg = e.what();
		std::cerr << "Opencv exception catched: " << '\n' << err_msg << std::endl;
	}
	catch (const pbio::Error& e)
	{
		std::cerr << "facerec exception catched: '" << e.what() << "' code: " << std::hex << e.code() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "exception catched: '" << e.what() << "'" << std::endl;
	}
	return 0;
}
