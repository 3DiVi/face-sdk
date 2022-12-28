/**
	\file RawSample.h
	\~English
	\brief RawSample - Interface object that stores a captured face sample.
	\~Russian
	\brief RawSample - Интерфейсный объект, хранящий образец лица.
*/

#ifndef __PBIO_API__PBIO__RAW_SAMPLE_H_
#define __PBIO_API__PBIO__RAW_SAMPLE_H_


#include <ostream>

#include "ComplexObject.h"
#include "ExceptionCheck.h"
#include "Point.h"
#include "RawImage.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"
#include "stl_wraps_impls/WrapVectorImpl.h"


namespace pbio
{
class AgeGenderEstimator;
class Capturer;
class FacerecService;
class LivenessEstimator;
class QualityEstimator;
class FaceQualityEstimator;
class Recognizer;
class VideoWorker;
class EmotionsEstimator;
class DepthLivenessEstimator;
class IRLivenessEstimator;
class Liveness2DEstimator;
class FaceAttributesEstimator;
class RawSampleExtractor;

/** \~English
	\brief Interface object that stores a captured face sample.
	\~Russian
	\brief Интерфейсный объект, хранящий образец лица.
*/
class RawSample : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to RawSample.
		\~Russian
		\brief Псевдоним для типа умного указателя на RawSample.
	*/
	typedef LightSmartPtr<RawSample>::tPtr Ptr;

	/** \~English
		\brief Sample types (see also Capturer::CapturerType).
		\~Russian
		\brief Типы образцов (см. также Capturer::CapturerType).
	*/
	enum SampleType
	{
		/** \~English
			\brief The face is frontal-oriented.
			\~Russian
			\brief Фронтальное лицо.
		*/
		SAMPLE_TYPE_FRONTAL = 0,

		/** \~English
			\brief The face is left-profile-oriented.
			\~Russian
			\brief Лицо в профиль, смотрящее влево.
		*/
		SAMPLE_TYPE_LEFT_PROFILE = 1,

		/** \~English
			\brief The face is right-profile-oriented.
			\~Russian
			\brief Лицо в профиль, смотрящее вправо.
		*/
		SAMPLE_TYPE_RIGHT_PROFILE = 2
	};

	/**  \~English
		\brief Image formats for saving.
		\~Russian 
		\brief Форматы изображения для сохранения (сериализации).
	*/
	enum ImageFormat
	{
		/** \~English
			\brief JPEG (lossy compression).
			\~Russian
			\brief JPEG (сжатие с потерями).
		*/
		IMAGE_FORMAT_JPG = 0,

		/** \~English
			\brief PNG (lossless compression).
			\~Russian
			\brief PNG (сжатие без потерь).
		*/
		IMAGE_FORMAT_PNG = 1,

		/** \~English
			\brief TIFF (lossless compression).
			\~Russian
			\brief TIFF (сжатие без потерь).
		*/
		IMAGE_FORMAT_TIF = 2,

		/** \~English
			\brief BMP (no compression).
			\~Russian
			\brief BMP (без сжатия).
		*/
		IMAGE_FORMAT_BMP = 3  
	};

	/** \~English
		\brief Face cropping types.
		\~Russian
		\brief Типы обрезки лица.
	*/
	enum FaceCutType
	{
		/** \~English
			\brief Unspecified cropping (any sample type).
			\~Russian
			\brief Базовый (подходит для образцов всех типов).
		*/
		FACE_CUT_BASE = 0,

		/** \~English
			\brief ISO/IEC 19794-5 Full Frontal (for ID, travel documents) (only frontal sample type).
			\~Russian
			\brief Полный фронтальный ТИЛ (тип изображения лица) по ГОСТ Р ИСО / МЭК 19794-5 (ISO/IEC 19794-5 Full Frontal) (только для фронтальных образцов).
		*/
		FACE_CUT_FULL_FRONTAL = 1,

