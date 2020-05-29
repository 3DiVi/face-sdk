/**
	\file CameraCalibrator.h
	\~English
	\brief CameraCalibrator - Interface object for camera calibration and correction of image distortion.
	\~Russian
	\brief CameraCalibrator - Интерфейсный объект для калибровки камеры и коррекции дисторсии.
*/

#ifndef __PBIO_API__PBIO__CAMERACALIBRATOR_H_
#define __PBIO_API__PBIO__CAMERACALIBRATOR_H_

#include <sstream>
#include <vector>


#include "ComplexObject.h"
#include "Error.h"
#include "ExceptionCheck.h"
#include "Image.h"
#include "Point.h"
#include "RawImage.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapIStreamImpl.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"




namespace pbio
{

class FacerecService;

/** \~English
	\brief Interface object for camera calibration and correction of image distortion.
	\~Russian
	\brief Интерфейсный объект для калибровки камеры и коррекции дисторсии.
*/
class CameraCalibrator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to CameraCalibrator. 
		\~Russian
		\brief Псевдоним для типа умного указателя на CameraCalibrator.
	*/
	typedef LightSmartPtr<CameraCalibrator>::tPtr Ptr;


	/** \~English
		\brief Сalibration process settings. 
		\~Russian
		\brief Настройки калибровки.
	*/
	struct CalibrationSettings
	{
		/** \~English
			\brief Types of calibration patterns. 
			\~Russian
			\brief Типы калибровочных шаблонов.
		*/
		enum PatternType
		{
			PATTERN_CHESSBOARD = 0,               /**< Chessboard pattern. */
			PATTERN_CIRCLES_GRID = 1,             /**< Circles grid pattern. */
			PATTERN_ASYMMETRIC_CIRCLES_GRID = 2,  /**< Asymmetric circles grid. */
		};

		/** \~English
			\brief Width of images for calibration.
			\~Russian
			\brief Ширина изображения для калибровки.
		*/
		int image_width;

		/** \~English
			\brief Height of images for calibration.
			\~Russian
			\brief Высота изображения для калибровки.
		*/
		int image_height;

		/** \~English
			\brief Pattern type.
			\~Russian
			\brief Тип калибровочного шаблона.
		*/
		PatternType pattern_type;

		/** \~English
			\brief Pattern width.
			\~Russian
			\brief Ширина калибровочного шаблона.
		*/
		int pattern_width;

		/** \~English
			\brief Pattern height.
			\~Russian
			\brief Высота калибровочного шаблона.
		*/
		int pattern_height;

		/** \~English
			\brief Fix the aspect ratio.
			\~Russian
			\brief Зафиксировать соотношение сторон.
		*/
		bool fix_aspect_ratio;  
		
		/** \~English
			\brief Consider a tangential distortion as zero.
			\~Russian
			\brief Считать тангенциальную дисторсию нулевой.
		*/
		bool calib_zero_tangent_dist;
		
		/** \~English
			\brief Fix the principal point at the center.
			\~Russian
			\brief Зафиксировать принципиальную точку в центре изображения.
		*/
		bool calib_fix_principal_point;  
		
		/** \~English
			\brief Use the rational distortion model.
			\~Russian
			\brief Использовать rational distortion model.
		*/
		bool rational_distortion_model; 

		CalibrationSettings()
		{
			fix_aspect_ratio          = false;
			calib_zero_tangent_dist   = false;
			calib_fix_principal_point = false;
			rational_distortion_model = true;
		}
	};


	/**
		\~English
		\brief
			Initialize the calibration process.
			All previous accepted patterns will be removed.

		\param[in]  settings
			Calibration process settings.

		\~Russian
		\brief
			Инициализация процесса калибровки.
			Ранее принятые шаблоны будут удалены.

		\param[in]  settings
			Настройки калибровки.
	*/
	void initCalibration(
		const CalibrationSettings settings);


	/**
		\~English
		\brief
			Search a pattern in the image and store it.

		\param[in]  image
			An image or a videoframe.

		\param[out]  pattern_found
			The pattern was successfully found in a given image.
			Can be NULL.

		\param[out]  pattern_accepted
			The found pattern was accepted (i.e., it differs significantly from the patterns already accepted).
			Can be NULL.

		\param[out]  pattern_points
			Points of a detected pattern.
			Can be NULL.

		\~Russian
		\brief
			Поиск калибровочного шаблона на изображении и его сохранение.

		\param[in]  image
			Изображение или кадр видео.

		\param[out]  pattern_found
			Шаблон был найден.
			Может принимать значение NULL.

		\param[out]  pattern_accepted
			Шаблон был принят (т.е. он достаточно отличается от уже принятых шаблонов).
			Может принимать значение NULL.

		\param[out]  pattern_points
			Точки найденного шаблона.
			Может принимать значение NULL.
	*/
	void addImage(
		const RawImage image,
		bool *pattern_found,
		bool *pattern_accepted,
		std::vector<Point> *pattern_points);


