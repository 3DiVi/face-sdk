#include <fstream>
#include <sstream>

#include "common.h"


Config::Config():
	max_person_id(0)
{
}

void Config::resize(uint64_t size)
{
	image_id2image_path.resize(size, "");
	image_id2person_id.resize(size, UNDEFINED_VALUE);
}

uint64_t Config::size() const
{
	return image_id2image_path.size();
}


time_point get_time_point()
{
	return chrono::high_resolution_clock::now();
}

uint64_t string2uint64_t(const std::string str)
{
	std::istringstream iss(str);
	uint64_t num;
	iss >> num;
	return num;
}

std::string uint64_t2string(const uint64_t num)
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

uint64_t filesize(std::string filename)
{
	std::ifstream ifs(filename.c_str(), std::ifstream::ate | std::ifstream::binary);
	TMASSERT(ifs.is_open(), "Can't open input file '" + filename + "'");
	return (uint64_t) ifs.tellg();
}

bool load_image(const std::string &path, std::vector<uint8_t> &data)
{
	data.clear();

	std::ifstream is( path.c_str(), std::ios::binary );
	if( !is.is_open() )
		return false;

	is.seekg( 0, is.end );
	size_t len = (size_t) is.tellg();
	is.seekg( 0, is.beg );
	data.resize( len );
	is.read( (char*) data.data(), len );

	return true;
}

Config read_dataset_config(
	const std::string file,
	const uint64_t begin_image_id,
	const uint64_t end_image_id)
{
	Config config;
	if( end_image_id != UNDEFINED_VALUE ) config.resize(end_image_id);

	std::ifstream ifs(file.c_str());
	TMASSERT(ifs.is_open(), "Can't open input file '" + file + "'");

	std::string str_person_id;
	std::string str_image_id;
	std::string image_path;

	// read config
	while(
		std::getline(ifs, str_person_id)
		&& std::getline(ifs, str_image_id)
		&& std::getline(ifs, image_path))
	{
		const uint64_t image_id = string2uint64_t(str_image_id);
		const uint64_t person_id = string2uint64_t(str_person_id);

		if( image_id < begin_image_id )
		{
			continue;
		}
		if( image_id >= end_image_id )
		{
			break;
		}

		if( image_id >= config.size() )
		{
			config.resize(image_id + 1);
		}
		config.image_id2image_path[image_id] = image_path;
		config.image_id2person_id[image_id] = person_id;
		if( config.max_person_id < person_id )
		{
			config.max_person_id = person_id;
		}
	}

	return config;
}