		/** \~English
			\brief ISO/IEC 19794-5 Token Frontal (fixed eye positions) (only frontal sample type).
			\~Russian
			\brief Условно фронтальный ТИЛ (тип изображения лица) по ГОСТ Р ИСО / МЭК 19794-5 (ISO/IEC 19794-5 Token Frontal) (только для фронтальных образцов).
		*/
		FACE_CUT_TOKEN_FRONTAL = 2   
	};

	/** \~English
		\brief Rectangle in an image.
		\~Russian
		\brief Прямоугольник на изображении.
	*/
	typedef pbio::Rectangle Rectangle;

	/** \~English
		\brief 2D point.
		\~Russian
		\brief 2D точка.
	*/
	typedef pbio::Point Point;

	/** \~English
		\brief Face orientation angles.
		\~Russian
		\brief Углы ориентации лица.
	*/
	struct Angles
	{
		/** \~English
			\brief Yaw angle in degrees.
			\~Russian
			\brief Угол поворота вокруг вертикальной оси в градусах.
		*/
		float yaw;

		/** \~English
			\brief Pitch angle in degrees.
			\~Russian
			\brief Угол поворота вокруг горизонтальной оси в градусах.
		*/
		float pitch;

		/** \~English
			\brief Roll angle in degrees.
			\~Russian
			\brief Угол поворота в картинной плоскости в градусах.
		*/
		float roll;
	};


	/**
		\~English
		\brief
			Get a face bounding rectangle. Thread-safe.

		\return
			Rectangle bounding face on the original image.

		\~Russian
		\brief
			Получить ограничивающий прямоугольник лица. Потокобезопасный.

		\return
			Прямоугольник, ограничивающий лицо на оригинальном изображении.
	*/
	Rectangle getRectangle() const;


	/**
		\~English
		\brief
			Get anthropometric landmarks. Only frontal samples. Thread-safe.

		\return
			The vector of anthropometric points in the original image.

		\~Russian
		\brief
			Получить антропометрические точки лица. Только для фронтальных образцов. Потокобезопасный.

		\return
			Вектор позиций антропометрических точек на оригинальном изображении.
	*/
	std::vector<Point> getLandmarks() const;


	/**
		\~English
		\brief
			Get the characteristic points of the eyes. Only frontal samples. Thread-safe.

		\return
			The vector of the positions of the points of the pupils and the boundaries of the eyelids in the original image.

		\~Russian
		\brief
			Получить характерные точки глаз. Только для фронтальных образцов. Потокобезопасный.

		\return
			Вектор позиций точек зрачков и контуров век на оригинальном изображении.
	*/
	std::vector<Point> getIrisLandmarks() const;


	/**
		\~English
		\brief
			Get a point of the left eye. Only frontal samples. Thread-safe.

		\return
			Point of the left eye.

		\~Russian
		\brief
			Получить позицию левого глаза. Только для фронтальных образцов. Потокобезопасный.

		\return
			Позиция левого глаза на оригинальном изображении.
	*/
	Point getLeftEye() const;


	/**
		\~English
		\brief
			Get a point of the right eye. Only frontal samples. Thread-safe.

		\return
			Point of the right eye.

		\~Russian
		\brief
			Получить позицию правого глаза. Только для фронтальных образцов. Потокобезопасный.

		\return
			Позиция правого глаза на оригинальном изображении.
	*/
	Point getRightEye() const;

	/**
		\~English
		\brief
			Get a face orientation. Only frontal samples. Thread-safe.

		\return
			Face orientation angles in degrees.

		\~Russian
		\brief
			Получить углы ориентации лица. Только для фронтальных образцов. Потокобезопасный.

		\return
			Углы ориентации лица.
	*/
	Angles getAngles() const;


