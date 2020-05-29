#ifndef __PBIO_API__PBIO__STL_WRAPS__WRAP_ISTREAM_H_
#define __PBIO_API__PBIO__STL_WRAPS__WRAP_ISTREAM_H_

//! @cond IGNORED

#include <stdint.h>

namespace pbio
{
namespace stl_wraps
{

// this is interface of the wrapper of std::istream
class WrapIStream
{
public:
	WrapIStream(){}

	virtual void read(char* buf, uint64_t size) = 0;

	static
	void read_func(void* stream, void* data, uint64_t bytes_count)
	{
		((WrapIStream*)stream)->read((char*) data, bytes_count);
	}
};

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS__WRAP_ISTREAM_H_
