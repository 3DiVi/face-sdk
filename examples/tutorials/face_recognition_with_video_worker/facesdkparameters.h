#ifndef FACESDKPARAMETERS_H
#define FACESDKPARAMETERS_H

#include <string>

/**
	\brief
		Processing and Face SDK settings.
*/
struct FaceSdkParameters
{
	std::string face_sdk_path = FACE_SDK_PATH;

	std::string videoworker_config = "video_worker_lbf.xml";

	std::string database_dir = DATABASE_PATH;
	const float fa_r = 1e-5;
	std::string method_config = "method6v6_recognizer.xml";
};

#endif // FACESDKPARAMETERS_H
