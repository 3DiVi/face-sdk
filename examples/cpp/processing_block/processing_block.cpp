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

void drawObjects(const pbio::Context& data, cv::Mat& image, std::string class_filter=std::string())
{
	for(const auto& obj : data.at("objects"))
	{
		std::string class_name = obj.at("class").getString();
		if(!class_filter.empty() && class_name.compare(class_filter))
			continue;
		const auto& rectCtx = obj.at("bbox");
		cv::Rect rect(cv::Point{static_cast<int>(rectCtx[0].getDouble()*image.size[1]), static_cast<int>(rectCtx[1].getDouble()*image.size[0])},
					  cv::Point{static_cast<int>(rectCtx[2].getDouble()*image.size[1]), static_cast<int>(rectCtx[3].getDouble()*image.size[0])});
		cv::rectangle(image, rect, {0, 255, 0}, 1);
		if(class_filter.empty())
			cv::putText(image, class_name, cv::Point{rect.x, rect.y+20}, cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),1,false);
	}
}

void drawEmotions(const pbio::Context& data, cv::Mat& image)
{
	drawObjects(data, image, std::string("face"));
	for(const auto& obj : data.at("objects"))
	{
		std::string class_name = obj.at("class").getString();
		if(class_name.compare("face"))
				continue;

		const pbio::Context& emotions = obj.at("emotions");
		cv::Point text_point{static_cast<int>(obj.at("bbox")[2].getDouble()*image.size[1]),
							 static_cast<int>(obj.at("bbox")[1].getDouble()*image.size[0])};

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

			cv::putText(image, emotion, text_point + cv::Point(top_shift * lenght_coef, 0), cv::FONT_HERSHEY_DUPLEX, text_line_height / (font_height*2), cv::Scalar(255,255,0),1,false);

			text_point.y += text_line_height;
			text_point.y += text_line_height / 3;
		}
	}
};

void drawGender(const pbio::Context& data, cv::Mat& image)
{
	drawObjects(data, image, std::string("face"));
	for(const auto& obj : data.at("objects"))
	{
		if(obj.at("class").getString().compare("face"))
			continue;
		cv::Point text_point{static_cast<int>(obj.at("bbox")[2].getDouble()*image.size[1]),
							 static_cast<int>(obj.at("bbox")[1].getDouble()*image.size[0])};
		cv::putText(image, obj.at("gender").getString(), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
	}
};

void drawLiveness(const pbio::Context& data, cv::Mat& image)
{
	drawObjects(data, image, std::string("face"));
	for(const auto& obj : data.at("objects"))
	{
		if(obj.at("class").getString().compare("face"))
			continue;
		cv::Point text_point{static_cast<int>(obj.at("bbox")[2].getDouble()*image.size[1]),
							 static_cast<int>(obj.at("bbox")[1].getDouble()*image.size[0])};
		if(obj.contains("liveness"))
		{
			const pbio::Context& liveness = obj.at("liveness");
			cv::putText(image, liveness.at("value").getString(), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
			text_point.y += 12;
			cv::putText(image, std::to_string(liveness.at("confidence").getDouble()), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
			if(liveness.contains("info"))
			{
				text_point.y += 12;
				cv::putText(image, "info: " + liveness.at("info").getString(), text_point, cv::FONT_HERSHEY_DUPLEX, 0.5,cv::Scalar(0,0,255), 1, false);
			}
		}
	}
};

const std::map<std::string, std::pair<std::string, std::string>> unitTypes {
 {"body",{"HUMAN_BODY_DETECTOR", "/bodydetectors/body.enc"}},
 {"objects",{"OBJECT_DETECTOR", "/objectdetectors/det-object.enc"}},
 {"emotions",{"EMOTION_ESTIMATOR", "/faceanalysis/emotion.enc"}},
 {"gender",{"GENDER_ESTIMATOR", "/faceanalysis/gender.enc"}},
 {"liveness",{"LIVENESS_ESTIMATOR", ""}}};

int main(int argc, char **argv)
{
	// print usage
	std::cout << "usage: " << argv[0] <<
		" [--input_image <path to image>]"
		" [--unit_type body|objects|emotions|gender|liveness]"
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
//		Here legacy facerec capturer is used to get cropped faces.
		if(!unit_type.compare("emotions") || !unit_type.compare("gender"))
		{
			const pbio::Capturer::Ptr capturer = service->createCapturer("common_capturer_uld_fda.xml");
			// capture faces
			std::vector<pbio::RawSample::Ptr> samples = capturer->capture(input_rawimg);
			// process each face
			auto result = service->createContext();
			for(const auto& sample : samples)
			{
				pbio::RawSample::Rectangle rect = sample->getRectangle();
				pbio::RawImage raw_image_crop = input_rawimg.crop(rect);

				auto imgCtx = ioData["image"];	// A shallow copy (reference), auto is deduced to pbio::Context::Ref
												// to make a deep copy define pbio::Context imgCtx = ioData["image"];
				pbio::context_utils::putImage(imgCtx, raw_image_crop);
				processingBlock(ioData);

				// add bboxes to the result
				auto bbox = service->createContext();
				bbox.push_back(double(rect.x) / image.size[1]);
				bbox.push_back(double(rect.y) / image.size[0]);
				bbox.push_back(double(rect.x + rect.width) / image.size[1]);
				bbox.push_back(double(rect.y + rect.height) / image.size[0]);
				result["objects"].push_back(ioData["objects"][0]);
				result["objects"][-1]["bbox"] = std::move(bbox);
			}
			ioData = std::move(result);
		}
		else  // just put the whole image to the Context
		{
			auto imgCtx = ioData["image"];
			pbio::context_utils::putImage(imgCtx, input_rawimg);
			ioData["image"] = imgCtx;
			processingBlock(ioData);
		}

	if(ioData.isNone() || !ioData.contains("objects"))
			std::cerr << "No objects have been detected!" << std::endl;
		else if(!unit_type.compare("body"))
			drawObjects(ioData, image, "body");
		else if(!unit_type.compare("objects"))
			drawObjects(ioData, image);
		else if(!unit_type.compare("emotions"))
			drawEmotions(ioData, image);
		else if(!unit_type.compare("gender"))
			drawGender(ioData, image);
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

