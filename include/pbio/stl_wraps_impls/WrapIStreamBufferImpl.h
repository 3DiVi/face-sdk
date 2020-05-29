#ifndef __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_ISTREAM_BUFFER_IMPL_H_
#define __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_ISTREAM_BUFFER_IMPL_H_

//! @cond IGNORED


#include <cstring>
#include <istream>

#include "pbio/stl_wraps/WrapIStream.h"

namespace pbio
{
namespace stl_wraps
{

class WrapIStreamBufferImpl : public WrapIStream
{
public:
	WrapIStreamBufferImpl(
		const char* const data,
		const int size):
		_data(data),
		_size(size),
		_pos(0)
	{
		// nothing else
	}

	virtual void read(char* buf, uint64_t size)
	{
		const uint64_t copy_size = std::min<uint64_t>(size, _size - _pos);

		if(copy_size > 0)
		{
			memcpy(buf, _data + _pos, copy_size);
			_pos += copy_size;
		}

		if(size > copy_size)
		{
			memset(buf + copy_size, 0, size - copy_size);
		}
	}

private:

	const char* const _data;
	const int _size;
	int _pos;

};

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_ISTREAM_BUFFER_IMPL_H_
