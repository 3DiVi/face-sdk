/**
 \file test_identify.cpp
 \brief Example of creating, saving, loading and matching templates.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <facerec/import.h>
#include <facerec/libfacerec.h>

// comment the line below to use boost timer
#define _C11_TIMER_

#ifdef _C11_TIMER_
// c++11 timer
#include <chrono>
namespace chrono = std::chrono;

#else
// boost
#include <boost/chrono.hpp>
namespace chrono = boost::chrono;

#endif


typedef chrono::high_resolution_clock::time_point time_point;

time_point get_time_point()
{
	return chrono::high_resolution_clock::now();
}

double milliseconds_from(const time_point& a)
{
	return chrono::duration_cast<chrono::microseconds>(
		chrono::high_resolution_clock::now() - a).count() / (double)1000;
}


void load_file(const std::string path, std::vector<uint8_t> *data);


pbio::RawSample::Ptr capture_single_face(const std::string image_file, const pbio::Capturer::Ptr capturer);

void enrollment_mode(const pbio::Capturer::Ptr capturer, const pbio::Recognizer::Ptr recognizer,
		const std::string enroll_directory, const std::string list_file, const std::string file_to_save_templates);

void identifying_mode(const pbio::Capturer::Ptr capturer, const pbio::Recognizer::Ptr recognizer,
		const std::string image_file, const std::string file_to_load_templates);

void verify_mode(const pbio::Capturer::Ptr capturer, const pbio::Recognizer::Ptr recognizer,
		const std::string image_file1, const std::string image_file2);

void print_usage(const std::string program_name)
{
	std::cout << "\nUsage:\n" << " enrollment mode:\n " << program_name
			<< " <dll_path> <conf_dir_path> <recognizer_config> "
					"enroll <enroll_directory> <list_file> <file_to_save_templates> \n"
					" identifying mode:\n " << program_name << " <dll_path> <conf_dir_path> <recognizer_config> "
					"identify <image_file> <file_to_load_templates> \n"
					" verify mode:\n " << program_name << " <dll_path> <conf_dir_path> <recognizer_config> "
					"verify <image_file1> <image_file2> \n" << std::endl;

	std::cout << "\nExample enrollment (run from bin folder): " << program_name
			<< " ../lib/libfacerec.so ../conf/facerec method9v1000_recognizer.xml "
					"enroll set1 set1/list.txt templates.bin" << std::endl;

	std::cout << "\nExample identifying (run from bin folder): " << program_name
			<< " ../lib/libfacerec.so ../conf/facerec method9v1000_recognizer.xml "
					"identify set2/01100.jpg templates.bin" << std::endl;

	std::cout << "\nExample verify (run from bin folder): " << program_name
			<< " ../lib/libfacerec.so ../conf/facerec method9v1000_recognizer.xml "
					"verify set2/01100.jpg set2/01101.jpg" << std::endl;
}

int main(int argc, char const *argv[])
{
	try
	{
		if( argc < 5 )
		{
			print_usage( argv[0] );
			return 0;
		}

		const std::string dll_path = argv[1];
		const std::string conf_dir_path = argv[2];
		const std::string recognizer_config = argv[3];

		time_point time_c;
		const time_point init_c = get_time_point();

		time_c = get_time_point();
		// create facerec service (initialize facerec)
		const pbio::FacerecService::Ptr service = pbio::FacerecService::createService( dll_path, conf_dir_path );
		std::cout << "createService time: " << milliseconds_from( time_c ) << " ms" << std::endl;

		time_c = get_time_point();
		// create capturer
		const pbio::Capturer::Ptr capturer = service->createCapturer( "common_capturer_blf_fda_back.xml" );
		std::cout << "createCapturer time: " << milliseconds_from( time_c ) << " ms" << std::endl;

		time_c = get_time_point();
		// create recognizer
		const pbio::Recognizer::Ptr recognizer = service->createRecognizer( recognizer_config );
		std::cout << "createRecognizer time: " << milliseconds_from( time_c ) << " ms" << std::endl;

		std::cout << "\ntotal initilazition time: " << milliseconds_from( init_c ) << " ms\n" << std::endl;

		const std::string mode = argv[4];

		if( mode == "enroll" )
		{
			if( argc != 8 )
			{
				print_usage( argv[0] );
				return 0;
			}

			const std::string enroll_directory = argv[5];
			const std::string list_file = argv[6];
			const std::string file_to_save_templates = argv[7];

			time_c = get_time_point();

			enrollment_mode( capturer, recognizer, enroll_directory, list_file, file_to_save_templates );

			std::cout << "enroll time: " << milliseconds_from( time_c ) << " ms" << std::endl;
		}
		else if( mode == "identify" )
		{
			if( argc != 7 )
			{
				print_usage( argv[0] );
				return 0;
			}

			const std::string image_file = argv[5];
			const std::string file_to_load_templates = argv[6];

			time_c = get_time_point();

			identifying_mode( capturer, recognizer, image_file, file_to_load_templates );

			std::cout << "identify time: " << milliseconds_from( time_c ) << " ms" << std::endl;
		}
		else if( mode == "verify" )
		{
			if( argc != 7 )
			{
				print_usage( argv[0] );
				return 0;
			}

			const std::string image_file1 = argv[5];
			const std::string image_file2 = argv[6];

			time_c = get_time_point();

			verify_mode( capturer, recognizer, image_file1, image_file2 );

			std::cout << "verify time: " << milliseconds_from( time_c ) << " ms" << std::endl;
		}
		else
		{
			std::cout << "Wrong mode '" << mode << "'" << std::endl;
			print_usage( argv[0] );
			return 0;
		}
	} catch( const pbio::Error &e )
	{
		std::cerr << "facerec exception catched: '" << e.what() << "' code: " << std::hex << e.code() << std::endl;
	} catch( const std::exception &e )
	{
		std::cerr << "exception catched: '" << e.what() << "'" << std::endl;
	}

	return 0;
}

void load_file(const std::string path, std::vector<uint8_t> *data)
{
	if( !data )
	{
		throw std::runtime_error( "load_file: got NULL data pointer" );
	}

	data->clear();

	std::ifstream file( path.c_str(), std::ios_base::binary );

	if( !file.is_open() )
	{
		throw std::runtime_error( "load_file: file '" + path + "' not opened" );
	}

	file.seekg( 0, file.end );
	const size_t len = (size_t) file.tellg();

	file.seekg( 0, file.beg );

	data->resize( len );

	file.read( (char*) data->data(), len );

	if( !file.good() )
	{
		throw std::runtime_error( "load_file: error reading file '" + path + "'" );
	}
}

pbio::RawSample::Ptr capture_single_face(const std::string image_file, const pbio::Capturer::Ptr capturer)
{
	// load image
	std::vector<uint8_t> image_data;
	load_file( image_file, &image_data );

	const time_point time_c = get_time_point();
	// capture face sample
	const std::vector<pbio::RawSample::Ptr> samples = capturer->capture( image_data.data(), image_data.size() );
	std::cout << (std::string("Capture '") + image_file + "' time: ") << milliseconds_from( time_c ) << " ms" << std::endl;

	if( samples.size() != 1 )
	{
		throw std::runtime_error( "faces count not 1" );
	}

	return samples[0];
}

void enrollment_mode(const pbio::Capturer::Ptr capturer, const pbio::Recognizer::Ptr recognizer,
		const std::string enroll_directory, const std::string list_file, const std::string file_to_save_templates)
{
	std::ifstream list( list_file.c_str() );

	if( !list.is_open() )
	{
		throw std::runtime_error( "'" + list_file + "' file not opened" );
	}

	// load count of images
	int count = 0;
	list >> count;

	std::vector<std::string> names;
	std::vector<pbio::Template::Ptr> templates;

	for( int i = 0; i < count; ++i )
	{
		std::string name;
		list >> name;

		// capture face sample
		const pbio::RawSample::Ptr sample = capture_single_face( enroll_directory + "/" + name, capturer );


		const time_point time_c = get_time_point();
		// processing of the image
		const pbio::Template::Ptr face_template = recognizer->processing( *sample );
		std::cout << "template creation time: " << milliseconds_from( time_c ) << " ms" << std::endl;

		// -
		names.push_back( name );
		templates.push_back( face_template );
	}

	list.close();

	std::cout << "\nall processed" << std::endl;

	// now - saving in file_to_save_templates
	std::ofstream templates_file( file_to_save_templates.c_str(), std::ios_base::binary );

	if( !templates_file.is_open() )
	{
		throw std::runtime_error( "'" + file_to_save_templates + "' file not opened" );
	}

	templates_file.write( (char*) &count, sizeof( count ) );

	for( int i = 0; i < count; ++i )
	{
		const std::string &name = names[i];

		// save the name
		for( size_t j = 0; j < name.length(); ++j )
			templates_file.write( &name[j], sizeof(char) );
		templates_file.write( "\0", sizeof(char) );

		// save the template
		templates[i]->save( templates_file );
	}

	// check writing error
	if( !templates_file.good() )
	{
		throw std::runtime_error( "error writing in '" + file_to_save_templates + "' file" );
	}

	std::cout << "all saved" << std::endl;
}

void identifying_mode(const pbio::Capturer::Ptr capturer, const pbio::Recognizer::Ptr recognizer,
		const std::string image_file, const std::string file_to_load_templates)
{
	// load face sample for identification
	const pbio::RawSample::Ptr the_sample = capture_single_face( image_file, capturer );

	const time_point time_c = get_time_point();
	// processing sample
	const pbio::Template::Ptr the_template = recognizer->processing( *the_sample );
	std::cout << "template creation time: " << milliseconds_from( time_c ) << " ms\n" << std::endl;

	// load templates

	std::ifstream templates_file( file_to_load_templates.c_str(), std::ios_base::binary );

	if( !templates_file.is_open() )
	{
		throw std::runtime_error( "'" + file_to_load_templates + "' file not opened" );
	}

	int count = 0;
	templates_file.read( (char*) &count, sizeof( count ) );

	std::vector<std::string> names;
	std::vector<pbio::Template::Ptr> templates;

	for( int i = 0; i < count; ++i )
	{
		// load name
		std::string name;
		for( ;; )
		{
			char s;
			templates_file.read( &s, sizeof(char) );
			if( !s )
				break;
			name.push_back( s );
		}

		// load template
		const pbio::Template::Ptr face_template = recognizer->loadTemplate( templates_file );

		// -
		names.push_back( name );
		templates.push_back( face_template );
	}

	templates_file.close();

	// identification
	// match the_template with all others

	// vector<pair<distance, name>>
	std::vector<std::pair<double, std::string> > matches;

	for( int i = 0; i < count; ++i )
	{
		const time_point time_c = get_time_point();
		const pbio::Recognizer::MatchResult match = recognizer->verifyMatch( *the_template, *templates[i] );
		std::cout << "templates matching time: " << milliseconds_from( time_c ) << " ms" << std::endl;

		matches.push_back( std::make_pair( match.distance, names[i] ) );
	}

	std::cout << "Attention! The matching time measured very roughly and limited by timer precision,"
		" because of the matching speed more than million comparisons per seconds." << std::endl;

	// sort matches in order closest first
	std::sort( matches.begin(), matches.end() );

	// print results
	std::cout << "\nResult of identifying face from '" << image_file << "' file:\n";
	for( size_t i = 0; i < matches.size(); ++i )
	{
		const double distance = matches[i].first;
		const std::string name = matches[i].second;

		std::cout << name << "  distance: " << distance;

		std::cout << "  score: " << recognizer->getROCCurvePointByDistanceThreshold(distance).score;

		if( i == 0 )
			std::cout << " (closest)";
		std::cout << std::endl;
	}

}

void verify_mode(const pbio::Capturer::Ptr capturer, const pbio::Recognizer::Ptr recognizer,
		const std::string image_file1, const std::string image_file2)
{
	// load face sample for identification
	const pbio::RawSample::Ptr the_sample1 = capture_single_face( image_file1, capturer );

	const pbio::RawSample::Ptr the_sample2 = capture_single_face( image_file2, capturer );

	time_point time_c;

	// processing sample1
	time_c = get_time_point();
	const pbio::Template::Ptr the_template1 = recognizer->processing( *the_sample1 );
	std::cout << "\ntemplate 1 creation time: " << milliseconds_from( time_c ) << " ms" << std::endl;

	// processing sample2
	time_c = get_time_point();
	const pbio::Template::Ptr the_template2 = recognizer->processing( *the_sample2 );
	std::cout << "template 2 creation time: " << milliseconds_from( time_c ) << " ms" << std::endl;

	// identification
	// match the_template with all others

	// vector<pair<distance, name>>
	std::vector<std::pair<double, std::string> > matches;

	time_c = get_time_point();
	const pbio::Recognizer::MatchResult match = recognizer->verifyMatch( *the_template1, *the_template2 );
	std::cout << "\ntemplates matching time: " << milliseconds_from( time_c ) << " ms" << std::endl;
	std::cout << "Attention! The matching time measured very roughly and limited by timer precision,"
		" because of the matching speed more than million comparisons per seconds.\n" << std::endl;


	std::cout << "Match Result"
		<< "\n FAR    " << match.fa_r
		<< "\n FRR    " << match.fr_r
		<< "\n DIST   " << match.distance
		<< "\n SCORE  " << match.score
		<< std::endl;
}

