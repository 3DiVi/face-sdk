#include <fstream>

#include "recognition_test11.h"

namespace recognition_test11 {


TemplateComparator::TemplateComparator(
	const std::vector<pbio::Template::Ptr> &image_id2templates,
	const int threads_count,
	const pbio::FacerecService::Ptr &service,
	const std::string recognizer_config):
image_id2templates(image_id2templates)
{
	for(int i = 0; i < threads_count; i++)
	{
		recognizers.push_back(service->createRecognizer(recognizer_config, false, true));
	}
}

float TemplateComparator::operator()(
	const int thread_id,
	const uint64_t im_id1,
	const uint64_t im_id2) const

{
	return recognizers[thread_id]->verifyMatch(*image_id2templates[im_id1], *image_id2templates[im_id2]).distance;
}


void test(
	const std::string dll_path,
	const std::string sdk_config_dir,
	const std::string recognizer_config,
	const std::string dataset_config,
	const std::vector<std::string> processing_result_files,
	const std::string result_roc_file,
	const std::string result_closest_mismatches_file,
	const uint64_t begin_image_id,
	uint64_t end_image_id,
	const int use_cpu_cores_count,
	const std::string runtime_log_file)
{
	TMASSERT( std::ofstream(result_roc_file.c_str()).is_open(),
		"Can't open output file '" + result_roc_file + "'" );
	std::ofstream ofs_closest_mismatches;
	if(!result_closest_mismatches_file.empty())
	{
		ofs_closest_mismatches.open(result_closest_mismatches_file.c_str());
		TMASSERT( ofs_closest_mismatches.is_open(),
			"Can't open output file '" + result_closest_mismatches_file + "'" );
	}

	// create service and recognizer
	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		dll_path,
		sdk_config_dir);
	const pbio::Recognizer::Ptr recognizer = service->createRecognizer(recognizer_config, false, false);

	// read dataset config
	const Config config = read_dataset_config(
		dataset_config,
		begin_image_id,
		end_image_id);
	end_image_id = config.size();

	// read templates
	std::vector<pbio::Template::Ptr> image_id2templates(
		end_image_id,
		pbio::Template::Ptr());
	for(size_t i = 0; i < processing_result_files.size(); i++)
	{
		std::ifstream ifs_templates(processing_result_files[i].c_str(), std::ios_base::binary);
		uint64_t image_id;
		while(ifs_templates.read((char*) &image_id, sizeof(image_id)))
		{
			if( image_id < begin_image_id
				|| image_id >= end_image_id
				|| config.image_id2image_path[image_id].empty() )
			{
				recognizer->loadTemplate(ifs_templates);
				continue;
			}

			image_id2templates[image_id] = recognizer->loadTemplate(ifs_templates);
		}
	}

	// create persons struct for test
	std::vector<std::vector<uint64_t> > persons(
		config.max_person_id+1,
		std::vector<uint64_t>());
	for(uint64_t image_id = begin_image_id; image_id < end_image_id; image_id++)
	{
		if( image_id2templates[image_id] )
		{
			persons[config.image_id2person_id[image_id]].push_back(image_id);
		}

	}

	// create template comparator
	const TemplateComparator comparator = TemplateComparator(
		image_id2templates,
		use_cpu_cores_count,
		service,
		recognizer_config);

	std::vector<std::pair<float, std::pair<uint64_t, uint64_t> > > closest_mismatches;

	// 1:1 recognition test
	make_recognition_test11(
		recognizer->getROCCurvePointByFRR(1).distance,  // min_distance
		recognizer->getROCCurvePointByFAR(1).distance,  // max_distance
		persons,
		comparator,
		result_roc_file,
		100,  // max_closest_mismatches_count
		&closest_mismatches,
		use_cpu_cores_count,
		runtime_log_file);

	// log closest mismatches
	if (ofs_closest_mismatches && !closest_mismatches.empty()){

		for(size_t i = 0; i < closest_mismatches.size(); i++)
		{
			const float distance = closest_mismatches[i].first;
			const uint64_t image_id1 = closest_mismatches[i].second.first;
			const uint64_t image_id2 = closest_mismatches[i].second.second;

			ofs_closest_mismatches <<
				distance << " " <<
				image_id1 << " " <<
				image_id2 << " " <<
				config.image_id2image_path[image_id1] << " " <<
				config.image_id2image_path[image_id2] << '\n';
		}

		ofs_closest_mismatches.flush();
		TASSERT(ofs_closest_mismatches.good());
	}
}


}  // namespace recognition_test11