#include <fstream>

#include "recognition_test1N.h"

#include "progress.h"


namespace recognition_test1N {

inline
bool search_result_less_distance(
	const pbio::Recognizer::SearchResult &a,
	const pbio::Recognizer::SearchResult &b)
{
	return a.match_result.distance < b.match_result.distance;
}

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
	const int acceleration)
{
	TMASSERT( std::ofstream(result_roc_file.c_str()).is_open(),
		"Can't open output file '" + result_roc_file + "'" );

	// create service and recognizer
	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		dll_path,
		sdk_config_dir);
	const pbio::Recognizer::Ptr recognizer = service->createRecognizer(
		recognizer_config,
		false,  // processing flag
		true);  // matching flag

	// read dataset config
	std::cout << "reading config" << std::endl;
	const Config config = read_dataset_config(
		dataset_config,
		begin_image_id,
		end_image_id);
	end_image_id = config.size();

	// read templates

	std::cout << "reading templates" << std::endl;

	std::vector<int> persons_templates_count(config.max_person_id + 1, 0);

	std::vector<uint64_t> all_templates_person_id;

	std::vector<pbio::Template::Ptr> all_templates;

	for(size_t i = 0; i < processing_result_files.size(); i++)
	{
		std::ifstream ifs_templates(processing_result_files[i].c_str(), std::ios_base::binary);
		uint64_t image_id;
		while(ifs_templates.read((char*) &image_id, sizeof(image_id)))
		{
			const uint64_t person_id = config.image_id2person_id[image_id];

			const pbio::Template::Ptr this_template = recognizer->loadTemplate(ifs_templates);

			if( image_id >= begin_image_id &&
				image_id < end_image_id &&
				!config.image_id2image_path[image_id].empty() )
			{
				all_templates.push_back(this_template);
				all_templates_person_id.push_back(person_id);
				++persons_templates_count[person_id];
			}
		}
	}




	std::vector<pbio::Template::Ptr> query_templates;
	std::vector<uint64_t> query_person_id;

	std::vector<pbio::Template::Ptr> gallery_templates;
	std::vector<uint64_t> gallery_person_id;

	std::vector<int> persons_templates_remaining = persons_templates_count;

	int query_persons_count = 0;

	// put one template from each person to gallery, and all other - to queries
	for(size_t i = 0; i < all_templates.size(); ++i)
	{
		const uint64_t person_id = all_templates_person_id[i];

		int &remaining_in_this_person = persons_templates_remaining[person_id];

		if(remaining_in_this_person == 1)
		{
			gallery_templates.push_back(all_templates[i]);
			gallery_person_id.push_back(person_id);
		}
		else
		{
			query_templates.push_back(all_templates[i]);
			query_person_id.push_back(person_id);

			if(remaining_in_this_person == 2)
				++query_persons_count;
		}

		--remaining_in_this_person;
	}


	std::cout << "gallery_templates size: " << gallery_templates.size() << std::endl;
	std::cout << "query_templates size:   " <<   query_templates.size() << std::endl;

	TMASSERT(gallery_templates.size() > 2, "that's horrific");

	// create index
	const pbio::TemplatesIndex::Ptr index = recognizer->createIndex(gallery_templates, use_cpu_cores_count);

	// do queries

	// pair< distance, pair< +tar, +far > >
	std::vector<std::pair<double, std::pair<double, double> > > info;

	Progress progress(query_templates.size(), query_templates.size() / 1000, runtime_log_file);

	for(size_t i = 0; i < query_templates.size(); ++i)
	{
		const pbio::Template &this_template = *query_templates[i];

		const uint64_t this_person = query_person_id[i];

		const int this_person_templates_count = persons_templates_count[this_person];

		const double w = 1.0 / query_persons_count / (this_person_templates_count - 1);

		std::vector<pbio::Recognizer::SearchResult> search_result;

		if(acceleration == -1)
		{
			search_result.resize(gallery_templates.size());
			for(size_t j = 0; j < gallery_templates.size(); ++j)
			{
				search_result[j].i = j;
				search_result[j].match_result =
					recognizer->verifyMatch(
						this_template,
						*gallery_templates[j]);
			}

			std::partial_sort(
				search_result.begin(),
				search_result.begin() + 2,
				search_result.end(),
				search_result_less_distance);

			search_result.resize(2);
		}
		else
		{
			search_result =
				recognizer->search(
					this_template,
					*index,
					2,    // search for 2 closest templates
					acceleration ?
						pbio::Recognizer::SEARCH_ACCELERATION_1 :
						pbio::Recognizer::NO_SEARCH_ACCELERATION);
		}

		TASSERT(search_result.size() == 2);

		double false_accept_distance = 0;

		if( this_person == gallery_person_id[ search_result[0].i ] )
		{
			const double true_accept_distance = search_result[0].match_result.distance;

			info.push_back(
				std::make_pair(
					true_accept_distance,
					std::make_pair(
						w,     // +tar
						0)));  // +far

			false_accept_distance = search_result[1].match_result.distance;
		}
		else
		{
			false_accept_distance = search_result[0].match_result.distance;
		}


		info.push_back(
			std::make_pair(
				false_accept_distance,
				std::make_pair(
					0,     // +tar
					w)));  // +far

		progress.tick(i + 1);
	}


	// build roc

	std::sort(info.begin(), info.end());

	std::ofstream rocfile(result_roc_file.c_str());
	TASSERT(rocfile.is_open() && rocfile.good());

	double fa_rate = 0;
	double ta_rate = 0;

	for(size_t i = 0; i < info.size(); ++i)
	{
		ta_rate += info[i].second.first;
		fa_rate += info[i].second.second;

		rocfile << fa_rate << " " << ta_rate << " " << info[i].first << "\n";
	}

	rocfile.flush();
	TASSERT(rocfile.is_open() && rocfile.good());

	const double eps = 1e-8;

	TASSERT( (fa_rate > 1 - eps) && (fa_rate < 1 + eps) );  // |fa_rate - 1| < eps
	TASSERT( ta_rate < 1 + eps);
}


}  // namespace recognition_test1N