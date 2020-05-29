#ifndef __SDK_TEST__DETECTION_H__
#define __SDK_TEST__DETECTION_H__

#include <fstream>

#include "common.h"
#include "progress.h"

namespace detection {


struct detection_context
{
	const Config &config;
	const std::string dataset_root_dir;
	const uint64_t begin_image_id;
	const uint64_t end_image_id;
	std::ofstream &detection_result_ofs;
	
	const uint64_t batch_size;
	uint64_t current_id;
	uint64_t done_count;
	mutex_type mutex;
	Progress progress;

	detection_context(
		const Config &config,
		const std::string dataset_root_dir,
		const uint64_t begin_image_id,
		const uint64_t end_image_id,
		std::ofstream &detection_result_ofs,
		const std::string runtime_log_file):
	config(config),
	dataset_root_dir(dataset_root_dir),
	begin_image_id(begin_image_id),
	end_image_id(end_image_id),
	detection_result_ofs(detection_result_ofs),
	batch_size(10),
	current_id(begin_image_id),
	done_count(0),
	progress(end_image_id - begin_image_id, (end_image_id - begin_image_id) / 1000, runtime_log_file)
	{
		// nothing
	}
};

class detection_worker{
public:
	detection_worker(
		detection_context &context,
		const pbio::Capturer::Ptr capturer);

	void operator()();

private:
	detection_context &context;
	const pbio::Capturer::Ptr capturer;
};


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
	const std::string runtime_log_file);


}  // namespace detection

#endif  // __SDK_TEST__DETECTION_H__