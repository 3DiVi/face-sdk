#ifndef __PBIO_API__PBIO__STRUCT_STORAGE_H_3e4a45e14b2b47dba8d3c32883c6a631
#define __PBIO_API__PBIO__STRUCT_STORAGE_H_3e4a45e14b2b47dba8d3c32883c6a631

//! @cond IGNORED

#include "ComplexObject.h"
#include "StructStorageFields.h"

namespace pbio
{

class StructStorage : public ComplexObject
{
public:

	StructStorage(
		const DHPtr &dll_handle,
		void* impl);

	int64_t get_int64(const int field_id) const;

	double get_double(const int field_id) const;

	void* get_pointer(const int field_id) const;

};


// implementation:

inline
StructStorage::StructStorage(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}

inline
int64_t StructStorage::get_int64(const int field_id) const
{
	void* exception = NULL;

	int64_t const result = _dll_handle->StructStorage_get_int64(_impl, field_id, &exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
double StructStorage::get_double(const int field_id) const
{
	void* exception = NULL;

	double const result = _dll_handle->StructStorage_get_double(_impl, field_id, &exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
void* StructStorage::get_pointer(const int field_id) const
{
	void* exception = NULL;

	void* const result = _dll_handle->StructStorage_get_pointer(_impl, field_id, &exception);

	checkException(exception, *_dll_handle);

	return result;
}


} // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__STRUCT_STORAGE_H_3e4a45e14b2b47dba8d3c32883c6a631