	/**
		\~English
		\brief
			Get a face ID. Thread-safe.

		\return
			- A non-negative track id, if this sample was captured by a tracker, or
			- Any meaningless negative value, otherwise.

		\~Russian
		\brief
			Получить целочисленный идентификатор. Потокобезопасный.

		\return
			- Неотрицательное число, если образец был получен с помощью трекера, или
			- Любое отрицательное число в противном случае.
	*/
	int getID() const;


	/**
		\~English
		\brief
			Get an ID of the frame, in which this RawSample is detected 
			(for samples made with VideoWorker). Thread-safe.

		\return
			Zero if this RawSample was made not with VideoWorker,
			\n
			otherwise it's a nonnegative integer - id (that was returned
			by the VideoWorker::addVideoFrame) of the frame, 
			in which this RawSample is detected.

		\~Russian
		\brief
			Получить ID кадра, на котором был обнаружен образец 
			(для образцов, полученных с помощью VideoWorker). Потокобезопасный.

		\return
			Ноль, если образец был получен не через VideoWorker,
			\n
			иначе - неотрицательное целое число - 
			идентификатор (который был возвращен методом VideoWorker::addVideoFrame) 
			кадра, на котором был обнаружен образец.
	*/
	int getFrameID() const;


	/**
		\~English
		\brief
			Does the sample store the original image
			(true for samples made with VideoWorker with enabled store_original_frame_in_raw_sample).
			Thread-safe.

		\return
			true, if it has original image, otherwise false.

		\~Russian
		\brief
			Хранит ли образец оригинальное изображение, с которого он был получен
			(true для образцов, полученных с помощью VideoWorker с включенным 
			параметром store_original_frame_in_raw_sample).
			Потокобезопасный.

		\return
			true, если содержит, иначе false.
	*/
	bool hasOriginalImage() const;

	/**
		\~English
		\brief
			Get the score of the detected face (for samples made with supported single-shot Capturers).
			Thread-safe.

		\return
			One if this RawSample was made with an unsupported detector,
			\n
			otherwise - a number in the range [0 ... 1].

		\~Russian
		\brief
			Получить уверенность детектирования лица
			(для образцов, полученных с помощью поддерживаемого Capturer). Потокобезопасный.

		\return
			Один, если образец был получен через неподдерживаемый Capturer,
			\n
			иначе - число в диапазоне [0 ... 1].
	*/
	float getScore() const;

	/**
		\~English
		\brief
			Get the score of face visibility.
			Thread-safe.

		\return
			One if face is fully visible,
			\n
			zero if face is invisible,
			\n
			otherwise - a number in the range [0 ... 1].

		\~Russian
		\brief
			Получить оценку видимости лица.
			Потокобезопасный.

		\return
			Один, если лицо полностью видимо,
			\n
			ноль, если лицо невидимо,
			\n
			иначе - число в диапазоне [0 ... 1].
	*/
	float getFaceVisibilityScore() const;

	/**
		\~English
		\brief
			Get an original image.
			Throws an exception if a sample doesn’t contain an original image (check with RawSample::hasOriginalImage first).
			Thread-safe.

		\param[out]  rows
			Height of the original image.

		\param[out]  cols
			Width of the original image.

		\param[out]  format
			Format of the original image.

		\param[out]  data
			Pointer to the image data buffer.

		\~Russian
		\brief
			Получить оригинальное изображение.
			Выбрасывается исключение, если образец не содержит оригинальное изображение (проверка через RawSample::hasOriginalImage).
			Потокобезопасный.

		\param[out]  rows
			Высота оригинального изображения.

		\param[out]  cols
			Ширина оригинального изображения.

		\param[out]  format
			Формат оригинального изображения.

		\param[out]  data
			Указатель на данные изображения.
	*/
	void getOriginalImage(
		int &rows,
		int &cols,
		IRawImage::Format &format,
		void const* &data) const;

	/**
		\~English
		\brief
			Get a sample type. Thread-safe.

		\return
			Type of the sample.

		\~Russian
		\brief
			Получить тип образца. Потокобезопасный.

		\return
			Тип образца.
	*/
	SampleType getType() const;



