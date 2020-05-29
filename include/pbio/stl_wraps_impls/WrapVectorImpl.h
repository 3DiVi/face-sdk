#ifndef __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_VECTOR_IMPL_H_
#define __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_VECTOR_IMPL_H_

//! @cond IGNORED


#include <vector>

#include "pbio/stl_wraps/WrapVector.h"

namespace pbio
{
namespace stl_wraps
{

inline
void assign_pointers_vector_func(
	void* pointers_vector,
	void* const* elements,
	int32_t elements_count)
{
	((std::vector<void*>*) pointers_vector)->assign(elements, elements + elements_count);
}

inline
void assign_floats_vector_func(
	void* pointers_vector,
	float const* elements,
	int32_t elements_count)
{
	((std::vector<float>*) pointers_vector)->assign(elements, elements + elements_count);
}

}  // stl_wraps namespace
}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STL_WRAPS_IMPLS__WRAP_VECTOR_IMPL_H_





