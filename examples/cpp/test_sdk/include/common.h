#ifndef __TEST_SDK__COMMON_H__
#define __TEST_SDK__COMMON_H__

#include <stdint.h>
#include <string>
#include <vector>

#include <facerec/libfacerec.h>

#include "TAssert.h"

// comment the line below to use boost thread and chrono
#define _TEST_SDK_USE_CXX11_

#ifdef _TEST_SDK_USE_CXX11_
// c++11
	#include <chrono>
	#include <thread>
	#include <memory>
	#include <mutex>

	namespace chrono = std::chrono;
	typedef std::thread thread;
	typedef std::mutex mutex_type;
	typedef std::lock_guard<mutex_type> lock_guard;
	#define shared_ptr std::shared_ptr

#else
// boost
	#include <boost/chrono.hpp>
	#include <boost/thread/thread.hpp>
	#include <boost/thread/mutex.hpp>

	namespace chrono = boost::chrono;
	typedef boost::thread thread;
	typedef boost::mutex mutex_type;
	typedef boost::lock_guard<mutex_type> lock_guard;
	#define shared_ptr boost::shared_ptr

#endif


const uint64_t UNDEFINED_VALUE = (uint64_t) -1;
typedef chrono::high_resolution_clock::time_point time_point;


// struct for data from dataset
struct Config
{

	std::vector<std::string> image_id2image_path;
	std::vector<uint64_t> image_id2person_id;
	uint64_t max_person_id;

	Config();

	void resize(uint64_t size);
	uint64_t size() const;
};


time_point get_time_point();

uint64_t string2uint64_t(const std::string str);
std::string uint64_t2string(const uint64_t num);

uint64_t filesize(std::string filename);

bool load_image(const std::string &path, std::vector<uint8_t> &data);

Config read_dataset_config(
	const std::string file,
	const uint64_t begin_image_id,
	const uint64_t end_image_id);


#endif  // __TEST_SDK__COMMON_H__