	/**
		\~English
		\brief
			Get a cropping rectangle (for preview) that will be used in cutFaceImage. Thread-safe.

		\return
			4 points that are corners of a rectangle in the counterclockwise order starting from the upper left corner.

		\param[in]  cut_type
			Face cropping types.

		\~Russian
		\brief
			Получить прямоугольник (для предпросмотра обрезки). Потокобезопасный.

		\return
			4 точки - углы прямоугольника в порядке против часовой стрелки, начиная с левого верхнего угла.

		\param[in]  cut_type
			Тип обрезки.
	*/
	std::vector<Point> getFaceCutRectangle(FaceCutType cut_type) const;


	/**
		\~English
		\brief
			Crop the face and save in the specified format. Thread-safe.

		\param[out]  binary_stream
			Output stream object for image saving.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  format
			Image encoding format.

		\param[in]  cut_type
			Face cropping types.

		\~Russian
		\brief
			Обрезать лицо и сохранить в указанном формате. Потокобезопасный.

		\param[out]  binary_stream
			Поток для сохранения закодированного изображения.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  format
			Формат для сохранения изображения.

		\param[in]  cut_type
			Тип обрезки.
	*/
	void cutFaceImage(
		std::ostream &binary_stream,
		ImageFormat format,
		FaceCutType cut_type) const;


	/**
		\~English
		\brief
			Crop face in RawImage format (with raw pixels). Thread-safe.

		\param[in]  color_model
			Image color model.

		\param[in]  cut_type
			Face cropping types.

		\return  RawImage with cropped face

		\~Russian
		\brief
			Обрезать лицо и выдать в формате RawImage (с декодированными пикселями). Потокобезопасный.

		\param[in]  color_model
			Цветовая модель изображения.

		\param[in]  cut_type
			Тип обрезки.

		\return  RawImage c кропом лица
	*/
	RawImage cutFaceRawImage(
		RawImage::Format color_model,
		FaceCutType cut_type) const;
	
	/**
		\~English
		\brief
			Crop the face and save in the specified format. Thread-safe.

		\param[out]  binary_stream
			Output stream object for image saving.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  format
			Image encoding format.

		\param[in]  cut_type
			Face cropping types.

		\~Russian
		\brief
			Обрезать лицо и сохранить в указанном формате. Потокобезопасный.

		\param[out]  binary_stream
			Поток для сохранения закодированного изображения.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  format
			Формат для сохранения изображения.

		\param[in]  cut_type
			Тип обрезки.
	*/
	void cutFaceImage(
		pbio::stl_wraps::WrapOStream &binary_stream,
		ImageFormat format,
		FaceCutType cut_type) const;

	/**
		\~English
		\brief
			Crop the face and save it in original size in the specified format. Thread-safe.

		\param[in]  original_image
			Image that was used to create this sample.

		\param[out]  binary_stream
			Output stream object for image saving.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  format
			Image encoding format.

		\param[in]  cut_type
			Face cropping type.

		\~Russian
		\brief
			Обрезать лицо и сохранить в исходном размере в указанном формате. Потокобезопасный.
		
		\param[in] original_image
			Изображение, используемое для создания образца. 

		\param[out]  binary_stream
			Объект потока вывода для сохранения изображения.
			Поток файла (std::ofstream) (если таковой имеется) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  format
			Формат кодирования изображения.

		\param[in]  cut_type
			Тип обрезки лица.
	*/
	void cutFaceImage(
		const RawImage original_image,
		std::ostream &binary_stream,
		ImageFormat format,
		FaceCutType cut_type) const;

