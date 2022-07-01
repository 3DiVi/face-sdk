#ifndef PROCESSINGBLOCK_H
#define PROCESSINGBLOCK_H

//! @cond IGNORED

#include "Context.h"
#include "DllHandle.h"
#include "ExceptionCheck.h"

namespace pbio
{

class FacerecService;

class ProcessingBlock
{
public:

	ProcessingBlock(const ProcessingBlock&) = delete;
	ProcessingBlock& operator=(const ProcessingBlock&) = delete;
	ProcessingBlock(ProcessingBlock&& other) : dll_handle_(other.dll_handle_), eh_(nullptr)
	{
		handle_ = other.handle_;
		other.handle_ = nullptr;
	};
	ProcessingBlock& operator=(ProcessingBlock&& other)
	{
		if (&other != this)
		{
			handle_ = other.handle_;
			other.handle_ = nullptr;
		}
		return *this;
	}

	virtual void operator()(pbio::Context& ctx)
	{
		dll_handle_->TDVProcessingBlock_processContext(handle_, ctx.getHandle(), &eh_);
		tdvCheckException(dll_handle_, eh_);
	}

	~ProcessingBlock() {
		if(handle_)
		{
			dll_handle_->TDVProcessingBlock_destroyBlock(handle_,  &eh_);
			if (eh_ && std::uncaught_exception())
				std::cerr << Error(dll_handle_->TDVException_getErrorCode(eh_), dll_handle_->TDVException_getMessage(eh_)).what();
			else
				tdvCheckException(dll_handle_, eh_);
			handle_ = nullptr;
		}
	}

private:

	ProcessingBlock(void* service, const DHPtr &dll_handle, const pbio::Context& ctx) : dll_handle_(dll_handle), eh_(nullptr) {
		const std::string unit_type = ctx["unit_type"].getString();
		if (!unit_type.compare("HUMAN_BODY_DETECTOR"))
		{
			void* exception = nullptr;
			handle_ = dll_handle_->FacerecService_ProcessingBlock_createHumanBodyDetector(service, ctx.getHandle(), &exception);
			checkException(exception, *dll_handle_);
		}
		else if (!unit_type.compare("EMOTION_ESTIMATOR"))
		{
			void* exception = nullptr;
			handle_ = dll_handle_->FacerecService_ProcessingBlock_createEmotionEstimator(service, ctx.getHandle(), &exception);
			checkException(exception, *dll_handle_);
		}
		else if (!unit_type.compare("GENDER_ESTIMATOR"))
		{
			void* exception = nullptr;
			handle_ = dll_handle_->FacerecService_ProcessingBlock_createGenderEstimator(service, ctx.getHandle(), &exception);
			checkException(exception, *dll_handle_);
		}
		else if (!unit_type.compare("LIVENESS_ESTIMATOR"))
		{
			void* exception = nullptr;
			handle_ = dll_handle_->FacerecService_ProcessingBlock_createLivenessEstimator(service, ctx.getHandle(), &exception);
			checkException(exception, *dll_handle_);
		}
		else if (!unit_type.compare("OBJECT_DETECTOR"))
		{
			void* exception = nullptr;
			handle_ = dll_handle_->FacerecService_ProcessingBlock_createObjectDetector(service, ctx.getHandle(), &exception);
			checkException(exception, *dll_handle_);
		}
		else
		{
			throw pbio::Error(0x18ba1f8e, "Error in pbio::ProcessingBlock"
						" unknown unit type for ProcessingBlock");
		}
	}


	typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;

	const DHPtr dll_handle_;
	mutable ContextEH* eh_;
	HPBlock* handle_;

	friend class FacerecService;
};
}

#endif // PROCESSINGBLOCK_H
