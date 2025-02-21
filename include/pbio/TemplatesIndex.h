/**
	\file TemplatesIndex.h
	\~English
	\brief TemplatesIndex - Interface object for working with the template index.
	\~Russian
	\brief TemplatesIndex - Интерфейсный объект для работы с индексом шаблонов.
*/

#ifndef __PBIO_API__PBIO__TEMPLATES_INDEX_H_
#define __PBIO_API__PBIO__TEMPLATES_INDEX_H_


#include <ostream>
#include <sstream>
#include <vector>
#include <string>


#include "ComplexObject.h"
#include "Error.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"




namespace pbio
{

class Recognizer;

/** \~English
	\brief Interface object for working with the template index.
	\~Russian
	\brief Интерфейсный объект для работы с индексом шаблонов.
*/
class TemplatesIndex : public ComplexObject
{
public:

	/** \~English  
		\brief Alias for the type of a smart pointer to TemplatesIndex.
		\~Russian
		\brief Псевдоним для типа умного указателя на TemplatesIndex.
	*/
	typedef LightSmartPtr<TemplatesIndex>::tPtr Ptr;

	/**
		\~English
		\brief
			Get a method name.
			Thread-safe.

		\return
			The name of the method created this index.

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
			Get a number of templates in the index.
			Thread-safe.

		\return
			Number of templates in the index.

		\~Russian
		\brief
			Получить количество шаблонов в индексе.
			Потокобезопасный.

		\return
			Количество шаблонов в индексе.
	*/
	size_t size() const;

	/**
		\~English
		\brief
			Get an i-th template.
			Thread-safe.

		\param[in]  i
			Integer i: 0 <= i < size.

		\return
			The i-th template.

		\~Russian
		\brief
			Получить i-й шаблон.
			Потокобезопасный.

		\param[in]  i
			Целое i: 0 <= i < size.

		\return
			i-й шаблон.
	*/
	pbio::Template::Ptr at(size_t i) const;


	/**
		\~English
		\brief
			Reserve memory for temporary buffers used while searching.
			These temporary buffers require 8 * size() * queries_count bytes.

		\param[in]  queries_count
			Integer queries_count >= 0 - max size of the queries_templates vector passed
			to Recognizer::search at once to prepare the buffers for.

		\~Russian
		\brief
			Зарезервировать память для временных буферов, используемых при поиске.
			Эти временные буферы требуют 8 * size() * queries_count байт.

		\param[in]  queries_count
			Целое число queries_count >= 0 - максимальный размер вектора
                queries_templates, подаваемого в Recognizer.search за один раз, 
                к которому нужно подготовить буферы.
	*/
	void reserveSearchMemory(const int queries_count) const;

private:
	TemplatesIndex(
		const DHPtr &dll_handle,
		void* impl);

	friend class Recognizer;
	friend class object_with_ref_counter<TemplatesIndex>;
};

}  // pbio namespace




////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline
TemplatesIndex::TemplatesIndex(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}



inline
std::string TemplatesIndex::getMethodName() const
{
	std::ostringstream name_stream;
	pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

	void* exception = NULL;

	_dll_handle->TemplatesIndex_getMethodName(
		_impl,
		&name_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	return name_stream.str();
}


inline
size_t TemplatesIndex::size() const
{
	void* exception = NULL;

	const size_t size = _dll_handle->TemplatesIndex_size(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return size;
}


inline
Template::Ptr TemplatesIndex::at(size_t i) const
{
	void* exception = NULL;

	pbio::facerec::TemplateImpl* const result_impl = _dll_handle->TemplatesIndex_at(
		_impl,
		i,
		&exception);

	checkException(exception, *_dll_handle);

	return Template::Ptr::make(_dll_handle, result_impl);
}


inline
void TemplatesIndex::reserveSearchMemory(const int queries_count) const
{
	void* exception = NULL;

	_dll_handle->TemplatesIndex_reserveSearchMemory(
		_impl,
		queries_count,
		&exception);

	checkException(exception, *_dll_handle);
}
}  // pbio namespace
#endif  // __PBIO_API__PBIO__TEMPLATES_INDEX_H_
