/**
	\file Capturer.h
	\~English
	\brief Capturer - Interface object for detecting or tracking of faces in the images or video sequences.
	\~Russian
	\brief Capturer - Интерфейсный объект для детекции и трекинга лиц на изображениях или видеопоследовательностях.
*/

#ifndef __PBIO_API__PBIO__CAPTURER_H_
#define __PBIO_API__PBIO__CAPTURER_H_

#include <sstream>
#include <vector>


#include "ComplexObject.h"
#include "Error.h"
#include "RawImage.h"
#include "RawSample.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"
#include "stl_wraps_impls/WrapVectorImpl.h"


namespace pbio
{

class FacerecService;

/**  
	\~English	
	\brief Interface object for detecting or tracking of faces in the images or video sequences. 
	\~Russian
	\brief Интерфейсный объект для детекции и трекинга лиц на изображениях или видеопоследовательностях.
*/
class Capturer : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to Capturer.
		\~Russian
		\brief Псевдоним для типа умного указателя на Capturer.
	*/
	typedef LightSmartPtr<Capturer>::tPtr Ptr;

	/** \~English
		\brief Capturer types (see also RawSample::SampleType).
		\~Russian
		\brief Типы детекторов (см. также RawSample::SampleType).
	*/
	enum CapturerType
	{
		/** \~English
			\brief Only frontal-oriented faces will be captured.
			\~Russian
			\brief Детектирует только лица в анфас.
		*/
		CAPTURER_TYPE_FRONTAL = 0,

		/** \~English
			\brief Only left-profile-oriented faces will be captured.
			\~Russian
			\brief Детектирует только лица в профиль, смотрящие влево.
		*/
		CAPTURER_TYPE_LEFT_PROFILE = 1,
		
		/** \~English
			\brief Only right-profile-oriented faces will be captured.
			\~Russian
			\brief Детектирует только лица в профиль, смотрящие вправо.
		*/
		CAPTURER_TYPE_RIGHT_PROFILE = 2
	};


	/**
		\~English
		\brief
			Capture faces in a given image or video frame.

		\param[in]  image
			Image or videoframe.

		\return
			Vector of captured face samples.

		\warning 
			Gray-scale images are not supported by the objects created with some configuration files (see \ref capturers).

		\~Russian
		\brief
			Детектировать (и отследить) лица на изображении (кадре видео).

		\param[in]  image
			Изображение или кадр видео.

		\return
			Вектор найденных лиц.

		\warning
			Черно-белые изображения не поддерживаются объектами, созданными с некоторыми конфигурационными файлами (см. \ref capturers).
	*/
	std::vector<RawSample::Ptr> capture(const RawImage image);

	/**
		\~English
		\brief
			Capture faces in a given image or video frame.

		\param[in]  data
			Pointer to the encoded image or videoframe with 8 or 24 bits per pixel.
			Supported formats are JPEG, PNG, TIF and BMP.

		\param[in]  data_size
			Data buffer size in bytes

		\return
			Vector of captured face samples.

		\warning 
			Gray-scale images are not supported by the objects created with some configuration files (see \ref capturers)

		\~Russian
		\brief
			Детектировать (и отследить) лица на изображении (кадре видео).

		\param[in]  data
			Указатель на закодированное изображение (или кадр видео) с 8 или 24 битами на пиксел.
			Поддерживаются форматы JPEG, PNG, TIF и BMP.

		\param[in]  data_size
			Размер буфера data в байтах.

		\return
			Вектор найденных лиц.

		\warning
			Черно-белые изображения не поддерживаются объектами, созданными с некоторыми конфигурационными файлами (см. \ref capturers).
	*/
	std::vector<RawSample::Ptr> capture(const unsigned char *data, int data_size);

