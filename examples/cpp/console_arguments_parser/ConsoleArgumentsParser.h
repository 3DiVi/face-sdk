#ifndef console_arguments_parser_ddc3acc2bc23444cbae09cf93a3285ee
#define console_arguments_parser_ddc3acc2bc23444cbae09cf93a3285ee

#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>
#include <stdexcept>

class ConsoleArgumentsParser
{
public:
	ConsoleArgumentsParser(const int argc, char const* const argv[]);

	template<typename T>
	T get(const std::string name, const T default_value);

	template<typename T>
	T get(const std::string name);

	template<typename T>
	std::vector<T> get_all(const std::string name);

	// return all unused before arguments
	std::vector<std::string> get();

	template<typename T>
	static
	T convert(
		const std::string &option,  // only for log
		const std::string &s);

private:

	int search(std::string option);

	template<typename T>
	static
	std::string type_name();

	std::vector<std::pair<int, std::string> > args;
};

// impl


inline
ConsoleArgumentsParser::ConsoleArgumentsParser(
	const int argc,
	char const* const argv[])
{
	for(int i = 1; i < argc; ++i)
		args.push_back(std::make_pair(0, argv[i]));
}

inline
int ConsoleArgumentsParser::search(std::string option)
{
	while(!option.empty() && option.back() == ' ')
		option.pop_back();

	for(size_t i = 0; i + 1 < args.size(); ++i)
		if(args[i].first == 0 && option == args[i].second)
		{
			args[i].first = 1;
			args[i + 1].first = 2;
			return i + 1;
		}

	return -1;
}


template<typename T>
inline
T ConsoleArgumentsParser::get(const std::string name, const T default_value)
{
	const int value_id = search(name);
	if(value_id < 0)
	{
		std::cout << " warning: " << name << " option (" << type_name<T>() << ") not found,"
			" use default value: '" << default_value << "'" << std::endl;
		return default_value;
	}
	return convert<T>(name, args[value_id].second);
}

template<typename T>
inline
T ConsoleArgumentsParser::get(const std::string name)
{
	const int value_id = search(name);
	if(value_id < 0)
	{
		std::cout << "\n   error: " << name << " option (" << type_name<T>() << ") not found \n" << std::endl;
		throw std::runtime_error("args error");
	}
	return convert<T>(name, args[value_id].second);
}


template<typename T>
inline
std::vector<T> ConsoleArgumentsParser::get_all(const std::string name)
{
	std::vector<T> result;

	for(;;)
	{
		const int value_id = search(name);

		if(value_id < 0)
			break;

		result.push_back(convert<T>(name, args[value_id].second));
	}

	if(result.empty())
	{
		std::cout << " warning: " << name << " option (" << type_name<T>() << ") not found \n" << std::endl;
	}

	return result;
}

template<> inline std::string ConsoleArgumentsParser::type_name<std::string>() { return "string  "; }
template<> inline std::string ConsoleArgumentsParser::type_name<int>()         { return "int     "; }
template<> inline std::string ConsoleArgumentsParser::type_name<float>()       { return "float   "; }
template<> inline std::string ConsoleArgumentsParser::type_name<double>()      { return "double  "; }
template<> inline std::string ConsoleArgumentsParser::type_name<uint64_t>()    { return "uint64_t"; }


template<>
inline
std::string ConsoleArgumentsParser::convert<std::string>(
	const std::string &option,
	const std::string &s)
{
	std::cout << "          " << option << " option (" << type_name<std::string>() << ") value: '" << s << "'" << std::endl;
	return s;
}



template<typename T>
inline
T ConsoleArgumentsParser::convert(
	const std::string &option,
	const std::string &s)
{
	std::cout << "          " << option << " option (" << type_name<T>() << ") value: ";

	if(s.empty())
	{
		std::cout << "can not convert empty string" << std::endl;
		throw std::runtime_error("args error");
	}

	std::istringstream iss(s);
	T result = -1;
	iss >> result;

	if(iss.bad() || !iss.eof())
	{
		std::cout << "can not convert from string '" << s << "'" << std::endl;
		throw std::runtime_error("args error");
	}

	std::cout << result << std::endl;

	return result;
}


inline
std::vector<std::string> ConsoleArgumentsParser::get()
{
	std::vector<std::string> result;
	for(size_t i = 0; i < args.size(); ++i)
		if(args[i].first == 0)
		{
			args[i].first = 3;
			result.push_back(args[i].second);
		}
	return result;
}


#endif // console_arguments_parser_ddc3acc2bc23444cbae09cf93a3285ee