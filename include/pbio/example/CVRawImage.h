/**
	\file CVRawImage.h
	\~English
	\brief CVRawImage - example of implementation of the pbio::IRawImage interface via OpenCV.
	\~Russian
	\brief CVRawImage - пример реализации интерфейса pbio::IRawImage через OpenCV.
*/

#ifndef __PBIO_API__PBIO__EXAMPLE__CV_RAW_IMAGE_H_
#define __PBIO_API__PBIO__EXAMPLE__CV_RAW_IMAGE_H_

#include <opencv2/opencv.hpp>

#include "pbio/IRawImage.h"

namespace pbio
{

/**
	\~English
	\brief
		Example of implementation of the pbio::IRawImage interface via OpenCV.

	\~Russian
	\brief
		Пример реализации интерфейса pbio::IRawImage через OpenCV.
*/
class CVRawImage : public pbio::IRawImage
{
public:

	/** 	\~English
		\brief Contructor.
		\~Russian
		\brief Конструктор. */
	CVRawImage(){}

	/**  	\~English
		\brief Virtual destructor. 
		\~Russian
		\brief Виртуальный деструктор. */
	virtual ~CVRawImage(){}

	/**
		\~English
		\brief
			Get a pointer to the image data buffer.

		\return
			- NULL, if the stored cv::Mat object is invalid
			- pointer to the image data buffer, otherwise.

		\~Russian
		\brief
			Получить указатель на данные изображения.

		\return
			- NULL, если хранимое изображение некорректно
			- указатель на данные изображения, в противном случае.
	*/
	virtual const unsigned char* data() const override;


	/**
		\~English
		\brief
			Get an image width.

		\return
			Number of columns in the image.

		\~Russian
		\brief
			Получить ширину изображения.

		\return
			Ширина изображения в пикселах.
	*/
	virtual int32_t width() const override;

	/**
		\~English
		\brief
			Get an image height.

		\return
			Number of rows in the image.

		\~Russian
		\brief
			Получить высоту изображения.

		\return
			Высота изображения в пикселах.
	*/
	virtual int32_t height() const override;


	/**
		\~English
		\brief
			Get an image format.

		\return
			- -1, if the stored cv::Mat object is invalid
			- pbio::IRawImage::FORMAT_GRAY, if the mat type is CV_8UC1,
			- pbio::IRawImage::FORMAT_BGR, if the mat type is CV_8UC3.

		\~Russian
		\brief
			Получить формат изображения.

		\return
			- -1, если хранимое изображение некорректно
			- pbio::IRawImage::FORMAT_GRAY, если mat имеет формат CV_8UC1,
			- pbio::IRawImage::FORMAT_BGR, если mat имеет формат CV_8UC3.
	*/
	virtual int32_t format() const override;


	/**
		\~English
		\brief
			Get a reference to the stored cv::Mat object.

		\return
			Reference to the stored cv::Mat object.

		\~Russian
		\brief
			Получить ссылку на хранимое изображение.

		\return
			Ссылка на хранимое изображение.
	*/
	cv::Mat& mat();


	/**
		\~English
		\brief
			Get a constant reference to the stored cv::Mat object.

		\return
			Constant reference to the stored cv::Mat object.

		\~Russian
		\brief
			Получить константную ссылку на хранимое изображение.

		\return
			Константная ссылка на хранимое изображение.
	*/
	const cv::Mat& mat() const;


private:
	/** \~English
		\brief Stored cv::Mat object. 
		\~Russian
		\brief Хранимое изображение. */
	cv::Mat _mat;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////


namespace pbio
{

inline
const unsigned char* CVRawImage::data() const
{
	if( _mat.empty() ||
		_mat.dims != 2 ||
		( _mat.type() != CV_8UC1 && _mat.type() != CV_8UC3 ) ||
		!_mat.isContinuous() )
	{
		return NULL;
	}

	return _mat.data;
}

inline
int32_t CVRawImage::width() const
{
	return _mat.cols;
}


inline
int32_t CVRawImage::height() const
{
	return _mat.rows;
}


inline
int32_t CVRawImage::format() const
{
	if(_mat.type() == CV_8UC1)
	{
		return (int32_t) FORMAT_GRAY;
	}

	if(_mat.type() == CV_8UC3)
	{
		return (int32_t) FORMAT_BGR;
	}

	return -1;
}



inline
cv::Mat& CVRawImage::mat()
{
	return _mat;
}

inline
const cv::Mat& CVRawImage::mat() const
{
	return _mat;
}

}  // pbio namespace

#endif  // __PBIO_API__PBIO__EXAMPLE__CV_RAW_IMAGE_H_
