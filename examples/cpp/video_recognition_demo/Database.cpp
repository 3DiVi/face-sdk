/**
	\file video_recognition_demo/src/Database.cpp
*/

#include <fstream>

#include "Database.h"
#include "MAssert.h"
#include "Worker.h"



#ifndef CV_AA
#define CV_AA cv::LINE_AA
#endif

cv::Mat Database::makeThumbnail(
	const pbio::RawSample &sample,
	const std::string name,
	const int thumbnail_size_in_)
{
	const int thumbnail_size =
		thumbnail_size_in_ > 0 ?
			thumbnail_size_in_:
			Worker::thumbnail_size;

	// buffer for the cutted image
	std::ostringstream stream;

	// make a cut in bmp format
	// so we don't waste time for encode/decode image
	// just copying it few times, which is irrelevant
	sample.cutFaceImage(
		stream,
		pbio::RawSample::IMAGE_FORMAT_BMP,
		pbio::RawSample::FACE_CUT_BASE);

	const std::string s_buffer(stream.str());

	const std::vector<uchar> buffer(s_buffer.begin(), s_buffer.end());

	cv::Mat temp = cv::imdecode(buffer, 1);

	// so we got an image

	// check it
	MAssert(!temp.empty(),);
	MAssert(temp.type() == CV_8UC3,);


	// and resize to the thumbnail_size

	cv::Rect res_rect;

	if(temp.rows >= temp.cols)
	{
		res_rect.height = thumbnail_size;
		res_rect.width = temp.cols * thumbnail_size / temp.rows;
	}
	else
	{
		res_rect.width = thumbnail_size;
		res_rect.height = temp.rows * thumbnail_size / temp.cols;
	}

	res_rect.x = (thumbnail_size - res_rect.width) / 2;
	res_rect.y = (thumbnail_size - res_rect.height) / 2;

	cv::Mat result(
		thumbnail_size,
		thumbnail_size,
		CV_8UC3,
		cv::Scalar::all(0));

	resize(
		temp,
		result(res_rect),
		res_rect.size());

	if(!name.empty())
	{
		result.rowRange(result.rows - 27, result.rows) *= 0.5f;

		cv::putText(
			result,
			name,
			cv::Point(0, result.rows - 7),
			cv::FONT_HERSHEY_DUPLEX,
			0.7,
			cv::Scalar::all(255),
			1,
			CV_AA);
	}

	return result;
}

Database::Database(
	const std::string database_list_filepath,
	pbio::Recognizer &recognizer,
	pbio::Capturer &capturer,
	const float distance_threshold)
{
	std::vector<std::string> files;

	{
		std::ifstream file(database_list_filepath);

		MAssert(file.is_open() && file.good(), "cannot open database list file '" + database_list_filepath + "'");

		while(!file.eof())
		{
			std::string s;
			std::getline(file, s);
			if(!s.empty())
				files.push_back(s);
		}
	}

	std::sort(files.begin(), files.end());

	int element_id_counter = 0;
	int person_id_counter = 0;
	std::string prev_dir;
	std::string name;

	for(size_t i = 0; i < files.size(); ++i)
	{
		const std::string filepath = files[i];

		#if defined(_WIN32)
		char const* const delims = "/\\";
		#else
		char const* const delims = "/";
		#endif

		const std::string dir = filepath.substr(0, filepath.find_last_of(delims));

		if(dir != prev_dir)
		{
			++person_id_counter;

			name.clear();

			// try to open name.txt file in this dir
			std::ifstream file(dir + "/name.txt");

			for(char c = file.get(); file.is_open() && file.good(); c = file.get())
				name += c;

			prev_dir = dir;
		}


		std::cout << "processing '" << filepath << "' name: '" << name << "'  person_id: " << person_id_counter << std::endl;

		// read image with opencv

		pbio::CVRawImage image;
		image.mat() = cv::imread(filepath);

		if(image.mat().empty() || image.mat().type() != CV_8UC3)
		{
			std::cout << "\n\nWARNING: cant read image '" << filepath << "'\n\n" << std::endl;
			continue;
		}

		// capture the face
		const std::vector<pbio::RawSample::Ptr> captured_samples = capturer.capture(image);

		if(captured_samples.size() != 1)
		{
			std::cout << "\n\nWARNING: detected " << captured_samples.size() <<
				" faces on '" << filepath << "' image instead of one, image ignored \n\n" << std::endl;
			continue;
		}

		const pbio::RawSample::Ptr sample = captured_samples[0];

		// make template
		const pbio::Template::Ptr templ = recognizer.processing(*sample);



		// prepare data for VideoWorker
		pbio::VideoWorker::DatabaseElement vw_element;
		vw_element.element_id = element_id_counter++;
		vw_element.person_id = person_id_counter;
		vw_element.face_template = templ;
		vw_element.distance_threshold = distance_threshold;

		vw_elements.push_back(vw_element);

		samples.push_back(sample);

		thumbnails.push_back(makeThumbnail(*sample, name));

		names.push_back(name);
	}

	MAssert(element_id_counter == (int) vw_elements.size(),);
	MAssert(element_id_counter == (int) samples.size(),);
	MAssert(element_id_counter == (int) thumbnails.size(),);
	MAssert(element_id_counter == (int) names.size(),);
}
