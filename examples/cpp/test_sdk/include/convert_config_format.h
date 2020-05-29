#ifndef __TEST_SDK__CONVERT_CONFIG_FORMAT_H__
#define __TEST_SDK__CONVERT_CONFIG_FORMAT_H__

#include "common.h"

namespace convert_config_format {


//	convert config with format:
//		person_dir_1/file1.jpg
//		person_dir_1/file2.jpg
//		...
//		person_dir_n/file15.jpg
//		person_dir_n/file16.jpg
//	to:
//		person_id1
//		image_id1
//		image_path1
//		person_id1
//		image_id2
//		image_path2
//		person_id2
//		image_id3
//		image_path3
//		...
void convert(
	const std::string src_config,
	const std::string dst_config);


}  // namespace convert_config_format


#endif  // __TEST_SDK__CONVERT_CONFIG_FORMAT_H__