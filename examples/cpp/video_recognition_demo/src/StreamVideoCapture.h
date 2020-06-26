/**
	\file video_recognition_demo/src/StreamVideoCapture.h
*/

#ifndef __VIDEO_RECOGNITION_DEMO__STREAM_VIDEO_CAPTURE_H__
#define __VIDEO_RECOGNITION_DEMO__STREAM_VIDEO_CAPTURE_H__

#include <atomic>
#include <mutex>
#include <thread>


#include <opencv2/opencv.hpp>


// specific implementation for streams
// differences:
//   - auto reopen
//   - second thread for data grab prevent
//     frame corruption and increasing of time offset
class StreamVideoCapture : public cv::VideoCapture
{
public:

	StreamVideoCapture(const std::string url);

	virtual bool open(const std::string url);

	virtual ~StreamVideoCapture();

	virtual StreamVideoCapture& operator >> (CV_OUT cv::Mat& image);

private:

	void grabThreadFunc();

	void stopGrabThread();

	static const int grab_sleep = 10;  // ms
	static const int max_grab_duration = 1000;  // ms

	const std::string stream_url;
	std::thread _grab_thread;
	std::atomic<bool> _shutdown;
	std::mutex _grab_mutex;
	bool _grab_success;
	bool _new_grab;
};


#endif  // __VIDEO_RECOGNITION_DEMO__STREAM_VIDEO_CAPTURE_H__
