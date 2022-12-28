#ifndef __PBIO_API__PBIO__COMPLEX_OBJECT_H_
#define __PBIO_API__PBIO__COMPLEX_OBJECT_H_

//! @cond IGNORED

#include "DllHandle.h"
#include "SmartPtr.h"

namespace pbio
{

// this class is NOT virtual
class ComplexObject
{
protected:

	typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;

	ComplexObject(
		const DHPtr &dll_handle,
		void* const impl
	):
		_dll_handle(dll_handle),
		_impl(impl)
	{
		// nothing else
	}

	~ComplexObject()
	{
		if(_impl)
			_dll_handle->apiObject_destructor(_impl);
	}

	const DHPtr _dll_handle;
	void* const _impl;
	int32_t refcounter4light_shared_ptr;

//private:
//	ComplexObject(const ComplexObject&);  // deleted
//	ComplexObject& operator=(const ComplexObject&);  // deleted
};


}  // pbio namespace

//! @endcond

#endif  // __PBIO_API__PBIO__COMPLEX_OBJECT_H_
