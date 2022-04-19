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
	ProcessingBlock(ProcessingBlock&& other) : dll_handle_(other.dll_handle_)
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

	ProcessingBlock(void* service, const DHPtr &dll_handle, const pbio::Context& ctx) : dll_handle_(dll_handle) {
		const std::string unit_type = ctx["unit_type"].getString();
		if (!unit_type.compare("HUMAN_BODY_DETECTOR"))
		{
			void* exception = nullptr;
			handle_ = dll_handle_->TDVProcessingBlock_createHumanBodyDetector(service, ctx.getHandle(), &exception);
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
	mutable ContextEH* eh_ = nullptr;
	HPBlock* handle_;

	friend class FacerecService;
};
}

#endif // PROCESSINGBLOCK_H