	/**
		\~English
		\brief
			Crop the face and save it in original size in the specified format. Thread-safe.

		\param[in]  original_image
			Image that was used to create this sample.

		\param[out]  binary_stream
			Output stream object for image saving.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  format
			Image encoding format.

		\param[in]  cut_type
			Face cropping type.

		\~Russian
		\brief
			Обрезать лицо и сохранить в исходном размере в указанном формате. Потокобезопасный.
		
		\param[in] original_image
			Изображение, используемое для создания образца. 

		\param[out]  binary_stream
			Объект потока вывода для сохранения изображения.
			Поток файла (std::ofstream) (если таковой имеется) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  format
			Формат кодирования изображения.

		\param[in]  cut_type
			Тип обрезки лица.
	*/
	void cutFaceImage(
		const RawImage original_image,
		pbio::stl_wraps::WrapOStream &binary_stream,
		ImageFormat format,
		FaceCutType cut_type) const;

	/**
		\~English
		\brief
			Save (serialize) the sample.
			Original image will not be saved
			(i.e. only the part of an image containing the face is saved).
			The format is platform-independent.
			Thread-safe.
			After saving, you can load it with FacerecService::loadRawSample.

		\param[out]  binary_stream
			Output stream object for saving the raw sample.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  format
			Image encoding format.

		\~Russian
		\brief
			Сохранить (сериализовать) образец.
			Оригинальное изображение не сохраняется.
			(т.е. сохраняется только часть изображения, содержащая лицо).
			Формат платформонезависимый.
			Потокобезопасный.
			Десериализовать образец можно через FacerecService::loadRawSample.

		\param[out]  binary_stream
			Объект потока.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  format
			Формат кодирования изображения.
	*/
	void save(std::ostream &binary_stream, ImageFormat format) const;

	/**
		\~English
		\brief
			Save (serialize) the sample.
			Original image will not be saved
			(i.e. only the part of an image containing the face is saved).
			The format is platform-independent.
			Thread-safe.
			After saving, you can load it with FacerecService::loadRawSample.

		\param[out]  binary_stream
			Output stream object for saving the raw sample.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  format
			Image encoding format.

		\~Russian
		\brief
			Сохранить (сериализовать) образец.
			Оригинальное изображение не сохраняется.
			(т.е. сохраняется только часть изображения, содержащая лицо).
			Формат платформонезависимый.
			Потокобезопасный.
			Десериализовать образец можно через FacerecService::loadRawSample.

		\param[out]  binary_stream
			Объект потока.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  format
			Формат кодирования изображения.
	*/
	void save(pbio::stl_wraps::WrapOStream &binary_stream, ImageFormat format) const;


	/**
		\~English
		\brief
			Save (serialize) all sample information except the image. You have to store the image
			on your own in order to pass it to the FacerecService::loadRawSampleWithoutImage for deserialization.
			If you transformed the image in soma way, you can set the parameters of coordinate space transformation:
			(x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
			that will be applied to the information about face position.
			Note that you must set the same transformation only once - either during serialization or during deserialization,
			otherwise transformation will be applied twice, which is wrong.
			The format is platform-independent.
			Thread-safe.
			After saving, you can load it with FacerecService::loadRawSampleWithoutImage.

		\param[out]  binary_stream
			Output stream object for saving the raw sample.
			The file stream (std::ofstream) (if any) must be opened with the set std::ios_base::binary flag.

		\param[in]  space_translation_x
			Coordinate space transform parameter.

		\param[in]  space_translation_y
			Coordinate space transform parameter.

		\param[in]  space_scale
			Coordinate space transform parameter.

		\~Russian
		\brief
			Сохранить (сериализовать) всю информацию, кроме изображения. Вы должны сохранить изображение
			самостоятельно для того, чтобы подать его в FacerecService::loadRawSampleWithoutImage для десериализации.
			В случае, если над изображением была выполнена трансформация,
			вы можете указать параметры преобразования пространства координат:
			(x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
			которое будет применено к информации о позиции лица.
			Заметьте, что параметры одного и того же преобразования нужно передавать только один раз - либо
			при сериализации, либо при десериализации, иначе преобразование будет применено дважды, что неверно.
			Формат платформонезависимый.
			Потокобезопасный.
			Десериализовать образец можно через FacerecService::loadRawSampleWithoutImage.

		\param[out]  binary_stream
			Объект потока для сохранения "сырого" изображения.
			Поток файла (std::ofstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  space_translation_x
			Параметр преобразования пространства координат.

		\param[in]  space_translation_y
			Параметр преобразования пространства координат.

		\param[in]  space_scale
			Параметр преобразования пространства координат.
	*/
	void saveWithoutImage(
		pbio::stl_wraps::WrapOStream &binary_stream,
		const double space_translation_x = 0,
		const double space_translation_y = 0,
		const double space_scale = 1) const;