	/**
		\~English
		\brief
			Evaluate covering of the pattern space to get more accurate calibration.

		\return
			A real number from 0 to 1.

		\~Russian
		\brief
			Оценка покрытия пространства поз калибровочного шаблона для более точной калибровки.

		\return
			Вещественное число от 0 до 1.
	*/
	float getPatternSpaceCoverProgress() const;


	/**
		\~English
		\brief
			Get a tip where to put a pattern in the image to get a better pattern space coverage.

		\return
			An empty vector if the tip is not ready, or a vector of pattern points.

		\~Russian
		\brief
			Получить подсказку о требуемой позиции калибровочного шаблона на изображении для лучшего покрытия.

		\return
			Пустой вектор, если подсказка не готова, или вектор точек шаблона-подсказки.
	*/
	std::vector<Point> getTip();


	/**
		\~English
		\brief
			Calibrate the camera.

		\param[in]  max_used_patterns_count
			Maximum number of patterns to use for calculation.
			If more patterns are accepted, the subset covering the pattern space best will be used.
			The recommended value is 50.

		\param[out]  reprojection_error
			Average reprojection error.
			Normal values are about 1 or lower.

		\return
			true, if the calibration was successful, otherwise false.

		\~Russian
		\brief
			Калибровка камеры.

		\param[in]  max_used_patterns_count
			Максимальное количество шаблонов для использования в расчетах. 
			Если принято больше шаблонов, то будет выбрано подмножество, покрывающее пространство поз лучше всего.
			Рекомендуемое значение - 50.

		\param[out]  reprojection_error
			Средняя ошибка репроекции.
			Нормальное значение - около 1 или меньше.

		\return
			true, если калибровка выполенена успешно, иначе false.
	*/
	bool calibrate(
		const int max_used_patterns_count,
		float *reprojection_error);


	/**
		\~English
		\brief
			Correct the image distortion using the calibrated or loaded camera parameters.

		\param[in]  image
			An image or a videoframe.
			The size can differ from the size of the images used for calibration.
			I.e. you can calibrate a camera once with one resolution and
			then calibrate the distortion of images from the same camera with any resolution.

		\param[in]  alpha
			A real value from 0 (the result image contains only valid pixels (no black areas))
			to 1 (all pixels from the original image are stored in the result image (no source image pixels are lost)).
			Intermediate values yield an intermediate result between those two extreme cases.

		\return
			Undistorted image of the same size and type as the source image.

		\~Russian
		\brief
			Скорректировать дисторсию изображения, используя откалиброванные или загруженные параметры камеры.

		\param[in]  image
			Изображение или кадр видео.
			Размер изображения может отличаться от размера, используемого для калибровки.
			Т.е. можно откалибровать камеру один раз, используя одно разрешение, 
			и потом корректировать изображения от этой же камеры, используя любое другое разрешение.

		\param[in]  alpha
			Вещественное число от 0 (результирующее изображение содержит только значимые пиксели (нет черных областей))
			до 1 (все пиксели исходного изображения будут присутствовать в результирующем).
			Промежуточные значения дают промежуточный результат между этими двумя крайними случаями.

		\return
			Откорректированное изображение того же размера и типа, что и исходное изображение.
	*/
	IRawImage::Ptr undistort(
		const RawImage image,
		const float alpha) const;



	/**
		\~English
		\brief
			Save the calibrated camera parameters to binary_stream.
			The format is platform-independent.

		\param[out]  binary_stream
			Output stream object.
			The file stream (std::ofstream) must be opened with the std::ios_base::binary flag.

		\~Russian
		\brief
			Сохранить откалиброванные параметры.
			Формат платформонезависимый.

		\param[out]  binary_stream
			Объект потока вывода.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.
	*/
	void saveCameraParameters(
		std::ostream &binary_stream) const;

	/**
		\~English
		\brief
			Save the calibrated camera parameters to binary_stream.
			The format is platform-independent.

		\param[out]  binary_stream
			Output stream object.
			The file stream (std::ofstream) must be opened with the std::ios_base::binary flag.

		\~Russian
		\brief
			Сохранить откалиброванные параметры.

		\param[out]  binary_stream
			Объект потока вывода.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.
	*/
	void saveCameraParameters(
		pbio::stl_wraps::WrapOStream &binary_stream) const;


	/**
		\~English
		\brief
			Load the calibrated camera parameters from binary_stream.
			The format is platform-independent.

		\param[in]  binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the std::ios_base::binary flag.

		\~Russian
		\brief
			Загрузить откалиброванные параметры.
			Формат платформонезависимый.

		\param[in]  binary_stream
			Объект потока ввода.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.
	*/
	void loadCameraParameters(
		std::istream &binary_stream);

	/**
		\~English
		\brief
			Load the calibrated camera parameters from binary_stream.
			The format is platform-independent.

		\param[in]  binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the std::ios_base::binary flag.

		\~Russian
		\brief
			Загрузить откалиброванные параметры.
			Формат платформонезависимый.

		\param[in]  binary_stream
			Объект потока ввода.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.
	*/
	void loadCameraParameters(
		pbio::stl_wraps::WrapIStream &binary_stream);


private:

	CameraCalibrator(
		const DHPtr &dll_handle,
		void* impl);

	int _pattern_width;
	int _pattern_height;

