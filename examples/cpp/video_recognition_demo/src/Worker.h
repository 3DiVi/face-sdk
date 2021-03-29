/**
	\file video_recognition_demo/src/Worker.h
*/

#ifndef __VIDEO_RECOGNITION_DEMO__WORKER_H__
#define __VIDEO_RECOGNITION_DEMO__WORKER_H__

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

#include <facerec/libfacerec.h>
#include <pbio/example/CVRawImage.h>

#include "Database.h"

#include "ImageAndDepthSource.h"


// this worker will manage one source:
//  - get frames from the sources and put them in a VideoWorker
//  - set callbacks
//  - draw results
class Worker
{
public:

	// constants for drawing
	static const int thumbnail_size = 150;
	static const int draw_border = 5;
	static const int max_count_in_stripe = 6;


	Worker(
		const Database &database,
		const pbio::VideoWorker::Ptr video_worker,
		const cv::Ptr<ImageAndDepthSource> source,
		const int stream_id,
		std::mutex &draw_image_mutex,
		cv::Mat &draw_image,
		const float frame_fps_limit);

	~Worker();

	void clearAfterStreamReset(const int track_id_threshold);

private:

	struct StiPersonData
	{
		pbio::RawSample::Ptr first_sample;
	};

	// data for each person that currently in process
	struct FaceData
	{
		pbio::RawSample::Ptr sample;
		bool lost;
		bool weak;
		int frame_id;
		std::chrono::steady_clock::time_point lost_time;
		int match_database_index;

		float draw_multilier;

		bool sti_person_id_set;
		int sti_person_id;

		pbio::ActiveLiveness::ActiveLivenessStatus active_liveness_status;

		bool age_gender_set;
		pbio::AgeGenderEstimator::AgeGender age_gender;

		bool emotions_set;
		pbio::EmotionsEstimator::EstimatedEmotionsVector emotions;

		FaceData()
		: match_database_index(-1)
		, lost(true)
		, draw_multilier(1)
		, age_gender_set(false)
		, emotions_set(false)
		{
			// nothing else
		}
	};

	struct SharedImageAndDepth
	{
		cv::Mat image;
		cv::Mat depth;
	};

	// data for drawing
	struct DrawingData
	{
		bool updated;

		cv::Mat frame;
		cv::Mat depth;
		int frame_id;

		// map<track_id , face>
		std::map<int, FaceData> faces;

		// map<sti_person_id, data>
		std::map<int, StiPersonData> sti_persons_data;

		DrawingData() : updated(false) {}
	};

	// tracking callback function
	//  userdata is supposed to be pointer to this Worker
	static
	void TrackingCallback(
		const pbio::VideoWorker::TrackingCallbackData &data,
		void* const userdata);


	// tracking lost callback function
	//  userdata is supposed to be pointer to this Worker
	static
	void TrackingLostCallback(
		const pbio::VideoWorker::TrackingLostCallbackData &data,
		void* const userdata);


	// match found callback function
	//  userdata is supposed to be pointer to this Worker
	static
	void MatchFoundCallback(
		const pbio::VideoWorker::MatchFoundCallbackData &data,
		void* const userdata);

	// sti person outdated callback function
	//  userdata is supposed to be pointer to this Worker
	static
	void StiPersonOutdatedCallback(
		const pbio::VideoWorker::StiPersonOutdatedCallbackData &data,
		void* const userdata);

	// here we grab frames from the sources
	// and put them in a VideoWorker
	void CameraThreadFunc();

	// draw results
	static
	cv::Mat Draw(
		const DrawingData &data,
		const Database &database);

	// here we draw results
	void DrawingThreadFunc();



	std::mutex &_draw_image_mutex;
	cv::Mat &_draw_image;

	pbio::VideoWorker::Ptr _video_worker;
	cv::Ptr<ImageAndDepthSource> _source;
	const int _stream_id;

	const Database &_database;

	int _tracking_callback_id;
	int _tracking_lost_callback_id;
	int _match_found_callback_id;
	int _sti_person_outdated_callback_id;


	std::atomic<bool> _shutdown;
	std::thread _camera_thread;
	std::thread _drawing_thread;

	int _frame_min_delay_ms;

	std::queue<std::pair<int, SharedImageAndDepth> > _frames;
	cv::Mat _last_depth_image;
	std::mutex _frames_mutex;

	DrawingData _drawing_data;
	std::mutex _drawing_data_mutex;
};

#endif  // __VIDEO_RECOGNITION_DEMO__WORKER_H__