	/**
		\copydoc saveWithoutImage
	*/
	void saveWithoutImage(
		std::ostream &binary_stream,
		const double space_translation_x = 0,
		const double space_translation_y = 0,
		const double space_scale = 1) const;


	/**
		\~English
		\brief
			Clone this RawSample with an internal face image downscaled to suitable size and without an original image.
			In order to reduce memory consumption (both in RAM and after serialization).
			\n At work Capturer and VideoWorker automatically downscale all RawSamples if the 
			<i>downscale_rawsamples_to_preferred_size</i> parameter is enabled (enabled by default),
			but it decreases the performance.
			\n It's recommended to disable <i>downscale_rawsamples_to_preferred_size</i>
			and use RawSample::downscaleToPreferredSize manually for RawSamples that you
			need to save or keep in RAM for a long time.

		\return
			Created RawSample.

		\~Russian
		\brief
			Создать копию образца с внутренним изображением лица, уменьшенным до предпочтительного
			размера и без оригинального изображения.
			В целях уменьшения потребления памяти (как в оперативной памяти, так и после сериализации).
			\n При работе Capturer и VideoWorker автоматически уменьшают все образцы, если параметр
			<i>downscale_rawsamples_to_preferred_size</i> включен (включен по умолчанию), но это снижает
			производительность.
			\n Рекомендуется отключать <i>downscale_rawsamples_to_preferred_size</i>,
			и использовать RawSample::downscaleToPreferredSize вручную для образцов, которые
			вам нужно сохранить или в течение длительного времени удерживать в оперативной памяти.

		\return
			Созданный RawSample.
	*/
	RawSample::Ptr downscaleToPreferredSize() const;

	//! @cond IGNORED
	void* getPtr() const;
	//! @endcond

private:

	RawSample(
		const DHPtr &dll_handle,
		void* impl);

	friend class AgeGenderEstimator;
	friend class Capturer;
	friend class FacerecService;
	friend class LivenessEstimator;
	friend class QualityEstimator;
	friend class FaceQualityEstimator;
	friend class Recognizer;
	friend class VideoWorker;
	friend class EmotionsEstimator;
	friend class DepthLivenessEstimator;
	friend class IRLivenessEstimator;
	friend class Liveness2DEstimator;
	friend class FaceAttributesEstimator;
	friend class RawSampleExtractor;
	friend class object_with_ref_counter<RawSample>;
};

}  // pbio namespace


