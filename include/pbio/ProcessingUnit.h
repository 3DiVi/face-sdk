#ifndef __PBIO_API__PBIO__PROCESSING_UNIT_H_
#define __PBIO_API__PBIO__PROCESSING_UNIT_H_

//! @cond IGNORED

#include "ComplexObject.h"
#include "SmartPtr.h"
#include "Error.h"
#include "ExceptionCheck.h"

namespace pbio
{

class FacerecService;


class ProcessingUnit : public ComplexObject
{
public:

	enum BLOCK_TYPE
	{
		FACE_ATTRIBUTES_ESTIMATOR = 1,
	};

	typedef LightSmartPtr<ProcessingUnit>::tPtr Ptr;

	char* processSparse(char* serializedCtx) const;

	void freeStr(char* str_ptr) const;

	void* getException() const;

private:

	ProcessingUnit(
		const DHPtr &dll_handle,
		const int block_type,
		const char* serializedConfig);

	~ProcessingUnit();

	void* _block_ptr;

	friend class FacerecService;
	friend class object_with_ref_counter<ProcessingUnit>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
ProcessingUnit::ProcessingUnit(
	const DHPtr &dll_handle,
	const int block_type,
	const char* serializedConfig)
: ComplexObject(dll_handle, nullptr)
{
	if (block_type == FACE_ATTRIBUTES_ESTIMATOR)
	{
		_block_ptr = _dll_handle->TDVFaceAttributesEstimator_createByConfig(serializedConfig);
	}else
	{
		throw pbio::Error(0x10ba1f8e, "Error in pbio::ProcessingUnit"
					" unknown block_type for ProcessingUnit");
	}
	checkException(getException(), *_dll_handle);
}


inline
ProcessingUnit::~ProcessingUnit()
{
	if(_block_ptr)
	{
		_dll_handle->TDVProcessingBlock_destroy(_block_ptr);
		_block_ptr = nullptr;
	}
}


inline
char* ProcessingUnit::processSparse(char* serializedCtx) const
{
    char* result = _dll_handle->TDVProcessingBlock_processSparse(_block_ptr, serializedCtx);
    checkException(getException(), *_dll_handle);
    return result;
}


inline
void* ProcessingUnit::getException() const
{
    return _dll_handle->TDVProcessingBlock_getException(_block_ptr);
}


inline
void ProcessingUnit::freeStr(char* str_ptr) const
{
	_dll_handle->tdvFreeStr(str_ptr);
}

}  // pbio namespace

//! @endcond


#endif  // __PBIO_API__PBIO__PROCESSING_BLOCK_H_
