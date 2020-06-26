#ifndef WORKER_H
#define WORKER_H

#include "qcameracapture.h"
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

		FaceData() :
			lost(true),
			match_database_index(-1)
		{
		}
	};

	// Drawing data
	struct DrawingData
	{
		bool updated;
		int frame_id;

		// map<track_id , face>
		std::map<int, FaceData> faces;

		QCameraCapture::FramePtr frame;

		DrawingData() : updated(false)
		{
		}
	};

	Worker(
		const pbio::FacerecService::Ptr service,
		const FaceSdkParameters face_sdk_parameters);
	~Worker();

	void addFrame(QCameraCapture::FramePtr frame);

	void getDataToDraw(DrawingData& data);

	const Database& getDatabase() const
	{
		return _database;
	}

private:
	DrawingData _drawing_data;
	std::mutex _drawing_data_mutex;

	std::queue<std::pair<int, QCameraCapture::FramePtr> > _frames;
	std::mutex _frames_mutex;

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
