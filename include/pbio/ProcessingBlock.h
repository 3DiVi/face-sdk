/**
	\file ProcessingBlock.h
	\~English
	\brief ProcessingBlock - Interface object used to work with estimators from Processing Block API.
	\~Russian
	\brief ProcessingBlock - Интерфейсный объект для взаимодействия с методами из Processing Block API.
*/

#ifndef PROCESSINGBLOCK_H
#define PROCESSINGBLOCK_H

#ifndef WITHOUT_PROCESSING_BLOCK

#include "Context.h"
#include "DllHandle.h"
#include "ExceptionCheck.h"

namespace pbio
{

class FacerecService;

/**
	\~English
	\brief Interface object used to work with estimators from Processing Block API.
	\~Russian
	\brief Интерфейсный объект для взаимодействия с методами из Processing Block API.
*/
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

	/**
		\~English
		\brief
			Calling the processing block function.
		\param[in]  сtx Context
		\~Russian
		\brief
			Вызов функции процессинг-блока.
		\param[in]  сtx Context
	*/
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
		void* exception = nullptr;
		handle_ = dll_handle_->FacerecService_ProcessingBlock_createProcessingBlock(service, ctx.getHandle(), &exception);
		checkException(exception, *dll_handle_);
	}

	typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;

	const DHPtr dll_handle_;
	mutable ContextEH* eh_;
	HPBlock* handle_;

	friend class FacerecService;
};
}

#endif // WITHOUT_PROCESSING_BLOCK
#endif // PROCESSINGBLOCK_H
