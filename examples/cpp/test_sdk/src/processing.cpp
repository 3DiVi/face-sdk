#include <fstream>
#include <limits>

#include "processing.h"

namespace processing {


processing_worker::processing_worker(
	processing_context &context,
	const pbio::FacerecService::Ptr facerec_service,
	const pbio::Recognizer::Ptr recognizer):
context(context),
facerec_service(facerec_service),
recognizer(recognizer)
{
	// nothing
}

void processing_worker::operator()() try
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
			const std::vector<char> &sample_vector = context.image_id2sample[image_id];

			if(sample_vector.empty())
			{
				continue;
			}

			TASSERT(!context.config.image_id2image_path[image_id].empty());

			pbio::stl_wraps::WrapIStreamBufferImpl stream_from_sample_vector(sample_vector.data(), sample_vector.size());

			std::vector<uint8_t> image;
			load_image(context.dataset_root_dir + "/" + context.config.image_id2image_path[image_id], image);

			const pbio::RawSample::Ptr sample = facerec_service->loadRawSampleWithoutImage(
				stream_from_sample_vector,
				image.data(),
				image.size());

			const pbio::Template::Ptr templ = recognizer->processing(*sample);

			dst_buf.write((char const*) &image_id, sizeof(image_id));
			templ->save(dst_buf);
			empty = false;
		}

		{ // under mutex
			const lock_guard guard(context.mutex);

			if(!empty)
			{
				context.processing_result_ofs << dst_buf.rdbuf();
			}
			
			context.done_count += end_batch_image_id - begin_batch_image_id;
			context.progress.tick(context.done_count);
		}
	} // for(;;)
}
catch(const std::exception &e)
{
	const lock_guard guard(context.mutex);
	std::cout << "\n\nexception catched in processing worker thread: '" << e.what() << "'\n" << std::endl;
	std::cerr << "\n\nexception catched in processing worker thread: '" << e.what() << "'\n" << std::endl;
}
catch(...)
{
	const lock_guard guard(context.mutex);
	std::cout << "\n\nexception ... catched in processing worker thread\n" << std::endl;
	std::cerr << "\n\nexception ... catched in processing worker thread\n" << std::endl;
}


void process(
	const std::string dll_path,
	const std::string sdk_config_dir,
	const std::string recognizer_config,
	const std::string dataset_config,
	const std::string dataset_root_dir,
	const std::vector<std::string> detection_result_files,
	const std::string processing_result_file,
	const uint64_t begin_image_id,
	uint64_t end_image_id,
	const int use_cpu_cores_count,
	const std::string runtime_log_file)
{
	// create service
	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		dll_path,
		sdk_config_dir);

	std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

	// read dataset config;
	const Config config = read_dataset_config(
		dataset_config,
		begin_image_id,
		end_image_id);
	end_image_id = config.size();

	std::ofstream ofs_templates(
		processing_result_file.c_str(),
		std::ios::binary);
	TASSERT(ofs_templates.is_open());

	// detection result
	std::vector<std::vector<char> > image_id2sample(end_image_id);
	for(size_t i = 0; i < detection_result_files.size(); i++)
	{
		// open input file
		std::ifstream src_file(detection_result_files[i].c_str(), std::ios_base::binary);
		TMASSERT(src_file.is_open(), "Can't open input file '" + detection_result_files[i] + "'.");

		while(!src_file.eof())
		{
			uint64_t image_id;

			src_file.read((char*)&image_id, sizeof(image_id));

			uint64_t sample_size;

			src_file.read((char*)&sample_size, sizeof(sample_size));


			if( (image_id < begin_image_id)
				|| (image_id >= end_image_id)
				|| config.image_id2image_path[image_id].empty() )
			{
				src_file.seekg(uint64_t(src_file.tellg()) + sample_size);
				continue;
			}
			else
			{
				image_id2sample[image_id].resize(sample_size);
				src_file.read(image_id2sample[image_id].data(), sample_size);
			}
		}
	}

	std::vector<shared_ptr<processing_worker> > workers(use_cpu_cores_count);
	std::vector<shared_ptr<thread> > threads(use_cpu_cores_count);
	
	processing_context context(
		config,
		image_id2sample,
		dataset_root_dir,
		begin_image_id,
		end_image_id,
		ofs_templates,
		runtime_log_file);

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		workers[i] = shared_ptr<processing_worker>(
			new processing_worker(
				context,
				service,
				service->createRecognizer(recognizer_config, true, false)) );
	}

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		threads[i] = shared_ptr<thread>( new thread(*workers[i]) );
	}

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		threads[i]->join();
	}

	ofs_templates.flush();
	TASSERT(ofs_templates.good());
}


}  // namespace processing
