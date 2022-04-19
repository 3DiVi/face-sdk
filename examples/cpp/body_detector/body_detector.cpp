#include <map>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <facerec/libfacerec.h>
void cvMatToBSM(pbio::Context&, const cv::Mat, bool copy=false);

int main(int argc, char **argv)
{
	std::string input_image_path(argv[1]), model_path(argv[2]);

#ifdef _WIN32
	const std::string dll_path = "./facerec.dll";
#else
	const std::string dll_path = "../lib/libfacerec.so";
#endif
	const std::string config_dir("../conf/facerec");
	const std::string license_dir("../license");

	try
	{
		const pbio::FacerecService::Ptr service =
			pbio::FacerecService::createService(
				dll_path,
				config_dir,
				license_dir);

		std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

		auto modelCtx = service->createContext();

		modelCtx["unit_type"] = "HUMAN_BODY_DETECTOR";			// mandatory
		modelCtx["model_path"] = model_path;					// mandatory
		modelCtx["confidence_threshold"] = 0.5;					// default value is 0.5
		modelCtx["iou_threshold"] = 0.8;						// default value is 0.5
		//modelCtx["ONNXRuntime"]["library_path"] = "../lib/";	// no default value, os-specific default search order

		pbio::ProcessingBlock bodyDetector = service->createProcessingBlock(modelCtx);

		cv::Mat image = cv::imread(input_image_path, cv::IMREAD_COLOR);
		cv::Mat input_image;
		cv::cvtColor(image, input_image, cv::COLOR_BGR2RGB);

		auto imgCtx = service->createContext();

		cvMatToBSM(imgCtx, input_image);

		auto ioData = service->createContext();
		ioData["image"] = imgCtx;

		////////////////////////////
		bodyDetector(ioData);     //
		////////////////////////////

		for(const auto& obj : ioData["objects"])
		{
			if(obj["class"].getString().compare("body"))
				continue;
			const auto& rectCtx = obj.at("bbox");
			cv::Rect rect(cv::Point{rectCtx[0].getLong(), rectCtx[1].getLong()},
						  cv::Point{rectCtx[2].getLong(), rectCtx[3].getLong()});
			cv::rectangle(image, rect, {0, 255, 0}, 1);
		}

		cv::imshow("image", image);
		cv::waitKey();

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

static const std::map<int, std::string> CvTypeToStr{{CV_8U,"uint8_t"}, {CV_8S,"int8_t"}, {CV_16U,"uint16_t"}, {CV_16S,"int16_t"},
													{CV_32S,"int32_t"}, {CV_32F,"float"}, {CV_64F,"double"}};


void cvMatToBSM(pbio::Context& bsmCtx, const cv::Mat img, bool copy)
{
	bsmCtx["format"] = "NDARRAY";
	size_t copy_sz = copy ? img.total()*img.elemSize() : 0;
	bsmCtx["blob"].setDataPtr(img.data, copy_sz);
	bsmCtx["dtype"] = CvTypeToStr.at(img.depth());
	for(int i = 0; i < img.dims; ++i)
		bsmCtx["shape"].push_back(img.size[i]);
	bsmCtx["shape"].push_back(img.channels());
}
