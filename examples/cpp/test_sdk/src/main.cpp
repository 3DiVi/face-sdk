#include <iostream>

#include <facerec/import.h>

#include "../../console_arguments_parser/ConsoleArgumentsParser.h"

#include "common.h"
#include "convert_config_format.h"
#include "detection.h"
#include "processing.h"
#include "recognition_test11.h"
#include "recognition_test1N.h"
#include "search_speed_test.h"
#include "TAssert.h"


int main(int argc, char* argv[]) try
{
	// help message
	if( argc == 1 )
	{
		const std::string help_message = 
			std::string(std::string(argv[0]) + " [--print_args FLAG] --mode MODE ...\n") +
			"modes:\n" +
			"\tdetection\n" +
			"\tprocessing\n" +
			"\trecognition_test_11\n" +
			"\trecognition_test_1N\n" +
			"\tsearch_speed_test\n" +
			"\tconvert_config_format\n" +
			"\n" +
			"detection:\n" +
			"\t--dll_path\n" +
			"\t--sdk_config_dir\n" +
			"\t--capturer_config\n" +
			"\t--dataset_config\n" +
			"\t--dataset_root_dir\n" +
			"\t--detection_result_file\n" +
			"\t[--begin_image_id]\n" +
			"\t[--end_image_id]\n" +
			"\t[--use_cpu_cores_count]\n" +
			"\t[--runtime_log_file]\n" +
			"\n" +
			"processing:\n" +
			"\t--dll_path\n" +
			"\t--sdk_config_dir\n" +
			"\t--recognizer_config\n" +
			"\t--dataset_config\n" +
			"\t--dataset_root_dir\n" +
			"\t--processing_result_file\n" +
			"\t[--begin_image_id]\n" +
			"\t[--end_image_id]\n" +
			"\t[--use_cpu_cores_count]\n" +
			"\t[--runtime_log_file]\n" +
			"\tFILES (detection result)\n" +
			"\n" +
			"recognition_test_11:\n" +
			"\t--dll_path\n" +
			"\t--sdk_config_dir\n" +
			"\t--recognizer_config\n" +
			"\t--dataset_config\n" +
			"\t--result_closest_mismatches_file\n" +
			"\t--result_roc_file\n" +
			"\t[--begin_image_id]\n" +
			"\t[--end_image_id]\n" +
			"\t[--use_cpu_cores_count]\n" +
			"\t[--runtime_log_file]\n" +
			"\tFILES (processing result)\n" +
			"\n" +
			"recognition_test_1N:\n" +
			"\t--dll_path\n" +
			"\t--sdk_config_dir\n" +
			"\t--recognizer_config\n" +
			"\t--dataset_config\n" +
			"\t--result_roc_file\n" +
			"\t[--acceleration]\n" +
			"\t[--begin_image_id]\n" +
			"\t[--end_image_id]\n" +
			"\t[--use_cpu_cores_count]\n" +
			"\t[--runtime_log_file]\n" +
			"\tFILES (processing result)\n" +
			"\n" +
			"search_speed_test:\n" +
			"\t--dll_path\n" +
			"\t--sdk_config_dir\n" +
			"\t--recognizer_config\n" +
			"\t[--templates_count]\n" +
			"\t[--queries_count]\n" +
			"\t[--query_k_nearest]\n" +
			"\t[--search_threads_count]\n" +
			"\t[--acceleration]\n" +
			"\t[--runtime_log_file]\n" +
			"\tTEMPLATES_FILES (processing result or result of utils/template_generator)\n" +
			"\n" +
			"convert_config_format:\n" +
			"\t--result_dataset_config\n" +
			"\tFILE\n"
			;

		std::cout << help_message << std::endl;
		return 1;
	}

	// create parser
	ConsoleArgumentsParser parser(argc, argv);

	// get mode
	const std::string mode = parser.get<std::string>("--mode");


#if defined(_WIN32)
	const std::string default_dll_path = "facerec.dll";
#else
	const std::string default_dll_path = "../lib/libfacerec.so";
#endif

	if( mode == "detection" )
	{
		std::cout << "\n";
		std::cout << "==============" << "\n";
		std::cout << "DETECTION MODE" << "\n";
		std::cout << "==============" << "\n" << std::endl;

		const std::string dll_path              = parser.get<std::string>("--dll_path             ", default_dll_path);
		const std::string sdk_config_dir        = parser.get<std::string>("--sdk_config_dir       ", "../conf/facerec");
		const std::string capturer_config       = parser.get<std::string>("--capturer_config      ");
		const std::string dataset_config        = parser.get<std::string>("--dataset_config       ");
		const std::string dataset_root_dir      = parser.get<std::string>("--dataset_root_dir     ");
		const std::string detection_result_file = parser.get<std::string>("--detection_result_file");
		const uint64_t    begin_image_id        = parser.get<uint64_t   >("--begin_image_id       ", 0);
		const uint64_t    end_image_id          = parser.get<uint64_t   >("--end_image_id         ", UNDEFINED_VALUE);
		const int         use_cpu_cores_count   = parser.get<int        >("--use_cpu_cores_count  ", 1);
		const std::string runtime_log_file      = parser.get<std::string>("--runtime_log_file     ", "");

		TASSERT(end_image_id > begin_image_id);
		TASSERT(use_cpu_cores_count > 0);

		detection::detect(
			dll_path,
			sdk_config_dir,
			capturer_config,
			dataset_config,
			dataset_root_dir,
			detection_result_file,
			begin_image_id,
			end_image_id,
			use_cpu_cores_count,
			runtime_log_file);

		return 0;
	}

	if( mode == "processing" )
	{
		std::cout << "\n";
		std::cout << "===============" << "\n";
		std::cout << "PROCESSING MODE" << "\n";
		std::cout << "===============" << "\n" << std::endl;

		const std::string dll_path               = parser.get<std::string>("--dll_path              ", default_dll_path);
		const std::string sdk_config_dir         = parser.get<std::string>("--sdk_config_dir        ", "../conf/facerec");
		const std::string recognizer_config      = parser.get<std::string>("--recognizer_config     ");
		const std::string dataset_config         = parser.get<std::string>("--dataset_config        ");
		const std::string dataset_root_dir       = parser.get<std::string>("--dataset_root_dir      ");
		const std::string processing_result_file = parser.get<std::string>("--processing_result_file");
		const uint64_t    begin_image_id         = parser.get<uint64_t   >("--begin_image_id        ", 0);
		const uint64_t    end_image_id           = parser.get<uint64_t   >("--end_image_id          ", UNDEFINED_VALUE);
		const int         use_cpu_cores_count    = parser.get<int        >("--use_cpu_cores_count   ", 1);
		const std::string runtime_log_file       = parser.get<std::string>("--runtime_log_file      ", "");
		
		TASSERT(end_image_id > begin_image_id);
		TASSERT(use_cpu_cores_count > 0);

		// get source config files
		const std::vector<std::string> detection_result_files = parser.get();
		TMASSERT(!detection_result_files.empty(), "Not found source config files.");

		processing::process(
			dll_path,
			sdk_config_dir,
			recognizer_config,
			dataset_config,
			dataset_root_dir,
			detection_result_files,
			processing_result_file,
			begin_image_id,
			end_image_id,
			use_cpu_cores_count,
			runtime_log_file);

		return 0;
	}

	if( mode == "recognition_test_11" )
	{
		std::cout << "\n";
		std::cout << "=========================" << "\n";
		std::cout << "1:1 RECOGNITION TEST MODE" << "\n";
		std::cout << "=========================" << "\n" << std::endl;

		const std::string dll_path                       = parser.get<std::string>("--dll_path                      ", default_dll_path);
		const std::string sdk_config_dir                 = parser.get<std::string>("--sdk_config_dir                ", "../conf/facerec");
		const std::string recognizer_config              = parser.get<std::string>("--recognizer_config             ");
		const std::string dataset_config                 = parser.get<std::string>("--dataset_config                ");
		const std::string result_closest_mismatches_file = parser.get<std::string>("--result_closest_mismatches_file");
		const std::string result_roc_file                = parser.get<std::string>("--result_roc_file               ");
		const uint64_t    begin_image_id                 = parser.get<uint64_t   >("--begin_image_id                ", 0);
		const uint64_t    end_image_id                   = parser.get<uint64_t   >("--end_image_id                  ", UNDEFINED_VALUE);
		const int         use_cpu_cores_count            = parser.get<int        >("--use_cpu_cores_count           ", 1);
		const std::string runtime_log_file               = parser.get<std::string>("--runtime_log_file              ", "");

		TASSERT(end_image_id > begin_image_id);
		TASSERT(use_cpu_cores_count > 0);

		// get source config files
		std::vector<std::string> processing_result_files = parser.get();
		TMASSERT(!processing_result_files.empty(), "Not found source templates files.");

		recognition_test11::test(
			dll_path,
			sdk_config_dir,
			recognizer_config,
			dataset_config,
			processing_result_files,
			result_roc_file,
			result_closest_mismatches_file,
			begin_image_id,
			end_image_id,
			use_cpu_cores_count,
			runtime_log_file);

		return 0;
	}

	if( mode == "recognition_test_1N" )
	{
		std::cout << "\n";
		std::cout << "=========================" << "\n";
		std::cout << "1:N RECOGNITION TEST MODE" << "\n";
		std::cout << "=========================" << "\n" << std::endl;

		const std::string dll_path            = parser.get<std::string>("--dll_path           ", default_dll_path);
		const std::string sdk_config_dir      = parser.get<std::string>("--sdk_config_dir     ", "../conf/facerec");
		const std::string recognizer_config   = parser.get<std::string>("--recognizer_config  ");
		const std::string dataset_config      = parser.get<std::string>("--dataset_config     ");
		const std::string result_roc_file     = parser.get<std::string>("--result_roc_file    ");
		const uint64_t    begin_image_id      = parser.get<uint64_t   >("--begin_image_id     ", 0);
		const uint64_t    end_image_id        = parser.get<uint64_t   >("--end_image_id       ", UNDEFINED_VALUE);
		const int         use_cpu_cores_count = parser.get<int        >("--use_cpu_cores_count", 1);
		const std::string runtime_log_file    = parser.get<std::string>("--runtime_log_file   ", "");
		const int         acceleration        = parser.get<int        >("--acceleration       ", 0);

		TASSERT(end_image_id > begin_image_id);
		TASSERT(use_cpu_cores_count > 0);

		// get source config files
		std::vector<std::string> processing_result_files = parser.get();
		TMASSERT(!processing_result_files.empty(), "Not found source templates files.");

		recognition_test1N::test(
			dll_path,
			sdk_config_dir,
			recognizer_config,
			dataset_config,
			processing_result_files,
			result_roc_file,
			begin_image_id,
			end_image_id,
			use_cpu_cores_count,
			runtime_log_file,
			acceleration);

		return 0;
	}

	if( mode == "search_speed_test" )
	{
		std::cout << "\n";
		std::cout << "======================" << "\n";
		std::cout << "SEARCH SPEED TEST MODE" << "\n";
		std::cout << "======================" << "\n" << std::endl;

		const std::string dll_path             = parser.get<std::string>("--dll_path            ", default_dll_path);
		const std::string sdk_config_dir       = parser.get<std::string>("--sdk_config_dir      ", "../conf/facerec");
		const std::string recognizer_config    = parser.get<std::string>("--recognizer_config   ");
		const uint64_t    templates_count      = parser.get<uint64_t   >("--templates_count     ", UNDEFINED_VALUE);
		const int         queries_count        = parser.get<int        >("--queries_count       ", 1);
		const int         query_k_nearest      = parser.get<int        >("--query_k_nearest     ", 1);
		const int         search_threads_count = parser.get<int        >("--search_threads_count", 1);
		const int         acceleration         = parser.get<int        >("--acceleration        ", 0);

		TASSERT(queries_count > 0);
		TASSERT(query_k_nearest > 0);
		TASSERT(search_threads_count > 0);

		// get source config files
		std::vector<std::string> templates_sources = parser.get();
		TMASSERT(!templates_sources.empty(), "Not found source templates files.");

		search_speed_test::test(
			dll_path,
			sdk_config_dir,
			recognizer_config,
			templates_sources,
			templates_count,
			queries_count,
			query_k_nearest,
			search_threads_count,
			acceleration);

		return 0;
	}

	if( mode == "convert_config_format" )
	{
		std::cout << "\n";
		std::cout << "==========================" << "\n";
		std::cout << "CONVERT CONFIG FORMAT MODE" << "\n";
		std::cout << "==========================" << "\n" << std::endl;

		const std::string result_dataset_config = parser.get<std::string>("--result_dataset_config");

		// get source config files
		std::vector<std::string> source_configs = parser.get();
		TMASSERT(source_configs.size() == 1, "Error! Expected one source config file.");

		convert_config_format::convert(
			source_configs[0],
			result_dataset_config);

		return 0;
	}

	TMASSERT(false, "Undefined mode.");

	return 1;
}
catch (const std::exception &e)
{
	std::cout << e.what() << std::endl;
	return 2;
}