////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline
RawSample::RawSample(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
RawSample::Rectangle RawSample::getRectangle() const
{
	Rectangle rectangle;

	void* exception = NULL;

	_dll_handle->RawSample_getRectangle(
		_impl,
		&rectangle.x,
		&rectangle.y,
		&rectangle.width,
		&rectangle.height,
		&exception);

	checkException(exception, *_dll_handle);

	return rectangle;
}


inline
std::vector<RawSample::Point> RawSample::getLandmarks() const
{
	std::vector<float> coordinates;

	void* exception = NULL;

	_dll_handle->RawSample_getLandmarks(
		_impl,
		&coordinates,
		pbio::stl_wraps::assign_floats_vector_func,
		&exception);

	checkException(exception, *_dll_handle);

	std::vector<Point> points(coordinates.size() / 3);

	for(size_t i = 0; i < points.size(); ++i)
	{
		points[i].x = coordinates[i * 3 + 0];
		points[i].y = coordinates[i * 3 + 1];
		points[i].z = coordinates[i * 3 + 2];
	}

	return points;
}


inline
std::vector<RawSample::Point> RawSample::getIrisLandmarks() const
{
	std::vector<float> coordinates;

	void* exception = NULL;

	_dll_handle->RawSample_getIrisLandmarks(
		_impl,
		&coordinates,
		pbio::stl_wraps::assign_floats_vector_func,
		&exception);

	checkException(exception, *_dll_handle);

	std::vector<Point> points(coordinates.size() / 3);

	for(size_t i = 0; i < points.size(); ++i)
	{
		points[i].x = coordinates[i * 3 + 0];
		points[i].y = coordinates[i * 3 + 1];
		points[i].z = coordinates[i * 3 + 2];
	}

	return points;
}


inline
RawSample::Point RawSample::getLeftEye() const
{
	Point p;

	void* exception = NULL;

	_dll_handle->RawSample_getLeftEye(
		_impl,
		&p.x,
		&p.y,
		&exception);

	checkException(exception, *_dll_handle);

	return p;
}

inline
RawSample::Point RawSample::getRightEye() const
{
	Point p;

	void* exception = NULL;

	_dll_handle->RawSample_getRightEye(
		_impl,
		&p.x,
		&p.y,
		&exception);

	checkException(exception, *_dll_handle);

	return p;
}

inline
RawSample::Angles RawSample::getAngles() const
{
	Angles angles;

	void* exception = NULL;

	_dll_handle->RawSample_getAngles(
		_impl,
		&angles.yaw,
		&angles.pitch,
		&angles.roll,
		&exception);

	checkException(exception, *_dll_handle);

	return angles;
}

inline
int RawSample::getID() const
{
	void* exception = NULL;

	const int result = _dll_handle->RawSample_getID(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
int RawSample::getFrameID() const
{
	void* exception = NULL;

	const int result = _dll_handle->RawSample_getFrameID(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
RawSample::SampleType RawSample::getType() const
{
	void* exception = NULL;

	const int result = _dll_handle->RawSample_getType(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return (SampleType) result;
}


inline
std::vector<RawSample::Point> RawSample::getFaceCutRectangle(
	FaceCutType cut_type) const
{
	void* exception = NULL;

	float corners[8];

	_dll_handle->RawSample_getFaceCutRectangle(
		_impl,
		int(cut_type),
		corners,
		&exception);

	checkException(exception, *_dll_handle);

	std::vector<RawSample::Point> result(4);
	for(int i = 0; i < 4; ++i)
	{
		result[i].x = corners[i * 2 + 0];
		result[i].y = corners[i * 2 + 1];
	}

	return result;
}


inline
void RawSample::cutFaceImage(
	std::ostream &binary_stream,
	RawSample::ImageFormat format,
	RawSample::FaceCutType cut_type) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	cutFaceImage(binary_stream_wrap, format, cut_type);
}

inline
RawImage RawSample::cutFaceRawImage(
	RawImage::Format color_model,
	FaceCutType cut_type) const
{
	int32_t width, height;
	std::ostringstream stream;
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(stream);

	void* exception = NULL;

	_dll_handle->RawSample_cutFaceImage(
		_impl,
		&binary_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&width,
		&height,
		-1,
		int(color_model),
		int(cut_type),
		&exception);

	checkException(exception, *_dll_handle);

	pbio::RawImage result(
		width,
		height,
		color_model,
		(unsigned char*) stream.str().c_str(),
		stream.str().size());

	return result;
}

inline
void RawSample::cutFaceImage(
	pbio::stl_wraps::WrapOStream &binary_stream,
	ImageFormat format,
	FaceCutType cut_type) const
{
	void* exception = NULL;

	int32_t w, h;

	_dll_handle->RawSample_cutFaceImage(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		&w,
		&h,
		int(format),
		-1,
		int(cut_type),
		&exception);

	checkException(exception, *_dll_handle);
}


inline
void RawSample::cutFaceImage(
	const RawImage original_image,
	std::ostream &binary_stream,
	ImageFormat format,
	FaceCutType cut_type) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	cutFaceImage(
		original_image,
		binary_stream_wrap,
		format,
		cut_type);
}

inline
void RawSample::cutFaceImage(
	const RawImage original_image,
	pbio::stl_wraps::WrapOStream &binary_stream,
	ImageFormat format,
	FaceCutType cut_type) const
{
	void* exception = NULL;

	const RawImage::CapiData cdata = original_image.makeCapiData();

	_dll_handle->RawSample_cutFaceImage_original_raw_image_with_crop(
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
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		int(format),
		int(cut_type),
		&exception);

	checkException(exception, *_dll_handle);
}


inline
void RawSample::save(
	std::ostream &binary_stream,
	ImageFormat format) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	save(binary_stream_wrap, format);
}

inline
void RawSample::save(
	pbio::stl_wraps::WrapOStream &binary_stream,
	ImageFormat format) const
{
	void* exception = NULL;

	_dll_handle->RawSample_save(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		int(format),
		&exception);

	checkException(exception, *_dll_handle);
}



inline
bool RawSample::hasOriginalImage() const
{
	void* exception = NULL;

	const bool result = _dll_handle->RawSample_hasOriginalImage(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
float RawSample::getScore() const
{
	void* exception = NULL;

	const float result = _dll_handle->RawSample_getScore(
			_impl,
			&exception);

	checkException(exception, *_dll_handle);

	return result;

}

inline
float RawSample::getFaceVisibilityScore() const
{
	void* exception = NULL;

	const float result = _dll_handle->RawSample_getFaceVisibilityScore(
			_impl,
			&exception);

	checkException(exception, *_dll_handle);

	return result;

}


inline
void RawSample::getOriginalImage(
	int &rows,
	int &cols,
	IRawImage::Format &format,
	void const* &data) const
{
	int32_t rows_, cols_, format_;

	void* exception = NULL;

	_dll_handle->RawSample_getOriginalImage(
		_impl,
		&rows_,
		&cols_,
		&format_,
		&data,
		&exception);

	checkException(exception, *_dll_handle);

	rows   = rows_;
	cols   = cols_;
	format = (IRawImage::Format) format_;
}


inline
RawSample::Ptr RawSample::downscaleToPreferredSize() const
{
	void* exception = NULL;

	pbio::facerec::RawSampleImpl* const raw_sampl_impl =
		_dll_handle->RawSample_downscaleToPreferredSize(
			_impl,
			&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, raw_sampl_impl);
}



inline
void RawSample::saveWithoutImage(
	pbio::stl_wraps::WrapOStream &binary_stream,
	const double space_translation_x,
	const double space_translation_y,
	const double space_scale) const
{
	void* exception = NULL;

	_dll_handle->RawSample_save_without_image(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapOStream::write_func,
		space_translation_x,
		space_translation_y,
		space_scale,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
void RawSample::saveWithoutImage(
	std::ostream &binary_stream,
	const double space_translation_x,
	const double space_translation_y,
	const double space_scale) const
{
	pbio::stl_wraps::WrapOStreamImpl binary_stream_wrap(binary_stream);

	saveWithoutImage(
		binary_stream_wrap,
		space_translation_x,
		space_translation_y,
		space_scale);
}

inline
void* RawSample::getPtr() const
{
	return _impl;
}


}  // pbio namespace



#endif  // __PBIO_API__PBIO__RAW_SAMPLE_H_
