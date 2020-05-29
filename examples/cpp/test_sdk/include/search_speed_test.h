#ifndef __TEST_SDK__SEARCH_SPEED_TEST_H__
#define __TEST_SDK__SEARCH_SPEED_TEST_H__

#include "common.h"

namespace search_speed_test {


void test(
	const std::string dll_path,
	const std::string sdk_config_dir,
	const std::string recognizer_config,
	const std::vector<std::string> templates_sources,
	const uint64_t templates_count,
	const int queries_count,
	const int query_k_nearest,
	const int search_threads_count,
	const int acceleration);


}  // namespace search_speed_test


#endif  // __TEST_SDK__SEARCH_SPEED_TEST_H__