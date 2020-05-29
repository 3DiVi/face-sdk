#ifndef __PBIO_API__PBIO__STL_WRAPS__WRAP_OSTREAM_H_
#define __PBIO_API__PBIO__STL_WRAPS__WRAP_OSTREAM_H_

//! @cond IGNORED

#include <stdint.h>

namespace pbio
{
namespace stl_wraps
{

// this is interface of the wrapper of std::ostream
class WrapOStream
{
public:
	WrapOStream(){}

	virtual void write(const char* buf, uint64_t size) = 0;

	static
	void write_func(void* stream, const void* data, uint64_t bytes_count)
	{
		((WrapOStream*)stream)->write((const char*) data, bytes_count);
	}
};

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS__WRAP_OSTREAM_H_


