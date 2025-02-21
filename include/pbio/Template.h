/**
	\file Template.h
	\~English
	\brief Template - Interface object for saving the face template.
	\~Russian
	\brief Template - Интерфейсный объект для сохранения шаблона лица.
*/

#ifndef __PBIO_API__PBIO__TEMPLATE_H_
#define __PBIO_API__PBIO__TEMPLATE_H_


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
class Template : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to Template.
		\~Russian
		\brief Псевдоним для типа умного указателя на Template.
	*/
	typedef LightSmartPtr<Template>::tPtr Ptr;

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

private:

	Template(
		const DHPtr &dll_handle,
		void* impl);

	friend class Recognizer;
	friend class VideoWorker;
	friend class TemplatesIndex;
	friend class DynamicTemplateIndex;
	friend class object_with_ref_counter<Template>;
};

}  // pbio namespace




////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline
Template::Template(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
std::string Template::getMethodName() const
{
	std::ostringstream name_stream;
	pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

	void* exception = NULL;

	_dll_handle->Template_getMethodName(
		_impl,
		&name_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	return name_stream.str();
}

inline
void Template::save(std::ostream &binary_stream) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	save(binary_stream_wrap);
}

inline
void Template::save(pbio::stl_wraps::WrapOStream &binary_stream) const
{
	void* exception = NULL;

	_dll_handle->Template_save(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);
}



}  // pbio namespace
#endif  // __PBIO_API__PBIO__TEMPLATE_H_
