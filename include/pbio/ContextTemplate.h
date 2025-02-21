/**
	\file ContextTemplate.h
	\~English
	\brief ContextTemplate - Interface object for saving the face template.
	\~Russian
	\brief ContextTemplate - Интерфейсный объект для сохранения шаблона лица.
*/

#ifndef __PBIO_API__PBIO__CONTEXT_TEMPLATE_H_
#define __PBIO_API__PBIO__CONTEXT_TEMPLATE_H_


#include <ostream>
#include <sstream>


#include "ComplexObject.h"
#include "Error.h"
#include "ExceptionCheck.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"



namespace pbio
{

class Recognizer;
class VideoWorker;
class TemplatesIndex;

/** \~English
	\brief Interface object for saving the face template.
	\~Russian
	\brief Интерфейсный объект для сохранения шаблона лица.
*/
class ContextTemplate
{
public:
	typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;

	/** \~English
		\brief Alias for the type of a smart pointer to ContextTemplate.
		\~Russian
		\brief Псевдоним для типа умного указателя на ContextTemplate.
	*/
	typedef LightSmartPtr<ContextTemplate>::tPtr Ptr;

	/**
		\~English
		\brief
			Get a method name.
			Thread-safe.

		\return
			The name of the method that created this template.

		\~Russian
		\brief
			Получить имя метода.
			Потокобезопасный.

		\return
			Имя метода, создавшего этот шаблон.
	*/
	std::string getMethodName() const;


	/**
		\~English
		\brief
			Save the template to binary_stream.
			The format is platform-independent.
			Thread-safe.

		\param[out]  binary_stream
			Output stream object for saving the template.
			The file stream (std::ofstream) must be opened with the set std::ios_base::binary flag.

		\~Russian
		\brief
			Сохранить шаблон.
			Формат платформонезависимый.
			Потокобезопасный.

		\param[out]  binary_stream
			Объект потока.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.
	*/
	void save(std::ostream &binary_stream) const;

	/**
		\~English
		\brief
			Save the template to binary_stream.
			The format is platform-independent.
			Thread-safe.

		\param[out]  binary_stream
			Output stream object for saving the template.
			The file stream (std::ofstream) must be opened with the set std::ios_base::binary flag.

		\~Russian
		\brief
			Сохранить шаблон.
			Формат платформонезависимый.
			Потокобезопасный.

		\param[out]  binary_stream
			Объект потока.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.
	*/
	void save(pbio::stl_wraps::WrapOStream &binary_stream) const;

	int32_t size() const;

	~ContextTemplate();

private:
	ContextTemplate(const DHPtr &dll_handle, void* impl);

	const DHPtr _dll_handle;
	void* _impl;
	int32_t refcounter4light_shared_ptr;

	friend class DynamicTemplateIndex;
	friend class Context;
	friend class FacerecService;
	friend class object_with_ref_counter<ContextTemplate>;
};

}  // pbio namespace




////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline ContextTemplate::ContextTemplate(const DHPtr &dll_handle,void* impl) :
	_dll_handle(dll_handle),
	_impl(impl)
{
	
}

inline std::string ContextTemplate::getMethodName() const
{
	std::ostringstream name_stream;
	pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

	void* exception = NULL;

	_dll_handle->ContextTemplate_getMethodName(
		_impl,
		&name_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	return name_stream.str();
}

inline void ContextTemplate::save(std::ostream &binary_stream) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	save(binary_stream_wrap);
}

inline void ContextTemplate::save(pbio::stl_wraps::WrapOStream &binary_stream) const
{
	void* exception = NULL;

	_dll_handle->ContextTemplate_save(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);
}

inline int32_t ContextTemplate::size() const
{
	void* exception = NULL;

	int32_t result = _dll_handle->ContextTemplate_size(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline ContextTemplate::~ContextTemplate()
{
	if (_impl)
    {
        _dll_handle->ContextTemplate_destructor(_impl);

		_impl = nullptr;
    }
}

}  // pbio namespace
#endif  // __PBIO_API__PBIO__TEMPLATE_H_