	/**
		\~English
		\brief
			Create a sample manually with the marked eye positions.

		\param[in]  image
			Image.

		\param[in] left_eye_x
			X coordinate of the left eye.

		\param[in] left_eye_y
			Y coordinate of the left eye.

		\param[in] right_eye_x
			X coordinate of the right eye.

		\param[in] right_eye_y
			Y coordinate of the right eye.

		\return
			Captured face sample.

		\~Russian
		\brief
			Создать сэмпл вручную с отмеченными позициями глаз.

		\param[in]  image
			Изображение.

		\param[in] left_eye_x
			Координата x левого глаза.

		\param[in] left_eye_y
			Координата y левого глаза.

		\param[in] right_eye_x
			Координата x правого глаза.

		\param[in] right_eye_y
			Координата y правого глаза.

		\return
			Созданный сэмпл.
	*/
	RawSample::Ptr manualCapture(
		const RawImage image,
		float left_eye_x,
		float left_eye_y,
		float right_eye_x,
		float right_eye_y);

	/**
		\~English
		\brief
			Create a sample manually with the marked positions of all points. 
			Used only to adjust the position of points.

		\warning
			For recognition methods 7.7, 6.7, 8.7: samples that are created with
			this member function will not contain enough information about facial landmarks
			(even if given points are from the FDA points set),
			so the FDA algorithm will be <b>rerun</b> during Recognizer::processing.
			If you use this member function in order to avoid using RawSample::save and
			store the image on your own, it's recommended to use new serialization
			functions: RawSample::saveWithoutImage and FacerecService::loadRawSampleWithoutImage.

		\param[in]  image
			Image.

		\param[in] points
			Vector of points.
			It should be of the same size and the points should be in the same
			order as in the vector received from RawSample::getLandmarks
			from the original sample.

		\return
			Captured face sample.

		\~Russian
		\brief
			Создать сэмпл вручную с отмеченными позициями всех точек.
			Используется для корректировки позиций точек.

		\warning
			Для методов распознавания 7.7, 6.7, 8.7: сэмплы, созданные с помощью
			этого метода, содержат недостаточно информации о позиции лица
			(даже если поданный вектор points содержит набор точек FDA),
			поэтому FDA-алгоритм будет <b>выполнен заново</b> в ходе выполнения Recognizer::processing.
			В случае, если вы используете этот метод, чтобы избежать сохранения через RawSample::save и
			хранить изображение самостоятельно, переходите на использование методов
			RawSample::saveWithoutImage и FacerecService::loadRawSampleWithoutImage.

		\param[in]  image
			Изображение.

		\param[in] points
			Вектор точек.
			Вектор должен быть того же размера, 
			и точки должны быть в том же порядке, как и в векторе, 
			полученном от RawSample::getLandmarks от оригинального сэмпла.

		\return
			Созданный сэмпл.
	*/
	RawSample::Ptr manualCapture(
		const RawImage image,
		const std::vector<RawSample::Point> &points);

	/**
		\~English
		\brief
			Create a sample manually with the marked eye positions.

		\param[in]  data
			Pointer to the encoded image or videoframe with 8 or 24 bits per pixel.
			Supported formats are JPEG, PNG, TIF and BMP.

		\param[in]  data_size
			Data buffer size in bytes

		\param[in] left_eye_x
			X coordinate of the left eye.

		\param[in] left_eye_y
			Y coordinate of the left eye.

		\param[in] right_eye_x
			X coordinate of the right eye.

		\param[in] right_eye_y
			Y coordinate of the right eye.

		\return
			Captured face sample.

		\~Russian
		\brief
			Создать сэмпл вручную с отмеченными позициями глаз.

		\param[in]  data
			Указатель на закодированное изображение (или кадр видео) с 8 или 24 битами на пиксел.
			Поддерживаются форматы JPEG, PNG, TIF и BMP.

		\param[in]  data_size
			Размер буфера data в байтах.

		\param[in] left_eye_x
			Координата x левого глаза.

		\param[in] left_eye_y
			Координата y левого глаза.

		\param[in] right_eye_x
			Координата x правого глаза.

		\param[in] right_eye_y
			Координата y правого глаза.

		\return
			Созданный сэмпл.
	*/
	RawSample::Ptr manualCapture(
		const unsigned char *data,
		int data_size,
		float left_eye_x,
		float left_eye_y,
		float right_eye_x,
		float right_eye_y);

