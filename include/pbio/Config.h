#ifndef __PBIO_API__PBIO__CONFIG_H_ea7a1512540d47be8a7663a585bcf973
#define __PBIO_API__PBIO__CONFIG_H_ea7a1512540d47be8a7663a585bcf973

#include <string>
#include <vector>
#include <utility>


namespace pbio
{

class FacerecService;

/** \~English
	\brief A class used to override the configuration parameters at runtime.
	\~Russian
	\brief Класс для переопределения параметров конфигурации во время выполнения.
*/
class Config
{
public:

	/** \~English
		\brief A constructor that takes the path to a configuration file.
		\~Russian
		\brief Конструктор, принимающий путь до конфигурационного файла.
	*/
	Config(const std::string config_filepath);

	/** \~English
		\brief A constructor that takes the path to a configuration file.
		\~Russian
		\brief Конструктор, принимающий путь до конфигурационного файла.
	*/
	Config(const char* const config_filepath);

	/**
		\~English
		\brief
			Override the parameter value.

		\param[in] parameter
			Parameter name (a tag name from the .xml config file).

		\param[in] value
			New parameter value.

		\return
			*this

		\~Russian
		\brief
			Переопределить значение параметра.

		\param[in] parameter
			Имя параметра (имя тэга из .xml конфигурационного файла).

		\param[in] value
			Новое значение параметра.

		\return
			*this
	*/
	Config& overrideParameter(
		const std::string parameter,
		const double value);

private:

	std::string config_filepath;

	std::vector<std::pair<std::string, double> > key_value;

	void prepare(
		std::vector<char const*> &keys,
		std::vector<double> &values) const;

	friend class FacerecService;
};

}  // pbio namespace




////////////////////////
/////IMPLEMENTATION/////
////////////////////////


namespace pbio
{


inline
Config::Config(
	const std::string config_filepath) :
config_filepath(config_filepath)
{
	// nothing else
}


inline
Config::Config(
	const char* const config_filepath) :
config_filepath(config_filepath)
{
	// nothing else
}

inline
Config& Config::overrideParameter(
	const std::string parameter,
	const double value)
{
	key_value.push_back(std::make_pair(parameter, value));
	return *this;
}

inline
void Config::prepare(
	std::vector<char const*> &keys,
	std::vector<double> &values) const
{
	keys.resize(key_value.size());
	values.resize(key_value.size());

	for(size_t i = 0; i < key_value.size(); ++i)
	{
		keys[i] = key_value[i].first.c_str();
		values[i] = key_value[i].second;
	}
}



}  // pbio namespace



#endif  // __PBIO_API__PBIO__CONFIG_H_ea7a1512540d47be8a7663a585bcf973
