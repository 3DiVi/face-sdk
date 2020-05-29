#include <fstream>

#include "convert_config_format.h"

namespace convert_config_format {


void convert(
	const std::string src_config,
	const std::string dst_config)
{
	// open output file
	std::ofstream dst_file(dst_config.c_str());
	TMASSERT(dst_file.is_open(), "Can't open output file.");

	uint64_t person_id_counter = 0;
	uint64_t image_id_counter = 0;
	std::string previous_string_person_id = "";

	// open input file
	std::ifstream src_file(src_config.c_str());
	TMASSERT(src_file.is_open(), "Can't open input file '" + src_config + "'.");

	// convertation
	std::string line;
	while(std::getline(src_file, line))
	{
		if(line.empty()) continue;

		// split line to person string id and path to image
		#ifdef _WIN32
			std::size_t sep1 = line.rfind("\\");
			std::size_t sep2 = line.rfind("/");
			std::size_t sep;

			if(sep1 == std::string::npos)
				sep = sep2;
			else if(sep2 == std::string::npos)
				sep = sep1;
			else
				sep = std::max<size_t>(sep1, sep2);
		#else
			std::size_t sep = line.rfind("/");
		#endif

		TMASSERT( sep != std::string::npos, "Slash not found in line: '" + line + "'");

		const std::string string_id = line.substr(0, sep);

		// write to new config
		if(previous_string_person_id.empty()) previous_string_person_id = string_id;

		if( string_id != previous_string_person_id )
		{
			previous_string_person_id = string_id;
			person_id_counter++;
		}

		dst_file << person_id_counter << '\n';
		dst_file << image_id_counter++ << '\n';
		dst_file << line << '\n';
	}

	dst_file.flush();
	TASSERT(dst_file.good());
}


}  // namespace convert_config_format