#ifndef __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_OSTREAM_IMPL_H_
#define __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_OSTREAM_IMPL_H_

//! @cond IGNORED


#include <ostream>

#include "pbio/stl_wraps/WrapOStream.h"

namespace pbio
{
namespace stl_wraps
{

class WrapOStreamImpl : public WrapOStream
{
public:
	WrapOStreamImpl(std::ostream &s):_s(s){}

	virtual void write(const char* buf, uint64_t size)
	{
		_s.write(buf, size);
	}

private:
	std::ostream &_s;
};

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_OSTREAM_IMPL_H_
