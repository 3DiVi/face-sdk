#ifndef __PBIO_API__PBIO__InternalImageBuffer_H_95c37b7db267489390b825c3535f4e88
#define __PBIO_API__PBIO__InternalImageBuffer_H_95c37b7db267489390b825c3535f4e88

#include "SmartPtr.h"
#include "IRawImage.h"

namespace pbio
{

class RawImage;

/** \~English
	\brief
	Interface object that stores image data.
	Always create new InternalImageBuffer for every image or video frame.
	Never change image data of InternalImageBuffer after first use.

	\~Russian
	\brief
	Интерфейсный объект, хранящий данные изображения.
	Всегда создавайте новый InternalImageBuffer для каждого изображения или кадра видео.
	Никогда не изменяйте данные изображения после первого использования.
*/
class InternalImageBuffer : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to InternalImageBuffer.
		\~Russian
		\brief Псевдоним для типа умного указателя на InternalImageBuffer.
	*/
	typedef LightSmartPtr<InternalImageBuffer>::tPtr Ptr;

	/**
		\~English
		\brief Format of image data.
		\~Russian
		\brief Формат данных изображения.
	*/
	typedef pbio::IRawImage::Format Format;

	/**
		\~English
		\brief
			Pointer to the image data buffer.
			All pixels must be stored continuously, row by row, without gaps at the end of each row.

		\~Russian
		\brief
			Указатель на данные изображения.
			Все пикселы должны быть сохранены последовательно, строка за строкой, без разрывов.
	*/
	unsigned char* const data;

	/**
		\~English
		\brief
			Size of the data buffer.

		\~Russian
		\brief
			Размера буфера data.
	*/
	const size_t data_buffer_size;

	/**
		\~English
		\brief Image width.
		\~Russian
		\brief Ширина изображения.
	*/
	const int width;

	/**
		\~English
		\brief Image height.
		\~Russian
		\brief Высота изображения.
	*/
	const int height;

	/**
		\~English
		\brief Format of image data.
		\~Russian
		\brief Формат данных изображения.
	*/
	const Format format;


private:

	static const int FORMAT_FLAG__DATA_IMAGET = 1 << 15;

	void const* const imagetptr_ptr;


	InternalImageBuffer(
		const DHPtr &dll_handle,
		void* impl,
		unsigned char* data,
		size_t data_buffer_size,
		int width,
		int height,
		Format format,
		void const* imagetptr_ptr)
	: ComplexObject(dll_handle, impl)
	, data(data)
	, data_buffer_size(data_buffer_size)
	, width(width)
	, height(height)
	, format(format)
	, imagetptr_ptr(imagetptr_ptr)
	{
		// nothing else
	}


	friend class object_with_ref_counter<InternalImageBuffer>;
	friend class RawImage;
};

}  // pbio namespace

#endif  // __PBIO_API__PBIO__InternalImageBuffer_H_95c37b7db267489390b825c3535f4e88