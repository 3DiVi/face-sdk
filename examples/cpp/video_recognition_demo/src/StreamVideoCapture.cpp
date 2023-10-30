/**
	\file video_recognition_demo/src/StreamVideoCapture.cpp
*/

#include "StreamVideoCapture.h"


StreamVideoCapture::StreamVideoCapture(const std::string url, bool no_repeat_on_empty) :
	stream_url(url),
	_no_repeat_on_empty(no_repeat_on_empty)
{
	_shutdown = true;
	_grab_success = false;
	open(stream_url);
}

bool StreamVideoCapture::open(const std::string url)
{
	bool opened = cv::VideoCapture::open(url);
	if (opened)
	{
		_shutdown = false;
		_new_grab = false;
		_grab_thread = std::thread(&StreamVideoCapture::grabThreadFunc, this);
	}
	return opened;
}

void StreamVideoCapture::stopGrabThread()
{
	_shutdown = true;
	if(_grab_thread.joinable())
		_grab_thread.join();
}

StreamVideoCapture::~StreamVideoCapture()
{
	stopGrabThread();
}

StreamVideoCapture& StreamVideoCapture::operator >> (CV_OUT cv::Mat& image)
{
	for (;; std::this_thread::sleep_for(std::chrono::milliseconds((int) grab_sleep)))
	{
		if (!isOpened() && !_no_repeat_on_empty)
		{
			// reopen videocapture
			stopGrabThread();
			open(stream_url);
			image = cv::Mat();
			continue;
		}
		else if(isOpened())
		{
			// check wait time
			const std::chrono::steady_clock::time_point start_wait = std::chrono::steady_clock::now();

			std::lock_guard<std::mutex> _grab_guard(_grab_mutex);

			if( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_wait).count() > max_grab_duration)
			{
				// close camera if stream is paused
				release();
				image = cv::Mat();
				continue;
			}

			// wait for the data
			if (!_new_grab)
			{
				_grab_success = grab();
				_new_grab = true;
			}

			// decode the frame
			bool _retrieve_success = false;

			if(_grab_success)
				_retrieve_success = retrieve(image);

			_new_grab = false;

			// if failed try to reopen
			if(!_retrieve_success || image.empty())
			{
				release();
				image = cv::Mat();
				continue;
			}
		}
		else
		{
			release();
			image = cv::Mat();
			_shutdown = true;
		}
		break;
	}
	return *this;
}

void StreamVideoCapture::grabThreadFunc()
{
	while(!_shutdown)
	{
		{
			std::lock_guard<std::mutex> _grab_guard(_grab_mutex);
			_grab_success = grab();
			_new_grab = true;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds((int) grab_sleep));
	}
}
