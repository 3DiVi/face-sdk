#ifndef __COMMON_SOURCE_H__0x9762092f__
#define __COMMON_SOURCE_H__0x9762092f__

#include <opencv2/opencv.hpp>

#include "StreamVideoCapture.h"

#include "MAssert.h"

#include "ImageAndDepthSource.h"


#ifndef CV_CAP_PROP_FRAME_WIDTH
#define CV_CAP_PROP_FRAME_WIDTH cv::CAP_PROP_FRAME_WIDTH
#endif

#ifndef CV_CAP_PROP_FRAME_HEIGHT
#define CV_CAP_PROP_FRAME_HEIGHT cv::CAP_PROP_FRAME_HEIGHT
#endif


class OpencvSource : public ImageAndDepthSource
{
public:

	virtual ~OpencvSource() {}

	virtual void get(ImageAndDepth& data) override;

	OpencvSource(const std::string cam_or_url, bool no_repeat_on_empty);

private:

	cv::Ptr<cv::VideoCapture> capturer;
};

OpencvSource::OpencvSource(const std::string cam_or_url, bool no_repeat_on_empty)
{
	MAssert(!cam_or_url.empty(),);

	// check if cam_or_url is number
	bool stream = false;

	for(size_t i = 0; i < cam_or_url.length(); ++i)
		stream = stream ||
			(cam_or_url[i] < '0') ||
			(cam_or_url[i] > '9');

	if(stream)
	{
		// open stream
		std::cout << "opening stream '" << cam_or_url << "'" << std::endl;
		capturer = new StreamVideoCapture(cam_or_url, no_repeat_on_empty);
	}
	else
	{
		// convert to integer
		std::istringstream istr(cam_or_url);
		int cam_id = -1;
		istr >> cam_id;

		MAssert(cam_id >= 0, "wrong webcam id");

		// open vebcam
		std::cout << "opening webcam " << cam_id << std::endl;
		capturer = new cv::VideoCapture(cam_id);
		MAssert(capturer->isOpened(), "webcam not opened");

		// set resolution
		capturer->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
		capturer->set(CV_CAP_PROP_FRAME_HEIGHT, 720);

		MAssert(capturer->isOpened(), "webcam not opened");
	}

	{
		// sometimes first few frames can be empty even if camera is good
		// so skip few frames
		cv::Mat frame;
		for(int i = 0; i < 10; ++i)
			*capturer >> frame;
	}

	// check first two frames
	cv::Mat image1, image2;
	(*capturer) >> image1;
	(*capturer) >> image2;
	std::cout << "image1 size: " << image1.size() << std::endl;
	std::cout << "image2 size: " << image2.size() << std::endl;

	MAssert(
		!image1.empty() &&
		!image2.empty() &&
		image1.size() == image2.size() &&
		image1.type() == CV_8UC3 &&
		image2.type() == CV_8UC3,
		"error opening webcam or stream");
}

inline
void OpencvSource::get(ImageAndDepth& data)
{
	cv::Mat image;
	(*capturer) >> image;

	data.color_image.assign(
		(const uint8_t*)image.data,
		(const uint8_t*)image.data + image.step[0] * image.rows);

	data.color_width = image.cols;
	data.color_height = image.rows;
	data.color_stride_in_bytes = image.step[0];
	data.color_format = pbio::IRawImage::FORMAT_BGR;
}

#endif // __COMMON_SOURCE_H__0x9762092f__
