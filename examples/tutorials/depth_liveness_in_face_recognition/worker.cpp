#include "worker.h"
#include "videoframe.h"

#include <assert.h>


Worker::Worker(
	const pbio::FacerecService::Ptr service,
	const FaceSdkParameters face_sdk_parameters)
{
	pbio::FacerecService::Config vwconfig(face_sdk_parameters.videoworker_config);

	vwconfig.overrideParameter("not_found_match_found_callback", 1);

	vwconfig.overrideParameter("depth_data_flag", 1);
	vwconfig.overrideParameter("weak_tracks_in_tracking_callback", 1);

	_video_worker = service->createVideoWorker(
		vwconfig,
		face_sdk_parameters.method_config,
		1,   // streams_count
		1,   // processing_threads_count
		1);  // matching_threads_count

	_database = Database(
		face_sdk_parameters.database_dir,
		service->createRecognizer(face_sdk_parameters.method_config, true, false),
		service->createCapturer("common_capturer4_lbf_singleface.xml"),
		face_sdk_parameters.fa_r);

	_video_worker->setDatabase(_database.vw_elements);

	_tracking_callback_id =
		_video_worker->addTrackingCallbackU(
			TrackingCallback,
			this);

	_tracking_lost_callback_id =
		_video_worker->addTrackingLostCallbackU(
			TrackingLostCallback,
			this);

	_match_found_callback_id =
		_video_worker->addMatchFoundCallbackU(
			MatchFoundCallback,
			this);
}

Worker::~Worker()
{
	_video_worker->removeTrackingCallback(_tracking_callback_id);
	_video_worker->removeTrackingLostCallback(_tracking_lost_callback_id);
	_video_worker->removeMatchFoundCallback(_match_found_callback_id);
}

// static
void Worker::TrackingCallback(
	const pbio::VideoWorker::TrackingCallbackData &data,
	void * const userdata)
{
	// Checking arguments
	assert(userdata);

	// frame_id - frame used for drawing, samples - info about faces to be drawn
	const int frame_id = data.frame_id;
	const std::vector<pbio::RawSample::Ptr> &samples = data.samples;

	// User information - pointer to Worker
	// Pass the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	// Get the frame with frame_id
	SharedImageAndDepth frame;
	{
		const std::lock_guard<std::mutex> guard(worker._frames_mutex);

		auto& frames = worker._frames;

		// Searching in worker._frames
		for(;;)
		{
			// Frames should already be received
			assert(frames.size() > 0);

			// Check that frame_id are received in ascending order
			assert(frames.front().first <= frame_id);

			if(frames.front().first == frame_id)
			{
				// Frame is found
				frame = frames.front().second;
				frames.pop();
				break;
			}
			else
			{
				// Frame was skipped (i.e. the worker._frames.front())
				std::cout << "skipped " << ":" << frames.front().first << std::endl;
				frames.pop();
			}
		}
	}

	// Update the information
	{
		const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

		// Frame
		worker._drawing_data.frame = frame;
		worker._drawing_data.frame_id = frame_id;
		worker._drawing_data.updated = true;

		// Samples
		for(size_t i = 0; i < samples.size(); ++i)
		{
			FaceData &face = worker._drawing_data.faces[samples[i]->getID()];
			face.frame_id = samples[i]->getFrameID();  // May differ from frame_id
			face.lost = false;
			face.sample = samples[i];
			face.liveness_status = data.samples_depth_liveness_confirmed[i];
		}
	}
}

// static
void Worker::TrackingLostCallback(
	const pbio::VideoWorker::TrackingLostCallbackData &data,
	void* const userdata)
{
	const int track_id = data.track_id;

	// User information - pointer to Worker
	// Pass the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	{
		const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

		FaceData &face = worker._drawing_data.faces[track_id];

		assert(!face.lost);

		face.lost = true;
	}
}

// static
void Worker::MatchFoundCallback(
	const pbio::VideoWorker::MatchFoundCallbackData &data,
	void* const userdata)
{
	// Checking arguments
	assert(userdata);

	const pbio::RawSample::Ptr &sample = data.sample;
	const pbio::Template::Ptr &templ = data.templ;
	const std::vector<pbio::VideoWorker::SearchResult> &search_results = data.search_results;

	// User information - pointer to Worker
	// Pass the pointer
	Worker &worker = *reinterpret_cast<Worker*>(userdata);

	assert(sample);
	assert(templ);
	assert(!search_results.empty());

	for(const auto &search_result: search_results)
	{
		const uint64_t element_id = search_result.element_id;

		if(element_id == pbio::VideoWorker::MATCH_NOT_FOUND_ID)
		{
			std::cout << "Match not found" << std::endl;
		}
		else
		{
			assert(element_id < worker._database.names.size());

			std::cout << "Match found with '"
				<< worker._database.names[element_id].toStdString() << "'";
		}
	}
	std::cout << std::endl;

	const uint64_t element_id = search_results[0].element_id;

	if(element_id != pbio::VideoWorker::MATCH_NOT_FOUND_ID)
	{
		assert(element_id < worker._database.thumbnails.size());

		// Set the match info in the worker._drawing_data.faces
		// with the best search result
		const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

		FaceData &face = worker._drawing_data.faces[sample->getID()];

		assert(!face.lost);

		face.match_database_index = element_id;
	}
}

void Worker::getDataToDraw(DrawingData &data)
{
	const std::lock_guard<std::mutex> guard(_drawing_data_mutex);

	// Delete the samples, for which TrackingLostCallback was called
	{
		for(auto it = _drawing_data.faces.begin();
			it != _drawing_data.faces.end();)
		{
			const std::map<int, FaceData>::iterator i = it;
			++it; // i can be deleted, so increment it at this stage

			FaceData &face = i->second;

			// Keep the faces, which are being tracked
			if(!face.lost)
				continue;

			_drawing_data.updated = true;

			// Delete faces
			_drawing_data.faces.erase(i);
		}
	}

	data = _drawing_data;
	_drawing_data.updated = false;
}

void Worker::addFrame(const ImageAndDepth& data)
{
	const int stream_id = 0;

	SharedImageAndDepth frame;

	if (!data.depth_image.empty())
	{
		const pbio::DepthMapRaw& depth_options = data.make_dmr();

		_video_worker->addDepthFrame(
			depth_options,
			stream_id,
			data.depth_timestamp_microsec);

		_last_depth_image = std::make_shared<QByteArray>(
			(const char*)data.depth_image.data(),
			depth_options.depth_data_stride_in_bytes * depth_options.depth_map_rows);
		_last_depth_options = depth_options;

		frame.depth_image = _last_depth_image;
		frame.depth_options = depth_options;
	}

	if (!data.color_image.empty())
	{
		QImage image(
			(const uchar*)data.color_image.data(),
			data.color_width,
			data.color_height,
			data.color_stride_in_bytes,
			QImage::Format_RGB888);

		if (data.color_format != pbio::IRawImage::FORMAT_RGB)
		{
			image = image.rgbSwapped();
		}

		frame.color_image = std::make_shared<QImage>(image);

		VideoFrame video_frame;
		video_frame.frame() = frame.color_image;

		const std::lock_guard<std::mutex> guard(_frames_mutex);

		const int frame_id = _video_worker->addVideoFrame(
			video_frame,
			stream_id,
			data.image_timestamp_microsec);

		if (data.depth_image.empty())
		{
			frame.depth_image = _last_depth_image;
			frame.depth_options = _last_depth_options;
		}

		_frames.push(std::make_pair(frame_id, frame));
	}

	_video_worker->checkExceptions();
}
