#ifndef __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_ISTREAM_IMPL_H_
#define __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_ISTREAM_IMPL_H_

//! @cond IGNORED


#include <istream>

#include "pbio/stl_wraps/WrapIStream.h"

namespace pbio
{
namespace stl_wraps
{

class WrapIStreamImpl : public WrapIStream
{
public:
	WrapIStreamImpl(std::istream &s):_s(s){}

	virtual void read(char* buf, uint64_t size) override
	{
		_s.read(buf, size);
	}

private:
	std::istream &_s;
};

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_ISTREAM_IMPL_H_
