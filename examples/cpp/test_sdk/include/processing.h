#ifndef __TEST_SDK__PROCESSING_H__
#define __TEST_SDK__PROCESSING_H__

#include "common.h"
#include "progress.h"

namespace processing {


struct processing_context
{
	const Config &config;
	const std::vector<std::vector<char> > &image_id2sample;
	const std::string dataset_root_dir;
	const uint64_t begin_image_id;
	const uint64_t end_image_id;
	std::ofstream &processing_result_ofs;
	
	const uint64_t batch_size;
	uint64_t current_id;
	uint64_t done_count;
	mutex_type mutex;
	Progress progress;

	processing_context(
		const Config &config,
		const std::vector<std::vector<char> > &image_id2sample,
		const std::string dataset_root_dir,
		const uint64_t begin_image_id,
		const uint64_t end_image_id,
		std::ofstream &processing_result_ofs,
		const std::string runtime_log_file):
	config(config),
	image_id2sample(image_id2sample),
	dataset_root_dir(dataset_root_dir),
	begin_image_id(begin_image_id),
	end_image_id(end_image_id),
	processing_result_ofs(processing_result_ofs),
	batch_size(10),
	current_id(begin_image_id),
	done_count(0),
	progress(end_image_id - begin_image_id, 1, runtime_log_file)
	{
		// nothing
	}
};

class processing_worker{
public:
	processing_worker(
		processing_context &context,
		const pbio::FacerecService::Ptr facerec_service,
		const pbio::Recognizer::Ptr recognizer);
	
	void operator()();

private:
	processing_context &context;
	const pbio::FacerecService::Ptr facerec_service;
	const pbio::Recognizer::Ptr recognizer;
};

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
	const std::string runtime_log_file);


}  // namespace processing


#endif  // __TEST_SDK__PROCESSING_H__