	/**
		\~English
		\brief
			Create a sample manually with the marked positions of all points. 
			Used only to adjust the position of points.

		\warning
			For recognition methods 7.7, 6.7, 8.7: samples that are created with
			this member function will not contain enough information about facial landmarks
			(even if given points are from the FDA points set),
			so the FDA algorithm will be <b>rerun</b> during the Recognizer::processing.
			If you use this member function in order to avoid using RawSample::save and
			store the image on your own, it's recommended to use new serialization
			functions: RawSample::saveWithoutImage and FacerecService::loadRawSampleWithoutImage.

		\param[in]  data
			Pointer to the encoded image or videoframe with 8 or 24 bits per pixel.
			Supported formats are JPEG, PNG, TIF and BMP.

		\param[in]  data_size
			Data buffer size in bytes

		\param[in] points
			Vector of points.
			It should be of the same size, and the points should be in the same
			order as in the vector received from RawSample::getLandmarks
			from the original sample.

		\return
			Captured face sample.

		\~Russian
		\brief
			Создать сэмпл вручную с отмеченными позициями всех точек.
			Используется для корректировки позиций точек.

		\warning
			Для методов распознавания 7.7, 6.7, 8.7: сэмплы, созданные с помощью
			этого метода содержат недостаточно информации о позиции лица
			(даже если поданный вектор points содержит набор точек FDA),
			поэтому FDA-алгоритм будет <b>выполнен заново</b> в ходе выполнения Recognizer::processing.
			В случае, если вы используете этот метод, чтобы избежать сохранения через RawSample::save и
			хранить изображение самостоятельно, переходите на использование методов
			RawSample::saveWithoutImage и FacerecService::loadRawSampleWithoutImage.

		\param[in]  data
			Указатель на закодированное изображение (или кадр видео) с 8 или 24 битами на пиксел.
			Поддерживаемые форматы: JPEG, PNG, TIF и BMP.

		\param[in]  data_size
			Размер буфера data в байтах.

		\param[in] points
			Вектор точек.
			Вектор должен быть того же размера, 
			и точки должны быть в том же порядке, как и в векторе, 
			полученном от RawSample::getLandmarks от оригинального сэмпла.

		\return
			Созданный сэмпл.
	*/
	RawSample::Ptr manualCapture(
		const unsigned char *data,
		int data_size,
		const std::vector<RawSample::Point> &points);

	/**
		\~English
		\brief
			Forget all previous frames and faces (only affects trackers).
		\~Russian
		\brief
			Удалить из истории все кадры и лица (только для трекеров).
	*/
	void resetHistory();


	/**
		\~English
		\brief
			Get a capturer type.

		\return
			Type of the capturer.

		\~Russian
		\brief
			Получить тип детектора.

		\return
			Тип детектора.
	*/
	CapturerType getType() const;

	/**
		\~English
		\brief
			Change the parameter value in runtime.

		\param[in] parameter
			Parameter name (tag name in the .xml config file).

		\param[in] value
			New parameter value.
		
		\~Russian
		\brief
			Изменить значение параметра в runtime.

		\param[in] parameter
			Имя параметра (имя тэга из .xml конфигурационного файла).

		\param[in] value
			Новое значение параметра.
	*/
	void setParameter(
		const std::string parameter,
		const double value);

private:

