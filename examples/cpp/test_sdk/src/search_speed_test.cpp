#include <fstream>

#include "search_speed_test.h"

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
	const int acceleration)
{
	// create service and recognizer
	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		dll_path,
		sdk_config_dir);
	std::cout << "Library version: " << service->getVersion() << std::endl << std::endl;

	const pbio::Recognizer::Ptr recognizer = service->createRecognizer(recognizer_config, false, true);

	// get size of tempates files
	uint64_t common_size = 0;
	for(size_t i = 0; i < templates_sources.size(); i++)
	{
		common_size += filesize(templates_sources[i]);
	}
	
	// read templates
	std::cout << "reading templates...\n" << std::endl;
	std::vector<pbio::Template::Ptr> all_templates;
	for(size_t i = 0; i < templates_sources.size(); i++)
	{
		std::ifstream ifs_templates(templates_sources[i].c_str(), std::ios_base::binary);
		TMASSERT(ifs_templates.is_open(), "Can't open input file '" + templates_sources[i] + "'");

		const uint64_t templates_limit =
			templates_count == UNDEFINED_VALUE
				? UNDEFINED_VALUE
				: templates_count + queries_count;
		uint64_t image_id;
		while( ifs_templates.read((char*) &image_id, sizeof(image_id))
			&& all_templates.size() < templates_limit )
		{
			all_templates.push_back(recognizer->loadTemplate(ifs_templates));

			// check amount all_templates
			if(all_templates.size() == 1)
			{
				const uint64_t given_templates_count = common_size / (uint64_t) ifs_templates.tellg();
				if( templates_count != UNDEFINED_VALUE )
				{
					TMASSERT(
						given_templates_count  >= templates_count,
						"Error! Expected " + uint64_t2string(templates_count + queries_count)
						+ " templates, but actual count " + uint64_t2string(given_templates_count) );
				}
			}
		}
	}

	TMASSERT(all_templates.size() > 0, "Can't load templates");

	// queries and dataset split
	std::vector<pbio::Template::Ptr> templates;
	std::vector<pbio::Template::Ptr> queries;
	queries.assign(all_templates.begin(), all_templates.begin() + queries_count);
	templates.assign(all_templates.begin() + queries_count, all_templates.end());

	// search speed test
	std::cout << "templates count in index: " << templates.size() << std::endl;
	std::cout << "queries count: " << queries.size() << std::endl;

	// create TemplatesIndex
	const pbio::TemplatesIndex::Ptr templates_index =
		(acceleration >= 0) ?
			recognizer->createIndex(templates, search_threads_count) :
			pbio::TemplatesIndex::Ptr();

	if(acceleration >= 0)
	{
		const time_point tick1 = get_time_point();
		templates_index->reserveSearchMemory(queries_count);
		const time_point tick2 = get_time_point();

		const double time12 = chrono::duration<double>(tick2 - tick1).count();

		std::cout << " reserve search memory in " << time12 << "s" << std::endl;
	}

	const int loop_count = 3;
	for(int loop_index = 0; loop_index < loop_count; ++loop_index)
	{
		const time_point tick1 = get_time_point();

		std::vector<std::vector<pbio::Recognizer::SearchResult> > search_result;

		if(acceleration == -1)
		{
			for(int q = 0; q < queries_count; ++q)
				for(size_t j = 0; j < templates.size(); ++j)
					recognizer->verifyMatch(*queries[q], *templates[j]).distance;
		}
		else
		{
			search_result =
				recognizer->search(
					queries,
					*templates_index,
					query_k_nearest,
					acceleration ?
						pbio::Recognizer::SEARCH_ACCELERATION_1 :
						pbio::Recognizer::NO_SEARCH_ACCELERATION);
		}

		const time_point tick2 = get_time_point();

		const double time12 = chrono::duration<double>(tick2 - tick1).count();

		std::cout << " done in " << time12 << "s " <<
			"  comparisons per sec: " << ((queries_count * (double) templates.size()) / time12) <<
			"  one query: " << (time12 / queries_count) << "s " << std::endl;
	}
}


}  // namespace search_speed_test