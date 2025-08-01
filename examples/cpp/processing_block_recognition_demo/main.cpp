#if defined(_WIN32)
#define NOMINMAX
#endif

#include <map>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <api/Service.h>

using Context = api::Context;

#include "../console_arguments_parser/ConsoleArgumentsParser.h"

void recognitionSample(std::string sdk_path, std::string input_image_path1, std::string input_image_path2, std::string window, std::string output, std::string mode, std::string modification);

int main(int argc, char **argv)
{
	std::cout << "usage: " << argv[0] <<
		" [--mode verify | identify]"
		" [--input_image <path to image>]"
		" [--input_image2 <path to image>]"
		" [--sdk_path ..]"
		" [--window <yes/no>]"
		" [--output <yes/no>]"
		" [--modification <30, 50, 100, 1000>]"
		<< std::endl;

	ConsoleArgumentsParser parser(argc, argv);
	const std::string mode               = parser.get<std::string>("--mode", "verify");
	const std::string input_image_path   = parser.get<std::string>("--input_image");
	const std::string input_image_path2  = parser.get<std::string>("--input_image2");
	const std::string sdk_dir            = parser.get<std::string>("--sdk_path", "..");
	const std::string window             = parser.get<std::string>("--window", "yes");
	const std::string output             = parser.get<std::string>("--output", "no");
	const std::string modification 		 = parser.get<std::string>("--modification", "1000");

	try{
		if (mode != "verify" && mode != "identify")
			throw std::runtime_error("there is no modifier " + mode);

		recognitionSample(sdk_dir, input_image_path, input_image_path2, window, output, mode, modification);
	}catch(const std::exception &e){
		std::cout << "! exception catched: '" << e.what() << "' ... exiting" << std::endl;
		return 1;
	}

	return 0;
}

static const std::map<int, std::string> CvTypeToStr{{CV_8U,"uint8_t"}, {CV_8S,"int8_t"}, {CV_16U,"uint16_t"}, {CV_16S,"int16_t"},
													{CV_32S,"int32_t"}, {CV_32F,"float"}, {CV_64F,"double"}};

void cvMatToBSM(api::Context& bsmCtx, const cv::Mat& img, bool copy=false)
{
	const cv::Mat& input_img = img.isContinuous() ? img : img.clone(); // setDataPtr requires continuous data
	size_t copy_sz = (copy || !img.isContinuous()) ? input_img.total()*input_img.elemSize() : 0;
	bsmCtx["format"] = "NDARRAY";
	bsmCtx["blob"].setDataPtr(input_img.data, copy_sz);
	bsmCtx["dtype"] = CvTypeToStr.at(input_img.depth());
	for(int i = 0; i < input_img.dims; ++i)
		bsmCtx["shape"].push_back(input_img.size[i]);
	bsmCtx["shape"].push_back(input_img.channels());
}

void drawBBox(const api::Context& obj, cv::Mat& image, std::string output, cv::Scalar color=cv::Scalar(0, 255, 0)){
	const api::Context& rectCtx = obj.at("bbox");
	cv::Rect rect(cv::Point{static_cast<int>(rectCtx[0].getDouble()*image.cols), static_cast<int>(rectCtx[1].getDouble()*image.rows)},
				  cv::Point{static_cast<int>(rectCtx[2].getDouble()*image.cols), static_cast<int>(rectCtx[3].getDouble()*image.rows)});
	cv::rectangle(image, rect, color, 2);
	if (output == "yes")
		std::cout << "BBox coordinates: " << static_cast<int>(rectCtx[0].getDouble()*image.cols) << ", " << static_cast<int>(rectCtx[1].getDouble()*image.rows) << ", " << static_cast<int>(rectCtx[2].getDouble()*image.cols) << ", " << static_cast<int>(rectCtx[3].getDouble()*image.rows) << "\n";
}


cv::Mat getCrop(const api::Context& obj, cv::Mat &image)
{
	int img_w = image.cols;
	int img_h = image.rows;

	const api::Context& rectCtx = obj.at("bbox");
	int x = static_cast<int>(rectCtx[0].getDouble() * img_w);
	int y = static_cast<int>(rectCtx[1].getDouble() * img_h);
	int width = static_cast<int>(rectCtx[2].getDouble() * img_w) - x;
	int height = static_cast<int>(rectCtx[3].getDouble() * img_h) - y;

	cv::Rect rect(std::max(0, x - int(width * 0.25)), std::max(0, y - int(height * 0.25)),
				  std::min(img_w, int(width * 1.5)), std::min(img_h, int(height * 1.5)));

	return image(rect);
}

void getObjectsWithMaxConfidence(api::Context& data, api::Context& result){
	double max_confidence = 0;
	int index_max_confidence = 0;
	for (int i = 0; i < data["objects"].size(); i++)
	{
		if(data["objects"][i]["class"].getString().compare("face"))
			continue;

		double confidence = data["objects"][i]["confidence"].getDouble();
		if (confidence > max_confidence)
			index_max_confidence = i;
	}

	result["objects"].push_back(data["objects"][index_max_confidence]);
	result["image"] = data["image"];
}

void checkFileExist(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("file " + path + "  not open");
	file.close();
}


api::Context getFaces(pbio::FacerecService &service, api::ProcessingBlock &faceDetector, api::ProcessingBlock &faceFitter, cv::Mat &image){
	cv::Mat input_image;
	cv::cvtColor(image, input_image, cv::COLOR_BGR2RGB);

	Context ioData = service.createContext();
	Context imgCtx = service.createContext();
	cvMatToBSM(imgCtx, input_image, true);
	ioData["image"] = imgCtx;

	faceDetector(ioData);
	faceFitter(ioData);

	return ioData;
}

