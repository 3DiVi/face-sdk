#include "detection.h"

namespace detection {


detection_worker::detection_worker(
	detection_context &context,
	const pbio::Capturer::Ptr capturer):
context(context),
capturer(capturer)
{
	// nothing
}

void detection_worker::operator()() try
{
	for(;;)
	{
		uint64_t begin_batch_image_id = 0;
		uint64_t end_batch_image_id = 0;
		std::stringstream dst_buf;

		{ // under mutex
			const lock_guard guard(context.mutex);

			begin_batch_image_id = context.current_id;
			end_batch_image_id = std::min<uint64_t>(context.current_id + context.batch_size, context.end_image_id);
			context.current_id = end_batch_image_id;
		}
		
		if( begin_batch_image_id == end_batch_image_id ) break;

		bool empty = true;

		for(uint64_t image_id = begin_batch_image_id; image_id < end_batch_image_id; image_id++)
		{
			if( context.config.image_id2image_path[image_id].empty() )
			{
				continue;
			}

			// load image file
			std::vector<uint8_t> image;

			const bool loaded =
				load_image(
					context.dataset_root_dir + "/" + context.config.image_id2image_path[image_id],
					image);

			TMASSERT(loaded, "file '" + context.dataset_root_dir + "/" + context.config.image_id2image_path[image_id] + "' not opened");

			// detection
			const std::vector<pbio::RawSample::Ptr> samples = capturer->capture(image.data(), image.size());

			if(samples.size() != 1)
			{
				{  // under mutex
					const lock_guard guard(context.mutex);
					std::cout << "Warning: on " << context.config.image_id2image_path[image_id] << " image "
						<< "detected " << samples.size() << " faces, expected one. "
						<< "Image is ignored." << std::endl;
				}
				continue;
			}

			// write
			dst_buf.write((const char*)&image_id, sizeof(image_id));

			std::stringstream sample_buf;
			samples[0]->saveWithoutImage(sample_buf);

			const uint64_t sample_size = sample_buf.tellp();
			dst_buf.write((const char*)&sample_size, sizeof(sample_size));

			dst_buf << sample_buf.rdbuf();

			empty = false;
		}

		{ // under mutex
			const lock_guard guard(context.mutex);

			if(!empty)
			{
				context.detection_result_ofs << dst_buf.rdbuf();
			}

			context.done_count += end_batch_image_id - begin_batch_image_id;
			context.progress.tick(context.done_count);
		}
	} // for(;;)
}
catch(const std::exception &e)
{
	const lock_guard guard(context.mutex);
	std::cout << "\n\nexception catched in detection worker thread: '" << e.what() << "'\n" << std::endl;
	std::cerr << "\n\nexception catched in detection worker thread: '" << e.what() << "'\n" << std::endl;
}
catch(...)
{
	const lock_guard guard(context.mutex);
	std::cout << "\n\nexception ... catched in detection worker thread\n" << std::endl;
	std::cerr << "\n\nexception ... catched in detection worker thread\n" << std::endl;
}


void detect(
	const std::string dll_path,
	const std::string sdk_config_dir,
	const std::string capturer_config,
	const std::string dataset_config,
	const std::string dataset_root_dir,
	const std::string detection_result_file,
	const uint64_t begin_image_id,
	uint64_t end_image_id,
	const int use_cpu_cores_count,
	const std::string runtime_log_file)
{
	// open output file
	std::ofstream detection_result_ofs(detection_result_file.c_str(), std::ios_base::binary);
	TMASSERT(detection_result_ofs.is_open(), "Can't open output file.");

	// create service
	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		dll_path,
		sdk_config_dir);

	std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

	// read dataset config
	const Config config = read_dataset_config(
		dataset_config,
		begin_image_id,
		end_image_id);
	end_image_id = config.size();

	std::vector<shared_ptr<detection_worker> > workers(use_cpu_cores_count);
	std::vector<shared_ptr<thread> > threads(use_cpu_cores_count);

	detection_context context(
		config,
		dataset_root_dir,
		begin_image_id,
		end_image_id,
		detection_result_ofs,
		runtime_log_file);

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		workers[i] = shared_ptr<detection_worker>( new detection_worker(
			context,
			service->createCapturer(capturer_config)) );
		threads[i] = shared_ptr<thread>( new thread(*workers[i]) );
	}

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		threads[i]->join();
	}

	detection_result_ofs.flush();
	TASSERT(detection_result_ofs.good());
}


}  // namespace detection