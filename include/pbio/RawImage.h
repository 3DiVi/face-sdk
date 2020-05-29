#ifndef __PBIO_API__PBIO__RAW_IMAGE_H_409ecb3ab16c416ea44ca0828ae7d624
#define __PBIO_API__PBIO__RAW_IMAGE_H_409ecb3ab16c416ea44ca0828ae7d624

#include "IRawImage.h"
#include "Rectangle.h"
#include "InternalImageBuffer.h"

namespace pbio
{

class RawSample;
class CameraCalibrator;
class Capturer;
class VideoWorker;
class FacerecService;

/** \~English
	\brief
	Struct that provides raw image data and optional cropping information.

	\~Russian
	\brief
	Структура, предоставляющая данные изображения в "сыром" формате и опциональную информацию для обрезки.
*/
struct RawImage
{
public:

	/**
		\~English
		\brief Format of image data.
		\~Russian
		\brief Формат данных изображения.
	*/
	typedef pbio::IRawImage::Format Format;

	/**
		\~English
		\brief Default constructor.
		\~Russian
		\brief Конструктор по умолчанию.
	*/
	RawImage();


	/**
		\~English
		\brief Constructor for implicit conversion from IRawImage.
		\~Russian
		\brief Конструктор для неявного преобразования из IRawImage.
	*/
	RawImage(const IRawImage&);

	/**
		\~English
		\brief Constructor for an image in InternalImageBuffer without cropping.
		\~Russian
		\brief Конструктор для изображения в InternalImageBuffer::Ptr без обрезки.
	*/
	RawImage(const InternalImageBuffer::Ptr);

	/**
		\~English
		\brief Constructor for an image without cropping.
		\~Russian
		\brief Конструктор для изображения без обрезки.
	*/
	RawImage(
		const int width,
		const int height,
		const Format format,
		unsigned char const* data);

	/**
		\~English
		\brief
			Create RawImage that represents the specified rectangle area of this image.

		\~Russian
		\brief
			Создать RawImage, представляющий указанную прямоугольную область этого изображения.
	*/
	RawImage crop(
		const Rectangle rectangle) const;

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
	unsigned char const* data;

	/**
		\~English
		\brief Image width.
		\~Russian
		\brief Ширина изображения.
	*/
	int width;

	/**
		\~English
		\brief Image height.
		\~Russian
		\brief Высота изображения.
	*/
	int height;

	/**
		\~English
		\brief Format of image data.
		\~Russian
		\brief Формат данных изображения.
	*/
	Format format;


	/**
		\~English
		\brief Flag, which indicates that croppping info is initialized. This value is set in the RawImage::crop member function.
		\~Russian
		\brief Флаг, указывающий, что информация для обрезки инициализирована. Это значение устанавливается в методе RawImage::crop.
	*/
	bool with_crop;

	/**
		\~English
		\brief Cropping offset along the x axis. This value is set in the RawImage::crop member function.
		\~Russian
		\brief Сдвиг обрезки по оси x. Это значение устанавливается в методе RawImage::crop.
	*/
	int crop_info_offset_x;

	/**
		\~English
		\brief Cropping offset along the y axis. This value is set in the RawImage::crop member function.
		\~Russian
		\brief Сдвиг обрезки по оси y. Это значение устанавливается в методе RawImage::crop.
	*/
	int crop_info_offset_y;

	/**
		\~English
		\brief Image width before RawImage::crop. This value is set in the RawImage::crop member function.
		\~Russian
		\brief Ширина изображения до обрезки. Это значение устанавливается в методе RawImage::crop.
	*/
	int crop_info_data_image_width;

	/**
		\~English
		\brief Image height before RawImage::crop. This value is set in the RawImage::crop member function.
		\~Russian
		\brief Высота изображения до обрезки. Это значение устанавливается в методе RawImage::crop.
	*/
	int crop_info_data_image_height;




private:

	struct CapiData
	{
		void const* data;
		int width;
		int height;
		int format;
		int with_crop;
		int crop_info_offset_x;
		int crop_info_offset_y;
		int crop_info_data_image_width;
		int crop_info_data_image_height;
	};

