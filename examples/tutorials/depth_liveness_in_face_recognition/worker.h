#ifndef WORKER_H
#define WORKER_H

#include "depthsensorcapture.h"
#include "facesdkparameters.h"
#include "database.h"

#include <facerec/libfacerec.h>

#include <mutex>
#include <queue>


class Worker
{
public:
	// Face data
	struct FaceData
	{
		pbio::RawSample::Ptr sample;
		bool lost;
		int frame_id;
		int match_database_index;
		pbio::DepthLivenessEstimator::Liveness liveness_status;

		FaceData() :
			lost(true),
			match_database_index(-1),
			liveness_status(pbio::DepthLivenessEstimator::NOT_COMPUTED)
		{
		}
	};

	struct SharedImageAndDepth
	{
		DepthSensorCapture::RGBFramePtr color_image;
		DepthSensorCapture::DepthFramePtr depth_image;

		pbio::DepthMapRaw depth_options;
	};

	// Drawing data
	struct DrawingData
	{
		bool updated;
		int frame_id;

		// map<track_id , face>
		std::map<int, FaceData> faces;

		SharedImageAndDepth frame;

		DrawingData() : updated(false)
		{
		}
	};

	Worker(
		const pbio::FacerecService::Ptr service,
		const FaceSdkParameters face_sdk_parameters);
	~Worker();

	void addFrame(const ImageAndDepth& frame);

	void getDataToDraw(DrawingData& data);

	const Database& getDatabase() const
	{
		return _database;
	}

private:
	DrawingData _drawing_data;
	std::mutex _drawing_data_mutex;

	std::queue<std::pair<int, SharedImageAndDepth> > _frames;
	std::mutex _frames_mutex;
	DepthSensorCapture::DepthFramePtr _last_depth_image;
	pbio::DepthMapRaw _last_depth_options;

	pbio::VideoWorker::Ptr _video_worker;

	Database _database;

	static void TrackingCallback(
		const pbio::VideoWorker::TrackingCallbackData &data,
		void* const userdata);

	static void TrackingLostCallback(
		const pbio::VideoWorker::TrackingLostCallbackData &data,
		void* const userdata);

	static void MatchFoundCallback(
		const pbio::VideoWorker::MatchFoundCallbackData &data,
		void* const userdata);

	int _tracking_callback_id;
	int _tracking_lost_callback_id;
	int _match_found_callback_id;
};

#endif // WORKER_H
