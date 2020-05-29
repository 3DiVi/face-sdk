#ifndef ___TEST_SDK__RECOGNITION_TEST_1N_H___
#define ___TEST_SDK__RECOGNITION_TEST_1N_H___

#include <fstream>

#include "common.h"
#include "progress.h"


namespace recognition_test1N
{

void test(
	const std::string dll_path,
	const std::string sdk_config_dir,
	const std::string recognizer_config,
	const std::string dataset_config,
	const std::vector<std::string> processing_result_files,
	const std::string result_roc_file,
	const uint64_t begin_image_id,
	uint64_t end_image_id,
	const int use_cpu_cores_count,
	const std::string runtime_log_file,
	const int acceleration);

} // recognition_test1N namespace


#endif  // ___TEST_SDK__RECOGNITION_TEST_1N_H___

