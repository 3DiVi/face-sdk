/**
	\file Error.h
	\~English
	\brief Error - the class of exceptions thrown when errors occur.
	\~Russian
	\brief Error - класс исключений, выбрасываемых при возникновении ошибок.
*/

#ifndef __PBIO_API__PBIO__ERROR_H_
#define __PBIO_API__PBIO__ERROR_H_

#include <exception>
#include <sstream>
#include <string>

#include <stdint.h>

namespace pbio
{

/** \~English
	\brief The class of exceptions thrown when errors occur.
	\~Russian
	\brief Класс исключений, выбрасываемых при возникновении ошибок.
*/
class Error : public std::exception
{
public:

	virtual ~Error() throw()
	{
		// nothing
	}

//! @cond IGNORED

	Error(const uint32_t code, const std::string what)
	{
		_code = code;
		_what = what;
	}

//! @endcond

	/**
		\~English
		\brief
			Gets a string identifying an error.

		\return
			A null-terminated character sequence identifying an error.

		\~Russian
		\brief
			Получить строку с описанием ошибки.

		\return
			Нуль-терминированная строка с описанием ошибки.
	*/
	virtual const char* what() const throw()
	{
		return _what.c_str();
	}


	/**
		\~English
		\brief
			Gets an error code

		\return
			Unsigned integer - error code.
		
		\~Russian
		\brief
			Получить код ошибки.

		\return
			Беззнаковое целое число - код ошибки.
	*/
	uint32_t code() const throw()
	{
		return _code;
	}

private:
	uint32_t _code;
	std::string _what;
};

}  // pbio namespace



#define PBI0x3dfb4fe3Assert( code, expr, description )  \
	do  \
	{  \
		if(!(expr))  \
		{  \
			throw pbio::Error(  \
				code,  \
				"Assertion '" #expr "' failed (" + std::string(description) +  \
					"), error code: " #code ".");  \
		}  \
	} while(0)


#endif  // __PBIO_API__PBIO__ERROR_H_
