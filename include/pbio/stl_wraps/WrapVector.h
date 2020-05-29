#ifndef __PBIO_API__PBIO__STL_WRAPS__WRAP_VECTOR_H_
#define __PBIO_API__PBIO__STL_WRAPS__WRAP_VECTOR_H_

//! @cond IGNORED


namespace pbio
{
namespace stl_wraps
{

// this is interface of the wrapper of std::vector<void*>
class WrapVector
{
public:

	typedef void* _Tp;

	WrapVector(){}

	virtual _Tp& operator[](const int i) = 0;

	virtual const _Tp& operator[](const int i) const = 0;

	virtual void resize(const int i) = 0;
};

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS__WRAP_VECTOR_H_