void recognitionSample(std::string sdk_path, std::string input_image_path1, std::string input_image_path2, std::string window, std::string output, std::string mode, std::string modification)
{
	api::Service service = api::Service::createService(sdk_path);

	Context detectorCtx = service.createContext();
	Context fitterCtx = service.createContext();
	Context faceTemplateExtractorCtx = service.createContext();
	
	detectorCtx["unit_type"] = "FACE_DETECTOR";
	detectorCtx["modification"] = "ssyv_light";

	fitterCtx["unit_type"] = "FACE_FITTER";
	fitterCtx["modification"] = "fda";

	faceTemplateExtractorCtx["unit_type"] = "FACE_TEMPLATE_EXTRACTOR";
	faceTemplateExtractorCtx["modification"] = modification;

	api::ProcessingBlock faceDetector = service.createProcessingBlock(detectorCtx);
	api::ProcessingBlock faceFitter = service.createProcessingBlock(fitterCtx);
	api::ProcessingBlock faceTemplateExtractor = service.createProcessingBlock(faceTemplateExtractorCtx);

	checkFileExist(input_image_path1);
	checkFileExist(input_image_path2);

	cv::Mat image = cv::imread(input_image_path1, cv::IMREAD_COLOR);
	cv::Mat image2 = cv::imread(input_image_path2, cv::IMREAD_COLOR);

	Context ioData = getFaces(service, faceDetector, faceFitter, image);
	Context ioData2 = getFaces(service, faceDetector, faceFitter, image2);

	if (!ioData["objects"].size())
		throw std::runtime_error("no face detected on " + input_image_path1 + " image");

	if (!ioData2["objects"].size())
		throw std::runtime_error("no face detected on " + input_image_path2 + " image");

	if (mode == "verify" && ioData["objects"].size() != 1)
		throw std::runtime_error("many faces in verify mode on" + input_image_path1 + " image");

	if (ioData2["objects"].size() != 1)
		throw std::runtime_error("many faces on" + input_image_path2 + " image");

	///////////Recognizer////////////////
	faceTemplateExtractor(ioData);
	faceTemplateExtractor(ioData2);
	/////////////////////////////////////

	if (mode == "verify")
	{
		Context verificationConfig = service.createContext();
		Context verificationData = service.createContext();

		verificationConfig["unit_type"] = "VERIFICATION_MODULE";
		verificationConfig["modification"] = modification;

		api::ProcessingBlock verificationModule = service.createProcessingBlock(verificationConfig);

		verificationData["template1"] = ioData["objects"][0]["face_template"];
		verificationData["template2"] = ioData2["objects"][0]["face_template"];

		///////////Verification////////////////
		verificationModule(verificationData);
		//////////////////////////////////

		double distance = verificationData["result"]["distance"].getDouble();
		double score = verificationData["result"]["score"].getDouble();

		cv::Scalar color = score >= 0.85 ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
		drawBBox(ioData["objects"][0], image, output, color);
		drawBBox(ioData2["objects"][0], image2, output, color);

		cv::Mat crop1 = getCrop(ioData["objects"][0], image);
		cv::Mat crop2 = getCrop(ioData2["objects"][0], image2);

		cv::resize(crop1, crop1, cv::Size(320, 480));
		cv::resize(crop2, crop2, cv::Size(320, 480));
		cv::Mat3b result(480, 640, cv::Vec3b(0,0,0));

		crop1.copyTo(result(cv::Rect(0, 0, crop1.cols, crop1.rows)));
		crop2.copyTo(result(cv::Rect(crop1.cols, 0, crop2.cols, crop2.rows)));

		std::cout << "distance = " << distance << "\n";
		std::cout << "verdict = " << (score >= 0.85 ? "True" : "False") << "\n";
		std::cout << "score = " << score << "\n";

		if (window == "yes"){
			cv::imshow("result", result);
			cv::waitKey();
			cv::destroyWindow("result");
		}
	}
	else
	{
		Context matcherConfig = service.createContext();
		Context templateIndexConfig = service.createContext();
		Context templates = service.createContext();
		Context matcherData = service.createContext();

		matcherConfig["unit_type"] = "MATCHER_MODULE";
		matcherConfig["modification"] = modification;

		templateIndexConfig["unit_type"] = "TEMPLATE_INDEX";
		templateIndexConfig["modification"] = modification;

		api::ProcessingBlock matcherModule = service.createProcessingBlock(matcherConfig);
		api::ProcessingBlock templateIndex = service.createProcessingBlock(templateIndexConfig);

		for (const Context& object : ioData["objects"])
		{
			templates.push_back(object["face_template"]);
		}

		ioData["templates"] = std::move(templates);

		templateIndex(ioData);

		matcherData["knn"] = 1l;
		matcherData["template_index"] = ioData["template_index"];
		matcherData["queries"] = ioData2["objects"][0]["face_template"]["template"];

		///////////Matcher////////////////
		matcherModule(matcherData);
		//////////////////////////////////

		int find_index = static_cast<int>(matcherData["results"][0]["index"].getLong());
		double distance = matcherData["results"][0]["distance"].getDouble();
		double score = matcherData["results"][0]["score"].getDouble();

		std::cout << "distance = " << distance <<" \n";
		std::cout << "verdict = " << (score >= 0.85 ? "True" : "False") <<" \n";
		std::cout << "index = " << find_index <<" \n";
		std::cout << "score = " << score << " \n";

		for(int i = 0; i < ioData["objects"].size(); i++)
		{
			const api::Context &obj = ioData["objects"][i];
			cv::Scalar color = (i == find_index && score >= 0.85) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
			drawBBox(obj, image, output, color);
		}

		if (window == "yes"){
			cv::imshow("result", image);
			cv::waitKey();
			cv::destroyWindow("result");
		}
	}
}
