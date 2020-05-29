/**
	\file IRawImage.h
	\~English
	\brief IRawImage - Raw image interface.
	\~Russian
	\brief IRawImage - интерфейс изображения в сыром формате
*/

#ifndef __PBIO_API__PBIO__I_RAW_IMAGE_H_6091ae57d2124e47a999e03cf97f64f7
#define __PBIO_API__PBIO__I_RAW_IMAGE_H_6091ae57d2124e47a999e03cf97f64f7

#include <stdint.h>
#include "SmartPtr.h"

namespace pbio
{

/** \~English
	\brief
	Raw image interface.
	To use this interface, you need to inherit it and create your own implementation for a specific image type.
	See an example of implementation in pbio/example/CVRawImage.h.

	\~Russian
	\brief
	Интерфейс изображения в сыром формате.
	Для использования необходимо создать собственную реализацию этого интерфейса.
	Пример реализации см. в pbio/example/CVRawImage.h.
*/
class IRawImage
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to IRawImage.
		\~Russian
		\brief  Псевдоним для типа умного указателя на IRawImage.
	*/
	typedef HeavySmartPtr<IRawImage>::tPtr Ptr;

	/** \~English
		\brief Format of image data.
		\~Russian
		\brief Формат данных изображения.
	*/
	enum Format
	{
		/** \~English
			\brief Grayscale, 8 bit per pixel.
			\~Russian
			\brief Оттенки серого, 8 бит на пиксел.
		*/
		FORMAT_GRAY = 0,

		/** \~English
			\brief RGB, 24 bit per pixel, 8 bit per channel.
			\~Russian
			\brief RGB, 24 бита на пиксел, 8 бит на канал.
		*/
		FORMAT_RGB = 1,

		/** \~English
			\brief BGR, 24 bit per pixel, 8 bit per channel.
			\~Russian
			\brief BGR, 24 бита на пиксел, 8 бит на канал.
		*/
		FORMAT_BGR = 2,

		/** \~English
			\brief NV21 format in the YUV color coding system, the standard image format used on the Android camera preview.
			\~Russian
			\brief Формат NV21 в цветовой системе кодирования YUV, стандартный формат изображений,  используемых в превью Android-камер.
		*/
		FORMAT_YUV_NV21 = 3,
		
		/** \~English
			\brief NV12 format in the YUV color coding system.
			\~Russian
			\brief Формат NV12 в цветовой системе кодирования YUV.
		*/
		FORMAT_YUV_NV12 = 4
	};


	/** \~English
		\brief Virtual destructor.
		\~Russian
		\brief Виртуальный деструктор.
	*/
	virtual ~IRawImage(){}


	/**
		\~English
		\brief
			Get a pointer to the image data buffer.
			All pixels must be stored continuously row by row, without gaps at the end of each row.

		\return
			Pointer to the image data buffer.

		\~Russian
		\brief
			Получить указатель на данные изображения.
			Все пикселы должны быть сохранены последовательно, строка за строкой, без разрывов.

		\return
			Указатель на данные изображения.
	*/
	virtual const unsigned char* data() const = 0;

	/**
		\~English
		\brief
			Get image width.

		\return
			Number of columns in the image.

		\~Russian
		\brief
			Получить ширину изображения.

		\return
			Ширина изображения в пикселах.
	*/
	virtual int32_t width() const = 0;

	/**
		\~English
		\brief
			Get image height.

		\return
			Number of rows in the image.

		\~Russian
		\brief
			Получить высоту изображения.

		\return
			Высота изображения в пикселах.
	*/
	virtual int32_t height() const = 0;

	/**
		\~English
		\brief
			Get image format.

		\return
			Number of the image format (see IRawImage::Format).

		\~Russian
		\brief
			Получить формат изображения.

		\return
			Формат изображения (см. IRawImage::Format).
	*/
	virtual int32_t format() const = 0;
};


}  // pbio namespace

#endif  // __PBIO_API__PBIO__I_RAW_IMAGE_H_6091ae57d2124e47a999e03cf97f64f7