	friend class FacerecService;
	friend class object_with_ref_counter<CameraCalibrator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline
CameraCalibrator::CameraCalibrator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	_pattern_width = 0;
	_pattern_height = 0;
}


inline
void CameraCalibrator::initCalibration(
	const CalibrationSettings settings)
{
	_pattern_height = 0;
	_pattern_width = 0;

	void* exception = NULL;

	_dll_handle->CameraCalibrator_initCalibration(
		_impl,

		settings.image_width,
		settings.image_height,
		settings.pattern_type,
		settings.pattern_width,
		settings.pattern_height,

		settings.fix_aspect_ratio,
		settings.calib_zero_tangent_dist,
		settings.calib_fix_principal_point,
		settings.rational_distortion_model,

		&exception);

	checkException(exception, *_dll_handle);

	_pattern_height = settings.pattern_height;
	_pattern_width = settings.pattern_width;
}


inline
void CameraCalibrator::addImage(
	const RawImage image,
	bool *pattern_found,
	bool *pattern_accepted,
	std::vector<Point> *pattern_points)
{

	int32_t pattern_found_i = 0;
	int32_t pattern_accepted_i = 0;

	std::vector<float> pattern_points_f;

	if(_pattern_height > 0 && _pattern_width > 0)
		pattern_points_f.resize(_pattern_height * _pattern_width * 2);

	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	_dll_handle->CameraCalibrator_addImage_with_crop(
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

		&pattern_found_i,
		&pattern_accepted_i,
		pattern_points_f.empty() ?
			NULL :
			&pattern_points_f[0],
		&exception);

	checkException(exception, *_dll_handle);


	if(pattern_found)
		*pattern_found = pattern_found_i;

	if(pattern_accepted)
		*pattern_accepted = pattern_accepted_i;

	if(pattern_points)
	{
		pattern_points->resize(_pattern_width * _pattern_height);
		for(size_t i = 0; i < pattern_points->size(); ++i)
		{
			pattern_points->at(i).x = pattern_points_f[i * 2 + 0];
			pattern_points->at(i).y = pattern_points_f[i * 2 + 1];
		}
	}
}


inline
float CameraCalibrator::getPatternSpaceCoverProgress() const
{
	void* exception = NULL;

	const float progress = _dll_handle->CameraCalibrator_getPatternSpaceCoverProgress(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return progress;
}


inline
std::vector<Point> CameraCalibrator::getTip()
{
	std::vector<float> pattern_points_f;

	if(_pattern_height > 0 && _pattern_width > 0)
		pattern_points_f.resize(_pattern_height * _pattern_width * 2);

	void* exception = NULL;

	const int32_t tip_ready = _dll_handle->CameraCalibrator_getTip(
		_impl,
		pattern_points_f.empty() ?
			NULL :
			&pattern_points_f[0],
		&exception);

	checkException(exception, *_dll_handle);

	std::vector<Point> pattern_points;

	if(tip_ready)
	{
		pattern_points.resize(_pattern_width * _pattern_height);

		for(size_t i = 0; i < pattern_points.size(); ++i)
		{
			pattern_points[i].x = pattern_points_f[i * 2 + 0];
			pattern_points[i].y = pattern_points_f[i * 2 + 1];
		}
	}

	return pattern_points;
}



inline
bool CameraCalibrator::calibrate(
	const int max_used_patterns_count,
	float *reprojection_error_ptr)
{
	void* exception = NULL;

	const float reprojection_error = _dll_handle->CameraCalibrator_calibrate(
		_impl,
		max_used_patterns_count,
		&exception);

	checkException(exception, *_dll_handle);

	if(reprojection_error < 0 - 0.1f)
		return false;

	if(reprojection_error_ptr)
		*reprojection_error_ptr = reprojection_error;

	return true;
}


inline
IRawImage::Ptr CameraCalibrator::undistort(
	const RawImage image,
	const float alpha) const
{
	pbio::image::Image::Ptr result(
		new pbio::image::Image(
			image.height,
			image.width,
			image.format));

	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	_dll_handle->CameraCalibrator_undistort_with_crop(
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

		alpha,
		result->data(),
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}



inline
void CameraCalibrator::saveCameraParameters(
	std::ostream &binary_stream) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	saveCameraParameters(binary_stream_wrap);
}

inline
void CameraCalibrator::saveCameraParameters(
	pbio::stl_wraps::WrapOStream &binary_stream) const
{
	void* exception = NULL;

	_dll_handle->CameraCalibrator_saveCameraParameters(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);
}


inline
void CameraCalibrator::loadCameraParameters(
	std::istream &binary_stream)
{
	pbio::stl_wraps::WrapIStreamImpl binary_stream_wrap(binary_stream);

	loadCameraParameters(binary_stream_wrap);
}

inline
void CameraCalibrator::loadCameraParameters(
	pbio::stl_wraps::WrapIStream &binary_stream)
{
	void* exception = NULL;

	_dll_handle->CameraCalibrator_loadCameraParameters(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapIStream::read_func,
		&exception);

	checkException(exception, *_dll_handle);
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__CAMERACALIBRATOR_H_