	Capturer(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<Capturer>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
Capturer::Capturer(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline 
std::vector<RawSample::Ptr> Capturer::capture(const RawImage image)
{
	std::vector<void*> void_result;

	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	_dll_handle->Capturer_capture_raw_image_with_crop(
		_impl,
		cdata.data,
		cdata.width,
		cdata.height,
		cdata.format,
		cdata.with_crop,
		cdata.crop_info_offset_x,
		cdata.crop_info_offset_y,
		cdata.crop_info_data_image_width,
		cdata.crop_info_data_image_height,
		&void_result,
		pbio::stl_wraps::assign_pointers_vector_func,
		&exception);


	checkException(exception, *_dll_handle);


	std::vector<RawSample::Ptr> result(void_result.size());

	for(size_t i = 0; i < void_result.size(); ++i)
		result[i] = RawSample::Ptr::make(_dll_handle, void_result[i]);

	return result;
}

inline
std::vector<RawSample::Ptr> Capturer::capture(const unsigned char *data, int data_size)
{
	std::vector<void*> void_result;

	void* exception = NULL;

	_dll_handle->Capturer_capture_encoded_image(
		_impl,
		data,
		data_size,
		&void_result,
		pbio::stl_wraps::assign_pointers_vector_func,
		&exception);


	checkException(exception, *_dll_handle);


	std::vector<RawSample::Ptr> result(void_result.size());

	for(size_t i = 0; i < void_result.size(); ++i)
		result[i] = RawSample::Ptr::make( _dll_handle, void_result[i] );

	return result;
}

inline
RawSample::Ptr Capturer::manualCapture(
	const RawImage image,
	float left_eye_x,
	float left_eye_y,
	float right_eye_x,
	float right_eye_y)
{
	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	void* const result_impl = _dll_handle->Capturer_manualCapture_raw_image_eyes_points_with_crop(
		_impl,
		cdata.data,
		cdata.width,
		cdata.height,
		cdata.format,

		cdata.with_crop,
		cdata.crop_info_offset_x,
		cdata.crop_info_offset_y,
		cdata.crop_info_data_image_width,
		cdata.crop_info_data_image_height,

		left_eye_x,
		left_eye_y,
		right_eye_x,
		right_eye_y,
		&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, result_impl);
}

inline
RawSample::Ptr Capturer::manualCapture(
	const RawImage image,
	const std::vector<RawSample::Point> &points)
{
	void* exception = NULL;

	std::vector<float> points_data(points.size() * 2);
	for(size_t i = 0; i < points.size(); ++i)
	{
		points_data[i * 2 + 0] = points[i].x;
		points_data[i * 2 + 1] = points[i].y;
	}

	const RawImage::CapiData cdata = image.makeCapiData();

	void* const result_impl = _dll_handle->Capturer_manualCapture_raw_image_points_vector_with_crop(
		_impl,
		cdata.data,
		cdata.width,
		cdata.height,
		cdata.format,

		cdata.with_crop,
		cdata.crop_info_offset_x,
		cdata.crop_info_offset_y,
		cdata.crop_info_data_image_width,
		cdata.crop_info_data_image_height,

		points_data.empty() ? NULL : points_data.data(),
		points.size(),
		&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, result_impl);
}

inline
RawSample::Ptr Capturer::manualCapture(
	const unsigned char *data,
	int data_size,
	float left_eye_x,
	float left_eye_y,
	float right_eye_x,
	float right_eye_y)
{
	void* exception = NULL;

	void* const result_impl = _dll_handle->Capturer_manualCapture_encoded_image_eyes_points(
		_impl,
		data,
		data_size,
		left_eye_x,
		left_eye_y,
		right_eye_x,
		right_eye_y,
		&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, result_impl);
}

inline
RawSample::Ptr Capturer::manualCapture(
	const unsigned char *data,
	int data_size,
	const std::vector<RawSample::Point> &points)
{
	void* exception = NULL;

	std::vector<float> points_data(points.size() * 2);
	for(size_t i = 0; i < points.size(); ++i)
	{
		points_data[i * 2 + 0] = points[i].x;
		points_data[i * 2 + 1] = points[i].y;
	}

	void* const result_impl = _dll_handle->Capturer_manualCapture_encoded_image_points_vector(
		_impl,
		data,
		data_size,
		points_data.empty() ? NULL : points_data.data(),
		points.size(),
		&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, result_impl);
}


inline
void Capturer::resetHistory()
{
	void* exception = NULL;

	_dll_handle->Capturer_resetHistory(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
Capturer::CapturerType Capturer::getType() const
{
	void* exception = NULL;

	const int result = _dll_handle->Capturer_getType(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return (CapturerType) result;
}

inline
void Capturer::setParameter(
	const std::string parameter,
	const double value)
{
	void* exception = NULL;

	_dll_handle->Capturer_setParameter(
		_impl,
		parameter.c_str(),
		value,
		&exception);

	checkException(exception, *_dll_handle);
}



}  // pbio namespace

#endif  // __PBIO_API__PBIO__CAPTURER_H_
