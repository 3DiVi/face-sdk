/**
	\file video_recognition_demo/src/MAssert.h
*/

#ifndef __VIDEO_RECOGNITION_DEMO__MASSERT_H__
#define __VIDEO_RECOGNITION_DEMO__MASSERT_H__

#include <stdexcept>

#define __LINE_STRINGIZE(x) __LINE_STRINGIZE2(x)
#define __LINE_STRINGIZE2(x) #x
#define __LINE_STRING__ __LINE_STRINGIZE(__LINE__)

#define MAssert( expr, descr ) if(!!(expr)) ; else ( \
	throw std::runtime_error("Assertion '" #expr "' failed (" \
		descr "). [" __FILE__ " : " __LINE_STRING__ "]") )


#endif  // __VIDEO_RECOGNITION_DEMO__MASSERT_H__