	CapiData makeCapiData() const;

	// if not empty then, data is owned by this buffer
	InternalImageBuffer::Ptr internal_image_buffer;

	friend class RawSample;
	friend class CameraCalibrator;
	friend class Capturer;
	friend class VideoWorker;
	friend class FacerecService;
};



inline
RawImage::RawImage()
: data(NULL)
, width(0)
, height(0)
, format((Format) -1)
, with_crop(false)
, crop_info_offset_x(-1)
, crop_info_offset_y(-1)
, crop_info_data_image_width(-1)
, crop_info_data_image_height(-1)
{
	// nothing else
}

inline
RawImage::RawImage(const IRawImage& a)
: data(a.data())
, width(a.width())
, height(a.height())
, format((Format) a.format())
, with_crop(false)
, crop_info_offset_x(-1)
, crop_info_offset_y(-1)
, crop_info_data_image_width(-1)
, crop_info_data_image_height(-1)
{
	// nothing else
}

inline
RawImage::RawImage(const InternalImageBuffer::Ptr a)
: data(a->data)
, width(a->width)
, height(a->height)
, format(a->format)
, with_crop(false)
, crop_info_offset_x(-1)
, crop_info_offset_y(-1)
, crop_info_data_image_width(-1)
, crop_info_data_image_height(-1)
, internal_image_buffer(a)
{
	// nothing else
}


inline
RawImage::RawImage(
	const int width,
	const int height,
	const Format format,
	unsigned char const* data)
: data(data)
, width(width)
, height(height)
, format(format)
, with_crop(false)
, crop_info_offset_x(-1)
, crop_info_offset_y(-1)
, crop_info_data_image_width(-1)
, crop_info_data_image_height(-1)
{
	// nothing else
}



inline
RawImage RawImage::crop(
	const Rectangle rectangle) const
{
	const int offset_x = with_crop ? crop_info_offset_x : 0;
	const int offset_y = with_crop ? crop_info_offset_y : 0;
	const int data_width = with_crop ? crop_info_data_image_width : width;
	const int data_height = with_crop ? crop_info_data_image_height : height;

	RawImage result = *this;

	result.width = rectangle.width;
	result.height = rectangle.height;

	result.with_crop = true;
	result.crop_info_offset_x = rectangle.x + offset_x;
	result.crop_info_offset_y = rectangle.y + offset_y;
	result.crop_info_data_image_width = data_width;
	result.crop_info_data_image_height = data_height;

	return result;
}



inline
RawImage::CapiData RawImage::makeCapiData() const
{
	CapiData result;
	result.data = data;
	result.width = width;
	result.height = height;
	result.format = format;
	result.with_crop = with_crop;
	result.crop_info_offset_x = crop_info_offset_x;
	result.crop_info_offset_y = crop_info_offset_y;
	result.crop_info_data_image_width = crop_info_data_image_width;
	result.crop_info_data_image_height = crop_info_data_image_height;

	PBI0x3dfb4fe3Assert(0x02a169c4, data, "RawImage with NULL data used");

	if(internal_image_buffer)
	{
		PBI0x3dfb4fe3Assert(0xb0be4ddd, data == internal_image_buffer->data,
			"RawImage data does not match corresponding InternalImageBuffer");

		PBI0x3dfb4fe3Assert(0x951aadf1, width == internal_image_buffer->width,
			"RawImage width does not match corresponding InternalImageBuffer");

		PBI0x3dfb4fe3Assert(0x88d7c6fc, height == internal_image_buffer->height,
			"RawImage height does not match corresponding InternalImageBuffer");

		PBI0x3dfb4fe3Assert(0xd7008c02, format == internal_image_buffer->format,
			"RawImage format does not match corresponding InternalImageBuffer");

		result.format |= InternalImageBuffer::FORMAT_FLAG__DATA_IMAGET;

		result.data = internal_image_buffer->imagetptr_ptr;
	}

	return result;
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__RAW_IMAGE_H_409ecb3ab16c416ea44ca0828ae7d624
