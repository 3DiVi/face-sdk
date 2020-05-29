#ifndef __ExceptionCheck_h_7f6db1a3a9994e3382e6b4d5c3fd2fd7
#define __ExceptionCheck_h_7f6db1a3a9994e3382e6b4d5c3fd2fd7

//! @cond IGNORED

#include <sstream>

#include "Error.h"
#include "DllHandle.h"

namespace pbio
{

class ExceptionDestroyer
{
public:

	ExceptionDestroyer(
		void* exception,
		const pbio::import::DllHandle &dll_handle) :
	exception(exception),
	dll_handle(dll_handle)
	{

	}

	~ExceptionDestroyer()
	{
		if(exception)
			dll_handle.apiObject_destructor(exception);
	}

private:

	void* const exception;
	const pbio::import::DllHandle &dll_handle;
};

inline
void checkException(
	void* exception,
	const pbio::import::DllHandle &dll_handle)
{
	const ExceptionDestroyer ex_de(exception, dll_handle);

	if(exception)
	{
		const std::string what = dll_handle.apiException_what(exception);

		const uint32_t code = dll_handle.apiException_code(exception);

		throw pbio::Error(code, what);
	}
}

}  // pbio namespace

//! @endcond

#endif  // __ExceptionCheck_h_7f6db1a3a9994e3382e6b4d5c3fd2fd7
