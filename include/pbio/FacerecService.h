/**
	\file FacerecService.h
	\~English
	\brief FacerecService - Interface object for creating other interface objects.
	\~Russian
	\brief FacerecService - Интерфейсный объект для создания других интерфейсных объектов.
*/

#ifndef __PBIO_API__PBIO__FACEREC_SERVICE_H_
#define __PBIO_API__PBIO__FACEREC_SERVICE_H_


#include <istream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <facerec/libfacerec.h>

#ifdef ANDROID
#include <jni.h>
#endif

#include "stl_wraps_impls/WrapIStreamImpl.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"


#include "AgeGenderEstimator.h"
#include "CameraCalibrator.h"
#include "Capturer.h"
#include "ComplexObject.h"
#include "DepthLivenessEstimator.h"
#include "IRLivenessEstimator.h"
#include "ActiveLiveness.h"
#include "Liveness2DEstimator.h"
#include "FaceAttributesEstimator.h"
#include "EmotionsEstimator.h"
#include "Error.h"
#include "ExceptionCheck.h"
#include "FaceQualityEstimator.h"
#include "LivenessEstimator.h"
#include "QualityEstimator.h"
#include "RawSample.h"
#include "Recognizer.h"
#include "SmartPtr.h"
#include "Template.h"
#include "VideoWorker.h"
#include "StructStorage.h"
#include "Config.h"
#include "ProcessingUnit.h"
#include "Context.h"
#include "ProcessingBlock.h"

namespace pbio
{

/** \~English
	\brief Interface object for creating other interface objects.
	\~Russian
	\brief Интерфейсный объект для создания других интерфейсных объектов.
*/
class FacerecService : ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to FacerecService.
		\~Russian
		\brief  Псевдоним для типа умного указателя на FacerecService.
	*/
	typedef LightSmartPtr<FacerecService>::tPtr Ptr;

	typedef pbio::Config Config;

    /**
        \~English \brief Storing the license as a string variable.
        \~Russian \brief Хранение лицензии в виде строковой переменной.
    */
	struct License
	{
		/**
			\~English \brief Сontent of a license.
			\~Russian \brief Содержимое лицензии.
		*/
		const std::string license_body;

		License(std::string license_body) :	license_body(license_body) { }
	};

	/** \~English
		\brief License status.
		\~Russian
		\brief Статус лицензии.
	*/
	struct LicenseState
	{
		/**
			\~English \brief State of the counter assigned to a specified license element.
			\~Russian \brief Статус счетчика, назначенного определенному элементу лицензии.
		*/
		struct CounterState
		{
			/**
				\~English \brief Element name (equals to the tag name).
				\~Russian \brief Имя элемента (совпадает с именем тэга).
			*/
			std::string name;

			/**
				\~English \brief Maximum counter value (equals to the tag value).
				\~Russian \brief Максимальное значение счетчика (совпадает со значением тэга).
			*/
			int64_t max_value;

			/**
				\~English \brief Current value (number of used elements).
				\~Russian \brief Текущее значение (количество используемых элементов).
			*/
			int64_t value;
		};

		/**
			\~English \brief Online license flag.
			\~Russian \brief Флаг онлайн-лицензии.
		*/
		bool online;


		/**
			\~English \brief Android application ID. Available only on Android (tag <android_app_id>).
			\~Russian \brief ID Android приложения. Доступен только на Android (тэг <android_app_id>).
		*/
		std::string android_app_id;

		/**
			\~English \brief Serial number of the Android device. Only available on Android (tag <android_serial>).
			\~Russian \brief Серийный номер Android-устройства. Доступен только на Android (тэг <android_serial>).
		*/
		std::string android_serial;

		/**
			\~English \brief iOS application ID. Available only on iOS (tag <ios_app_id>).
			\~Russian \brief ID iOS приложения. Доступен только на iOS (тэг <ios_app_id>).
		*/
		std::string ios_app_id;

		/**
			\~English \brief Hardware signature (tag <Reg>).
			\~Russian \brief Ключ оборудования (тэг <Reg>).
		*/
		std::string hardware_reg;

		/**
			\~English \brief State of all license counters.
			\~Russian \brief Статус всех счетчиков лицензии.
		*/
		std::vector<CounterState> counters;
	};

	/**
		\~English
		\brief
			Initializes the facerec lib (can be called only once).

		\param[in]  dll_path
			An absolute or a relative path to libfacerec.so on Linux or facerec.dll on Windows.

		\param[in]  facerec_conf_dir
			An absolute or a relative path to the directory with config files (the 'face_sdk/conf/facerec/' directory).

		\param[in]  license_dir
			If you need to specify the directory containing the license file, then it's an absolute
			or a relative path to this directory, otherwise, it's an empty string.
			In case of an empty string, a license file is searched first in the 'face_sdk/license/' directory. If it's not found, it's searched in the 'face_sdk/conf/facerec/' directory.

		\return
			Created FacerecService object.

		\~Russian
		\brief
			Инициализировать работу с библиотекой libfacerec (нельзя вызывать более одного раза).

		\param[in]  dll_path
			Абсолютный или относительный путь до файла билиотеки - libfacerec.so на Linux или facerec.dll на Windows.

		\param[in]  facerec_conf_dir
			Абсолютный или относительный путь до каталога с конфигурационными файлами (каталог 'face_sdk/conf/facerec/' в дистрибутиве).

		\param[in]  license_dir
			Если требуется указать директорию с файлом лицензии, то абсолютный
			или относительный путь до этой директории, иначе - пустая строка.
			В случае пустой строки файл лицензии ищется сначала в каталоге 'face_sdk/license/', затем в каталоге 'face_sdk/conf/facerec/'.

		\return
			Созданный объект FacerecService.
	*/
	static FacerecService::Ptr createService(
		const std::string dll_path,
		std::string facerec_conf_dir,
		const std::string license_dir = std::string());

	/**
		\~English
		\brief
			Initializes the facerec lib with license content (can be called only once).

		\param[in]  dll_path
			An absolute or a relative path to libfacerec.so on Linux or facerec.dll on Windows.

		\param[in]  facerec_conf_dir
			An absolute or a relative path to the directory with config files (the 'face_sdk/conf/facerec/' directory).

		\param[in]  license
			Сontent of a license (see pbio::FacerecService::License)

		\return
			Created FacerecService object.

		\~Russian
		\brief
				Инициализировать работу с библиотекой libfacerec на основе содержимого лицензии (нельзя вызывать более одного раза).

		\param[in]  dll_path
			Абсолютный или относительный путь до файла билиотеки - libfacerec.so на Linux или facerec.dll на Windows.

		\param[in]  facerec_conf_dir
			Абсолютный или относительный путь до каталога с конфигурационными файлами (каталог 'face_sdk/conf/facerec/' в дистрибутиве).

		\param[in]  license
			Содержимое лицензии (см. pbio::FacerecService::License)

		\return
			Созданный объект FacerecService.
	*/
	static FacerecService::Ptr createService(
		const std::string dll_path,
		std::string facerec_conf_dir,
		const pbio::FacerecService::License license);

//! @cond IGNORED

	// internal usage only
	static FacerecService::Ptr createService(
		void* const ae_ptr,
		const std::string dll_path,
		std::string facerec_conf_dir,
		const std::string license,
		const bool is_license_dir);

//! @endcond


	/**
		\~English
		\brief
			Get version of face recognition library.
			Thread-safe.

		\return
			The version of face recognition library.

		\~Russian
		\brief
			Получить версию библиотеки.
			Потокобезопасный.

		\return
			Версия библиотеки.
	*/
	std::string getVersion() const;

	/**
		\~English
		\brief
			Force online license update.

		\~Russian
		\brief
			Принудительно обновить онлайн-лицензию.
	*/
	void forceOnlineLicenseUpdate() const;

	/**
		\~English
		\brief
			Creates a Capturer object.
			Type and features depend on the content of the configuration file.
			Thread-safe.

		\param[in]  config
			Сonfiguration file with optionally overridden parameters.

		\return
			Created Capturer object.

		\~Russian
		\brief
			Создать объект Capturer.
			Тип и возможности зависят от указанного конфигурационного файла.
			Потокобезопасный.

		\param[in]  config
			Конфигурационный файл с опционально переопределенными параметрами.

		\return
			Созданный объект Capturer.
	*/
	Capturer::Ptr createCapturer(const pbio::FacerecService::Config config) const;

	/**
		\~English
		\brief
			Creates a AgeGenderEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the config file.

		\return
			Created AgeGenderEstimator object.

		\~Russian
		\brief
			Создать объект AgeGenderEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект AgeGenderEstimators.
	*/
	AgeGenderEstimator::Ptr createAgeGenderEstimator(const std::string ini_file) const;

	/**
		\~English
		\brief
			Creates a  QualityEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the config file.

		\return
			Created QualityEstimator object.

		\~Russian
		\brief
			Создать объект QualityEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект QualityEstimator.
	*/
	QualityEstimator::Ptr createQualityEstimator(const std::string ini_file) const;

	/**
		\~English
		\brief
			Creates a FaceQualityEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the config file.

		\return
			Created FaceQualityEstimator object.

		\~Russian
		\brief
			Создать объект FaceQualityEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект FaceQualityEstimator.
	*/
	FaceQualityEstimator::Ptr createFaceQualityEstimator(const std::string ini_file) const;

	/**
		\~English
		\brief
			Creates a LivenessEstimator object.
			Thread-safe.

		\return
			Created LivenessEstimator object.

		\~Russian
		\brief
			Создать объект LivenessEstimator.
			Потокобезопасный.

		\return
			Созданный объект LivenessEstimator.
	*/
	LivenessEstimator::Ptr createLivenessEstimator() const;

	/**
		\~English
		\brief
			Creates a EmotionsEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the config file.

		\return
			Created EmotionsEstimator object.

		\~Russian
		\brief
			Создать объект EmotionsEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект EmotionsEstimator.
	*/
	EmotionsEstimator::Ptr createEmotionsEstimator(const std::string ini_file) const;

	/**
		\~English
		\brief
			Creates a Recognizer object.
			Thread-safe.

		\param[in]  ini_file
			Name of the config file.

		\param[in]  processing
			Flag to toggle the Recognizer::processing method in the created recognizer.

		\param[in]  matching
			Flag to toggle the Recognizer::verifyMatch method in the created recognizer.

		\param[in]  processing_less_memory_consumption
			Flag to toggle the Recognizer::processing member
			function optimizations that consume a lot of RAM when creating the recognizer (see the docs).

		\return
			Created Recognizer object.

		\~Russian
		\brief
			Создать объект Recognizer.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\param[in]  processing
			Флаг для включения / выключения метода Recognizer::processing в создаваемом разпознавателе.

		\param[in]  matching
			Флаг для включения / выключения метода Recognizer::verifyMatch в создаваемом разпознавателе.

		\param[in]  processing_less_memory_consumption
			Флаг для выключения оптимизаций метода Recognizer::processing,
			потребляющих много оперативной памяти при создании распознавателя (см. документацию).

		\return
			Созданный объект Recognizer.
	*/
	Recognizer::Ptr createRecognizer(
		const char* ini_file,
		const bool processing = true,
		const bool matching = true,
		const bool processing_less_memory_consumption = false) const;


	/**
		\~English
		\brief
			Creates a Recognizer object.
			Thread-safe.

		\param[in]  recognizer_config
			The Recognizer configuration file with optionally overridden parameters.

		\param[in]  processing
			Flag to toggle the Recognizer::processing method in the created recognizer.

		\param[in]  matching
			Flag to toggle the Recognizer::verifyMatch method in the created recognizer.

		\param[in]  processing_less_memory_consumption
			Flag to toggle the Recognizer::processing member
			function optimizations that consume a lot of RAM when creating the recognizer (see the docs).

		\return
			Created Recognizer object.

		\~Russian
		\brief
			Создать объект Recognizer.
			Потокобезопасный.

		\param[in]  recognizer_config
			Конфигурационный файл Recognizer с опционально переопределенными параметрами.

		\param[in]  processing
			Флаг для включения / выключения метода Recognizer::processing в создаваемом разпознавателе.

		\param[in]  matching
			Флаг для включения / выключения метода Recognizer::verifyMatch в создаваемом разпознавателе.

		\param[in]  processing_less_memory_consumption
			Флаг для выключения оптимизаций метода Recognizer::processing,
			потребляющих много оперативной памяти при создании распознавателя (см. документацию).

		\return
			Созданный объект Recognizer.
	*/
	Recognizer::Ptr createRecognizer(
		const pbio::FacerecService::Config recognizer_config,
		const bool processing = true,
		const bool matching = true,
		const bool processing_less_memory_consumption = false) const;

	/**
		\~English
		\brief
			Creates a VideoWorker object.
			Thread-safe.<br>
			When VideoWorker is created with <i>matching_thread=0</i> and <i>processing_thread=0</i>,
			then the standard Capturer license is used. <br>Depending on the settings, VideoWorker uses either the
			<i>VideoClient</i> license (face tracking on video streams) or the <i>VideoClientExt</i> license (face tracking, template
			creation and matching with the database).

		\param[in]  params
			Parameters of the VideoWorker constructor.

		\return
			Created VideoWorker object.

		\~Russian
		\brief
			Создать объект VideoWorker.
			Потокобезопасный.<br>
			Если при создании VideoWorker указаны параметры <i>matching_thread=0</i> и
			<i>processing_thread=0</i>, то потребляется обычная лицензия Capturer. <br>В зависимости от настроек,
			VideoWorker потребляет лицензию <i>VideoClient</i> (детекция лиц на видеопотоках)
			либо <i>VideoClientExt</i> (детекция лиц на видеопотоках, создание шаблонов и сравнение с базой).

		\param[in]  params
			Параметры конструктора VideoWorker.

		\return
			Созданный объект VideoWorker.
	*/
	VideoWorker::Ptr createVideoWorker(
		const pbio::VideoWorker::Params params) const;


	/**
		\~English
        \brief
		\param[in]  video_worker_config
			The VideoWorker configuration file with optionally overridden parameters.

		\param[in]  recognizer_ini_file
			Name of the config file for the Recognizer that will be used inside the VideoWorker.

		\param[in]  streams_count
			Number of video streams.

		\param[in]  processing_threads_count
			Number of threads for creating templates.

		\param[in]  matching_threads_count
			Number of threads for matching templates with the database.

		\return
			Created VideoWorker object.

		\~Russian
        \brief
		\param[in]  video_worker_config
			Конфигурационный файл VideoWorker с опционально переопределенными параметрами.

		\param[in]  recognizer_ini_file
			Имя конфигурационного файла для используемого распознавателя (Recognizer).

		\param[in]  streams_count
			Количество видеопотоков.

		\param[in]  processing_threads_count
			Количество потоков для создания шаблонов.

		\param[in]  matching_threads_count
			Количество потоков для сравнения шаблонов, созданных из видеопотоков, с базой.

		\return
			Созданный объект VideoWorker.
	*/
	VideoWorker::Ptr createVideoWorker(
		const pbio::FacerecService::Config video_worker_config,
		const char* recognizer_ini_file,
		const int streams_count,
		const int processing_threads_count,
		const int matching_threads_count) const;


	/**
		\~English
        \brief
		\param[in]  video_worker_config
			The VideoWorker configuration file with optionally overridden parameters.

		\param[in]  recognizer_config
			The Recognizer configuration file with optionally overridden parameters.

		\param[in]  streams_count
			Number of video streams.

		\param[in]  processing_threads_count
			Number of threads for creating templates.

		\param[in]  matching_threads_count
			Number of threads for matching templates with the database.

		\return
			Created VideoWorker object.

		\~Russian
        \brief
		\param[in]  video_worker_config
			Конфигурационный файл VideoWorker с опционально переопределенными параметрами.

		\param[in]  recognizer_config
			Конфигурационный файл Recognizer с опционально переопределенными параметрами.

		\param[in]  streams_count
			Количество видеопотоков.

		\param[in]  processing_threads_count
			Количество потоков для создания шаблонов.

		\param[in]  matching_threads_count
			Количество потоков для сравнения шаблонов, созданных из видеопотоков, с базой.

		\return
			Созданный объект VideoWorker.
	*/
	VideoWorker::Ptr createVideoWorker(
		const pbio::FacerecService::Config video_worker_config,
		const pbio::FacerecService::Config recognizer_config,
		const int streams_count,
		const int processing_threads_count,
		const int matching_threads_count) const;



	/**
		\~English
		\brief
			Loads a RawSample object that was saved with the RawSample::save member function.
			The format is platform-independent.
			Thread-safe.

		\param[in]  binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the set flag std::ios_base::binary.

		\return
			Loaded RawSample object.

		\~Russian
		\brief
			Загрузить объект RawSample, сохраненный с помощью метода RawSample::save.
			Формат платформонезависимый.
			Потокобезопасный.

		\param[in]  binary_stream
			Объект потока ввода.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.

		\return
			Загруженный объект RawSample.
	*/
	RawSample::Ptr loadRawSample(std::istream &binary_stream) const;

	/**
		\~English
		\brief
			Loads a RawSample object that was saved with the RawSample::save member function.
			The format is platform-independent.
			Thread-safe.

		\param[in]  binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the set flag std::ios_base::binary.

		\return
			Loaded RawSample object.

		\~Russian
		\brief
			Загрузить объект RawSample, сохраненный с помощью метода RawSample::save.
			Формат платформонезависимый.
			Потокобезопасный.

		\param[in]  binary_stream
			Объект потока ввода.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.

		\return
			Загруженный объект RawSample.
	*/
	RawSample::Ptr loadRawSample(pbio::stl_wraps::WrapIStream &binary_stream) const;


	/**
		\~English
		\brief
			Loads a RawSample object that was saved with the RawSample::saveWithoutImage member function.
			If you transformed the image in some way, you can set the parameters of coordinate space transformation:
			(x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
			that will be applied to the information abour face position.
			Note that you must set the same transformation only once - either during serialization or during deserialization,
			otherwise transformation will be applied twice, which is wrong.
			The format is platform-independent.
			Thread-safe.

		\param[in]  binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the set flag std::ios_base::binary.

		\param[in]  image
			Image.

		\param[in]  space_translation_x
			Coordinate space transform parameter.

		\param[in]  space_translation_y
			Coordinate space transform parameter.

		\param[in]  space_scale
			Coordinate space transform parameter.

		\return
			Loaded RawSample object.

		\~Russian
		\brief
			Загрузить объект RawSample, сохраненный с помощью метода RawSample::saveWithoutImage.
			В случае, если над изображением была выполнена трансформация,
			вы можете указать параметры преобразования пространства координат:
			(x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
			которое будет применено к информации о позиции лица.
			Заметьте, что параметры одного и того же преобразования нужно передавать только один раз - либо
			при сериализации, либо при десериализации, иначе преобразование будет применено дважды, что неверно.
			Формат платформонезависимый.
			Потокобезопасный.

		\param[in]  binary_stream
			Объект потока ввода.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  image
			Изображение.

		\param[in]  space_translation_x
			Параметр преобразования пространства координат.

		\param[in]  space_translation_y
			Параметр преобразования пространства координат.

		\param[in]  space_scale
			Параметр преобразования пространства координат.

		\return
			Загруженный объект RawSample.
	*/
	RawSample::Ptr loadRawSampleWithoutImage(
		pbio::stl_wraps::WrapIStream &binary_stream,
		const RawImage image,
		const double space_translation_x = 0,
		const double space_translation_y = 0,
		const double space_scale = 1) const;

	/**
		\copydoc loadRawSampleWithoutImage
	*/
	RawSample::Ptr loadRawSampleWithoutImage(
		std::istream &binary_stream,
		const RawImage image,
		const double space_translation_x = 0,
		const double space_translation_y = 0,
		const double space_scale = 1) const;

	/**
		\copybrief loadRawSampleWithoutImage

		\~English

		\param[in]  binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the set flag std::ios_base::binary.

		\param[in]  image_data
			Pointer to the encoded image with 8 or 24 bits per pixel.
			Supported formats are JPEG, PNG, TIF and BMP.

		\param[in]  image_data_size
			Image data buffer size in bytes

		\param[in]  space_translation_x
			Coordinate space transform parameter.

		\param[in]  space_translation_y
			Coordinate space transform parameter.

		\param[in]  space_scale
			Coordinate space transform parameter.

		\return
			Loaded RawSample object.

		\~Russian

		\param[in]  binary_stream
			Объект потока.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.

		\param[in]  image_data
			Указатель на закодированное изображение с 8 или 24 битами на пиксел.
			Поддерживаемые форматы: JPEG, PNG, TIF и BMP.

		\param[in]  image_data_size
			Размер буфера image_data в байтах.

		\param[in]  space_translation_x
			Параметр преобразования пространства координат.

		\param[in]  space_translation_y
			Параметр преобразования пространства координат.

		\param[in]  space_scale
			Параметр преобразования пространства координат.

		\return
			Загруженный объект RawSample.
	*/
	RawSample::Ptr loadRawSampleWithoutImage(
		pbio::stl_wraps::WrapIStream &binary_stream,
		const unsigned char *image_data,
		int image_data_size,
		const double space_translation_x = 0,
		const double space_translation_y = 0,
		const double space_scale = 1) const;

	/**
		\copydoc loadRawSampleWithoutImage(pbio::stl_wraps::WrapIStream&,
			const unsigned char*,int,double,double,double)const
	*/
	RawSample::Ptr loadRawSampleWithoutImage(
		std::istream &binary_stream,
		const unsigned char *image_data,
		int image_data_size,
		const double space_translation_x = 0,
		const double space_translation_y = 0,
		const double space_scale = 1) const;


	/**
		\~English
		\brief
			Creates a CameraCalibrator object.
			Thread-safe.

		\return
			Created CameraCalibrator object.

		\~Russian
		\brief
			Создать объект CameraCalibrator.
			Потокобезопасный.

		\return
			Созданный объект CameraCalibrator.
	*/
	CameraCalibrator::Ptr createCameraCalibrator() const;


	/**
		\~English
		\brief
			Creates a DepthLivenessEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the configuration file.

		\return
			Created DepthLivenessEstimator object.

		\~Russian
		\brief
			Создать объект DepthLivenessEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект DepthLivenessEstimator.
	*/
	DepthLivenessEstimator::Ptr createDepthLivenessEstimator(
		const std::string ini_file) const;


	/**
		\~English
		\brief
			Creates a DepthLivenessEstimator object.
			Thread-safe.

		\param[in]  config
			Сonfiguration file with optionally overridden parameters.

		\return
			Created DepthLivenessEstimator object.

		\~Russian
		\brief
			Создать объект DepthLivenessEstimator.
			Потокобезопасный.

		\param[in]  config
			Конфигурационный файл с опционально переопределенными параметрами.

		\return
			Созданный объект DepthLivenessEstimator.
	*/
	DepthLivenessEstimator::Ptr createDepthLivenessEstimator(
		const pbio::FacerecService::Config config) const;


	/**
		\~English
		\brief
			Creates an IRLivenessEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the configuration file.

		\return
			Created IRLivenessEstimator object.

		\~Russian
		\brief
			Создать объект IRLivenessEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект IRLivenessEstimator.
	*/
	IRLivenessEstimator::Ptr createIRLivenessEstimator(
		const std::string ini_file) const;


	/**
		\~English
		\brief
			Creates an IRLivenessEstimator object.
			Thread-safe.

		\param[in]  config
			Сonfiguration file with optionally overridden parameters.

		\return
			Created IRLivenessEstimator object.

		\~Russian
		\brief
			Создать объект IRLivenessEstimator.
			Потокобезопасный.

		\param[in]  config
			Конфигурационный файл с опционально переопределенными параметрами.

		\return
			Созданный объект IRLivenessEstimator.
	*/
	IRLivenessEstimator::Ptr createIRLivenessEstimator(
		const pbio::FacerecService::Config config) const;


	/**
		\~English
		\brief
			Creates an Liveness2DEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the configuration file.

		\return
			Created Liveness2DEstimator object.

		\~Russian
		\brief
			Создать объект Liveness2DEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект Liveness2DEstimator.
	*/
	Liveness2DEstimator::Ptr createLiveness2DEstimator(
		const std::string ini_file) const;


	/**
		\~English
		\brief
			Creates an Liveness2DEstimator object.
			Thread-safe.

		\param[in]  config
			Сonfiguration file with optionally overridden parameters.

		\return
			Created Liveness2DEstimator object.

		\~Russian
		\brief
			Создать объект Liveness2DEstimator.
			Потокобезопасный.

		\param[in]  config
			Конфигурационный файл с опционально переопределенными параметрами.

		\return
			Созданный объект Liveness2DEstimator.
	*/
	Liveness2DEstimator::Ptr createLiveness2DEstimator(
		const pbio::FacerecService::Config config) const;

	/**
		\~English
		\brief
			Creates an FaceAttributesEstimator object.
			Thread-safe.

		\param[in]  ini_file
			Name of the configuration file.

		\return
			Created FaceAttributesEstimator object.

		\~Russian
		\brief
			Создать объект FaceAttributesEstimator.
			Потокобезопасный.

		\param[in]  ini_file
			Имя конфигурационного файла.

		\return
			Созданный объект FaceAttributesEstimator.
	*/
	FaceAttributesEstimator::Ptr createFaceAttributesEstimator(
		const std::string ini_file) const;

	//! @cond IGNORED

	ProcessingUnit::Ptr createProcessingUnit(
		const int block_type,
		const char* serializedConfig) const;

	Context createContext() const;

	ProcessingBlock createProcessingBlock(const Context& config) const;

	//! @endcond

	/**
		\~English
		\brief
			Get the license state.
			Thread-safe.

		\return
			License state.

		\~Russian
		\brief
			Получить статус лицензии.
			Потокобезопасный.

		\return
			Статус лицензии.
	*/
	LicenseState getLicenseState() const;


	/**
		\~English
		\brief
			Enalbe keeping algorithms data in memory even if no owners left.
			This will speedup repeated algorithms initialization.
			Thread-safe.

		\~Russian
		\brief
			Включить удержание данных для алгоритмов в памяти, даже если не осталось владельцев.
			Это ускорит повторную инициализацию алгоритмов.
			Потокобезопасный.
	*/
	void keepAlgorithmsCache() const;

	/**
		\~English
		\brief
			Disable keeping algorithms data in memory.
			Thread-safe.

		\~Russian
		\brief
			Отключить удержание данных для алгоритмов в памяти.
			Потокобезопасный.
	*/
	void freeAlgorithmsCache() const;


	/**
		\~English
		\brief
			Create InternalImageBuffer for specified image size and format.
			Thread-safe.
			Always create new InternalImageBuffer for every image or video frame.
			Never change image data of InternalImageBuffer after first use.

		\param[in]  width
			Image width.

		\param[in]  height
			Image height.

		\param[in]  format
			Image format.

		\return
			Created InternalImageBuffer.

		\~Russian
		\brief
			Создать InternalImageBuffer для указанного размера и формата изображения.
			Потокобезопасный.
			Всегда создавайте новый InternalImageBuffer для каждого изображения или кадра видео.
			Никогда не изменяйте данные изображения после первого использования.

		\param[in]  width
			Ширина изображения.

		\param[in]  height
			Высота изображения.

		\param[in]  format
			Формат изображения.

		\return
			Созданный объект InternalImageBuffer.
	*/
	InternalImageBuffer::Ptr createInternalImageBuffer(
		const int width,
		const int height,
		const RawImage::Format format);


#ifdef ANDROID
	/**
		\~English
		\brief
			Create InternalImageBuffer and fill with data from given android.media.Image
			java object with image format YUV_420_888.
			Thread-safe.
			Available only on Android.
			Always create new InternalImageBuffer for every image or video frame.
			Never change image data of InternalImageBuffer after first use.

		\param[in]  env
			Pointer to the JNI function table - JNIEnv.

		\param[in]  android_media_image
			android.media.Image java object with image format YUV_420_888.

		\return
			Created InternalImageBuffer with data copied from given image.
			Format will be either RawImage::FORMAT_YUV_NV21 of RawImage::FORMAT_YUV_NV12.

		\~Russian
		\brief
			Создать InternalImageBuffer и заполнить данные изображения из поданного java объекта
			android.media.Image с форматом изображения YUV_420_888.
			Потокобезопасный.
			Доступен только на Android.
			Всегда создавайте новый InternalImageBuffer для каждого изображения или кадра видео.
			Никогда не изменяйте данные изображения после первого использования.

		\param[in]  env
			Указатель на таблицу JNI функций - JNIEnv.

		\param[in]  android_media_image
			android.media.Image java объект с форматом изображения YUV_420_888.

		\return
			Созданный объект InternalImageBuffer с данными скопированными из поданного изображения.
			Формат будет либо RawImage::FORMAT_YUV_NV21, либо RawImage::FORMAT_YUV_NV12.
	*/
	InternalImageBuffer::Ptr createInternalImageBuffer(
		JNIEnv *env,
		jobject android_media_image);
#endif  // ifdef ANDROID


	/**
		\~English
		\brief
			Convert input image to android.graphics.Bitmap.Config.ARGB_8888 format.
			Input must be in YUV_NV21 of YUV_NV12 format.
			Note: actual bytes order is BGRA, it looks like ARGB_8888
			name use little-endian 32-bit integer notation.
			Thread-safe.

		\param[in]  image
			Image in YUV_NV21 or YUV_NV12 format.

		\param[in]  downscale_x2
			Downscale image during conversion, so the result image
			size will be image.width/2 x image.height/2.

		\param[out]  result_buffer
			Data buffer to store converted result, with size image.width * image.height bytes if
			downscale_x2, or 4 * image.width * image.height bytes otherwise.

        \~Russian
		\brief
			Конвертировать входное изображение в формат android.graphics.Bitmap.Config.ARGB_8888.
			Формат входного изображения должен быть YUV_NV21 или YUV_NV12.
			Замечание: в действительности порядок байт BGRA, а название ARGB_8888 похоже
			предполагает нотацию 32-битного little-endian целого числа.
			Потокобезопасный.

		\param[in]  image
			Изображение в формате YUV_NV21 или YUV_NV12.

		\param[in]  downscale_x2
			Уменшить изображение в процессе конвертации, так что
			результат будет размера image.width/2 x image.height/2.

		\param[out]  result_buffer
			Буффер для сохранения результата размера image.width * image.height байт, если
			downscale_x2, либо размера 4 * image.width * image.height байт в противном случае.

	*/
	void convertYUV2ARGB(
		const RawImage image,
		const bool downscale_x2,
		void* const result_buffer);


	void convertYUV2RGB(
		const RawImage image,
		const bool downscale_x2,
		const int base_angle,
		void* const result_buffer);


private:

	const std::string _facerec_conf_dir;

	friend class object_with_ref_counter<FacerecService>;
protected:
	FacerecService(
		const DHPtr &dll_handle,
		const std::string &facerec_conf_dir,
		void* impl);
};

}  // pbio namespace




////////////////////////
/////IMPLEMENTATION/////
////////////////////////


namespace pbio
{


// static
inline
FacerecService::Ptr FacerecService::createService(
	const std::string dll_path,
	std::string facerec_conf_dir,
	const std::string license_dir)
{
	return createService(NULL, dll_path, facerec_conf_dir, license_dir, true);
}

// static
inline
FacerecService::Ptr FacerecService::createService(
	const std::string dll_path,
	std::string facerec_conf_dir,
	const pbio::FacerecService::License license)
{
	return createService(NULL, dll_path, facerec_conf_dir, license.license_body, false);
}

// static
inline
FacerecService::Ptr FacerecService::createService(
	void* const ae_ptr,
	const std::string dll_path,
	std::string facerec_conf_dir,
	const std::string license,
	const bool is_license_dir)
{

#ifdef __STATIC_LIBFACEREC_BUILD__
	(void) dll_path;

	const DHPtr dll_handle( DHPtr::make() );
#else
	const DHPtr dll_handle( DHPtr::make(dll_path.c_str()) );
#endif

	if(facerec_conf_dir.empty())
	{
		facerec_conf_dir = "./";
	}
	else if(facerec_conf_dir[facerec_conf_dir.length() - 1] != '/')
	{
		facerec_conf_dir += '/';
	}

	void* exception = NULL;

	void* the_impl;

	if (is_license_dir)
	{
		the_impl = dll_handle->FacerecService_constructor3(
			ae_ptr,
			facerec_conf_dir.c_str(),
			license.empty() ?
				NULL :
				license.c_str(),
			dll_path.c_str(),
			&exception);
	}else
	{
		the_impl = dll_handle->FacerecService_constructor5(
			ae_ptr,
			facerec_conf_dir.c_str(),
			license.c_str(),
			dll_path.c_str(),
			&exception);
	}

	checkException(exception, *dll_handle);

	return FacerecService::Ptr::make(
		dll_handle,
		facerec_conf_dir,
		the_impl);
}


inline
FacerecService::FacerecService(
	const DHPtr &dll_handle,
	const std::string &facerec_conf_dir,
	void* impl):
ComplexObject(dll_handle, impl),
_facerec_conf_dir(facerec_conf_dir)
{
	std::string lib_version = getVersion();

	if (LIBFACEREC_VERSION != lib_version)
		std::cerr << "WARNING: The version in the header does not match the version in the library. Header version: "
			<< LIBFACEREC_VERSION << ", library version: " << lib_version << std::endl;
}


inline
std::string FacerecService::getVersion() const
{
	std::ostringstream version_stream;
	pbio::stl_wraps::WrapOStreamImpl version_stream_wrap(version_stream);

	void* exception = NULL;

	_dll_handle->get_version(
		&version_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	return version_stream.str();
}


inline
void FacerecService::forceOnlineLicenseUpdate() const
{
	void* exception = NULL;

	_dll_handle->FacerecService_forceOnlineLicenseUpdate(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
Capturer::Ptr FacerecService::createCapturer(
	const pbio::FacerecService::Config config) const
{
	const std::string file_path = _facerec_conf_dir + config.config_filepath;

	std::vector<char const*> overridden_keys;
	std::vector<double> overridden_values;

	config.prepare(overridden_keys, overridden_values);

	void* exception = NULL;

	pbio::facerec::CapturerImpl* const capturer_impl =
		_dll_handle->FacerecService_createCapturerE(
			_impl,
			file_path.c_str(),
			overridden_keys.size(),
			overridden_keys.empty() ? NULL : &(overridden_keys[0]),
			overridden_values.empty() ? NULL : &(overridden_values[0]),
			&exception);

	checkException(exception, *_dll_handle);

	return Capturer::Ptr::make(_dll_handle, capturer_impl);
}

inline
AgeGenderEstimator::Ptr FacerecService::createAgeGenderEstimator(const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::AgeGenderEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createAgeGenderEstimator(
			_impl,
			file_path.c_str(),
			&exception);

	checkException(exception, *_dll_handle);

	return AgeGenderEstimator::Ptr::make(_dll_handle, the_impl);
}


inline
QualityEstimator::Ptr FacerecService::createQualityEstimator(const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::QualityEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createQualityEstimator(
			_impl,
			file_path.c_str(),
			&exception);

	checkException(exception, *_dll_handle);

	return QualityEstimator::Ptr::make(_dll_handle, the_impl);
}


inline
FaceQualityEstimator::Ptr FacerecService::createFaceQualityEstimator(const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::FaceQualityEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createFaceQualityEstimator(
			_impl,
			file_path.c_str(),
			&exception);

	checkException(exception, *_dll_handle);

	return FaceQualityEstimator::Ptr::make(_dll_handle, the_impl);
}

inline
LivenessEstimator::Ptr FacerecService::createLivenessEstimator() const
{
	void* exception = NULL;

	pbio::facerec::LivenessEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createLivenessEstimator(
			_impl,
			&exception);

	checkException(exception, *_dll_handle);

	return LivenessEstimator::Ptr::make(_dll_handle, the_impl);
}


inline
EmotionsEstimator::Ptr FacerecService::createEmotionsEstimator(
	const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::EmotionsEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createEmotionsEstimator(
			_impl,
			file_path.c_str(),
			&exception);

	checkException(exception, *_dll_handle);

	return EmotionsEstimator::Ptr::make(_dll_handle, the_impl);
}


inline
Recognizer::Ptr FacerecService::createRecognizer(
	const char* ini_file,
	const bool processing,
	const bool matching,
	const bool processing_less_memory_consumption) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::RecognizerImpl* const recognizer_impl =
		_dll_handle->FacerecService_createRecognizer2(
			_impl,
			file_path.c_str(),
			0,
			NULL,
			NULL,
			(int) processing,
			(int) matching,
			(int) processing_less_memory_consumption,
			&exception);

	checkException(exception, *_dll_handle);

	return Recognizer::Ptr::make(_dll_handle, recognizer_impl);
}


inline
Recognizer::Ptr FacerecService::createRecognizer(
	const pbio::FacerecService::Config recognizer_config,
	const bool processing,
	const bool matching,
	const bool processing_less_memory_consumption) const
{
	const std::string file_path = _facerec_conf_dir + recognizer_config.config_filepath;

	std::vector<char const*> overridden_keys;
	std::vector<double> overridden_values;

	recognizer_config.prepare(overridden_keys, overridden_values);

	void* exception = NULL;

	pbio::facerec::RecognizerImpl* const recognizer_impl =
		_dll_handle->FacerecService_createRecognizer2(
			_impl,
			file_path.c_str(),
			overridden_keys.size(),
			overridden_keys.empty() ? NULL : &(overridden_keys[0]),
			overridden_values.empty() ? NULL : &(overridden_values[0]),
			(int) processing,
			(int) matching,
			(int) processing_less_memory_consumption,
			&exception);

	checkException(exception, *_dll_handle);

	return Recognizer::Ptr::make(_dll_handle, recognizer_impl);
}



inline
VideoWorker::Ptr FacerecService::createVideoWorker(
	const pbio::FacerecService::Config video_worker_ini_file,
	const char* recognizer_ini_file,
	const int streams_count,
	const int processing_threads_count,
	const int matching_threads_count) const
{
	return createVideoWorker(
		VideoWorker::Params()
			.video_worker_config(video_worker_ini_file)
			.recognizer_ini_file(recognizer_ini_file)
			.streams_count(streams_count)
			.processing_threads_count(processing_threads_count)
			.matching_threads_count(matching_threads_count)
		);
}

inline
VideoWorker::Ptr FacerecService::createVideoWorker(
	const pbio::FacerecService::Config video_worker_ini_file,
	const pbio::FacerecService::Config recognizer_config,
	const int streams_count,
	const int processing_threads_count,
	const int matching_threads_count) const
{
	return createVideoWorker(
		VideoWorker::Params()
			.video_worker_config(video_worker_ini_file)
			.recognizer_config(recognizer_config)
			.streams_count(streams_count)
			.processing_threads_count(processing_threads_count)
			.matching_threads_count(matching_threads_count)
		);
}

inline
VideoWorker::Ptr FacerecService::createVideoWorker(
	const VideoWorker::Params params) const
{
	std::vector<char const*> vw_overridden_keys;
	std::vector<double> vw_overridden_values;

	params._video_worker_config.prepare(vw_overridden_keys, vw_overridden_values);
	std::vector<std::string> extra_over_params;

	if(!params._active_liveness_checks_order.empty()){
		bool is_unique = true;
		for (auto it = params._active_liveness_checks_order.begin(); it != params._active_liveness_checks_order.end(); ++it)
			if (std::find(it + 1, params._active_liveness_checks_order.end(), *it) != params._active_liveness_checks_order.end()) {
				is_unique = false;
				break;
			}
		if(!is_unique)
			throw pbio::Error(0x3302330e,
				"Error 0x3302330e: Set a unique order of `active_liveness_checks_order` for Active Liveness.");
		for (size_t i = 0; i < params._active_liveness_checks_order.size(); i++){
			ActiveLiveness::CheckType check = params._active_liveness_checks_order[i];
			std::string check_str = ActiveLiveness::CheckTypeToString(check);
			extra_over_params.push_back("active_liveness.check_" + check_str);
			vw_overridden_keys.push_back(extra_over_params.back().c_str());
			vw_overridden_values.push_back(-(double)(i+1));
		}
	}


	if (!params._recognizer_ini_file.empty() && !params._recognizer_config.config_filepath.empty())
		throw pbio::Error(0xb3fe4d07, "Error: 0xed877a99 You must use either recognizer_config or recognizer_ini_file.");

	pbio::FacerecService::Config recognizer_config =
		params._recognizer_ini_file.empty() ?
			params._recognizer_config :
			pbio::FacerecService::Config(params._recognizer_ini_file);

	std::vector<char const*> rec_overridden_keys;
	std::vector<double> rec_overridden_values;

	recognizer_config.prepare(rec_overridden_keys, rec_overridden_values);

	void* exception = NULL;

	pbio::facerec::VideoWorkerImpl* const vw_impl =
		_dll_handle->FacerecService_createVideoWorker_sti_age_gender_emotions(
			_impl,

			VideoWorker::STrackingCallback,
			VideoWorker::STemplateCreatedCallback,
			VideoWorker::SMatchFoundCallback,
			VideoWorker::STrackingLostCallback,
			VideoWorker::SStiPersonOutdatedCallback,

			(_facerec_conf_dir + params._video_worker_config.config_filepath).c_str(),
			vw_overridden_keys.size(),
			vw_overridden_keys.empty() ? NULL : &(vw_overridden_keys[0]),
			vw_overridden_values.empty() ? NULL : &(vw_overridden_values[0]),

			(_facerec_conf_dir + recognizer_config.config_filepath).c_str(),
			rec_overridden_keys.size(),
			rec_overridden_keys.empty() ? NULL : &(rec_overridden_keys[0]),
			rec_overridden_values.empty() ? NULL : &(rec_overridden_values[0]),

			params._streams_count,
			params._processing_threads_count,
			params._matching_threads_count,
			params._short_time_identification_enabled,
			params._short_time_identification_distance_threshold,
			params._short_time_identification_outdate_time_seconds,
			params._age_gender_estimation_threads_count,
			params._emotions_estimation_threads_count,
			&exception);

	checkException(exception, *_dll_handle);

	return VideoWorker::Ptr::make(_dll_handle, vw_impl);
}


inline
RawSample::Ptr FacerecService::loadRawSample(std::istream &binary_stream) const
{
	pbio::stl_wraps::WrapIStreamImpl binary_stream_wrap(binary_stream);

	return loadRawSample(binary_stream_wrap);
}

inline
RawSample::Ptr FacerecService::loadRawSample(pbio::stl_wraps::WrapIStream &binary_stream) const
{
	void* exception = NULL;

	pbio::facerec::RawSampleImpl* const raw_sampl_impl =
		_dll_handle->FacerecService_loadRawSample(
			_impl,
			&binary_stream,
			pbio::stl_wraps::WrapIStream::read_func,
			&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, raw_sampl_impl);
}

inline
RawSample::Ptr FacerecService::loadRawSampleWithoutImage(
		pbio::stl_wraps::WrapIStream &binary_stream,
		const RawImage image,
		const double space_translation_x,
		const double space_translation_y,
		const double space_scale) const
{
	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	pbio::facerec::RawSampleImpl* const raw_sampl_impl =
		_dll_handle->FacerecService_loadRawSampleWithoutImage_raw_image(
			_impl,
			&binary_stream,
			pbio::stl_wraps::WrapIStream::read_func,

			cdata.data,
			cdata.width,
			cdata.height,
			cdata.format,
			cdata.with_crop,
			cdata.crop_info_offset_x,
			cdata.crop_info_offset_y,
			cdata.crop_info_data_image_width,
			cdata.crop_info_data_image_height,

			space_translation_x,
			space_translation_y,
			space_scale,

			&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, raw_sampl_impl);
}

inline
RawSample::Ptr FacerecService::loadRawSampleWithoutImage(
		std::istream &binary_stream,
		const RawImage image,
		const double space_translation_x,
		const double space_translation_y,
		const double space_scale) const
{
	pbio::stl_wraps::WrapIStreamImpl binary_stream_wrap(binary_stream);

	return loadRawSampleWithoutImage(
		binary_stream_wrap,
		image,
		space_translation_x,
		space_translation_y,
		space_scale);
}

inline
RawSample::Ptr FacerecService::loadRawSampleWithoutImage(
		pbio::stl_wraps::WrapIStream &binary_stream,
		const unsigned char *image_data,
		int image_data_size,
		const double space_translation_x,
		const double space_translation_y,
		const double space_scale) const
{
	void* exception = NULL;

	pbio::facerec::RawSampleImpl* const raw_sampl_impl =
		_dll_handle->FacerecService_loadRawSampleWithoutImage_encoded_image(
			_impl,
			&binary_stream,
			pbio::stl_wraps::WrapIStream::read_func,

			image_data,
			image_data_size,

			space_translation_x,
			space_translation_y,
			space_scale,

			&exception);

	checkException(exception, *_dll_handle);

	return RawSample::Ptr::make(_dll_handle, raw_sampl_impl);
}

inline
RawSample::Ptr FacerecService::loadRawSampleWithoutImage(
		std::istream &binary_stream,
		const unsigned char *image_data,
		int image_data_size,
		const double space_translation_x,
		const double space_translation_y,
		const double space_scale) const
{
	pbio::stl_wraps::WrapIStreamImpl binary_stream_wrap(binary_stream);


	return loadRawSampleWithoutImage(
		binary_stream_wrap,
		image_data,
		image_data_size,
		space_translation_x,
		space_translation_y,
		space_scale);
}


inline
CameraCalibrator::Ptr FacerecService::createCameraCalibrator() const
{
	void* exception = NULL;

	pbio::facerec::CameraCalibratorImpl* const calibrator_impl =
		_dll_handle->FacerecService_createCameraCalibrator(
			_impl,
			&exception);

	checkException(exception, *_dll_handle);

	return CameraCalibrator::Ptr::make(_dll_handle, calibrator_impl);
}



inline
DepthLivenessEstimator::Ptr FacerecService::createDepthLivenessEstimator(
	const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::DepthLivenessEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createDepthLivenessEstimatorE(
			_impl,
			file_path.c_str(),
			0,     // overridden keys size
			NULL,  // overridden keys
			NULL,  // overriden values
			&exception);

	checkException(exception, *_dll_handle);

	return DepthLivenessEstimator::Ptr::make(_dll_handle, the_impl);
}


inline
DepthLivenessEstimator::Ptr FacerecService::createDepthLivenessEstimator(
	const pbio::FacerecService::Config config) const
{
	const std::string file_path = _facerec_conf_dir + config.config_filepath;

	std::vector<char const*> overridden_keys;
	std::vector<double> overridden_values;

	config.prepare(overridden_keys, overridden_values);

	void* exception = NULL;

	pbio::facerec::DepthLivenessEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createDepthLivenessEstimatorE(
			_impl,
			file_path.c_str(),
			overridden_keys.size(),
			overridden_keys.empty() ? NULL : &(overridden_keys[0]),
			overridden_values.empty() ? NULL : &(overridden_values[0]),
			&exception);

	checkException(exception, *_dll_handle);

	return DepthLivenessEstimator::Ptr::make(_dll_handle, the_impl);
}

inline
IRLivenessEstimator::Ptr FacerecService::createIRLivenessEstimator(
	const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::IRLivenessEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createIRLivenessEstimatorE(
			_impl,
			file_path.c_str(),
			0,     // overridden keys size
			NULL,  // overridden keys
			NULL,  // overriden values
			&exception);

	checkException(exception, *_dll_handle);

	return IRLivenessEstimator::Ptr::make(_dll_handle, the_impl);
}

inline
IRLivenessEstimator::Ptr FacerecService::createIRLivenessEstimator(
	const pbio::FacerecService::Config config) const
{
	const std::string file_path = _facerec_conf_dir + config.config_filepath;

	std::vector<char const*> overridden_keys;
	std::vector<double> overridden_values;

	config.prepare(overridden_keys, overridden_values);

	void* exception = NULL;

	pbio::facerec::IRLivenessEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createIRLivenessEstimatorE(
			_impl,
			file_path.c_str(),
			overridden_keys.size(),
			overridden_keys.empty() ? NULL : &(overridden_keys[0]),
			overridden_values.empty() ? NULL : &(overridden_values[0]),
			&exception);

	checkException(exception, *_dll_handle);

	return IRLivenessEstimator::Ptr::make(_dll_handle, the_impl);
}

inline
Liveness2DEstimator::Ptr FacerecService::createLiveness2DEstimator(
	const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::Liveness2DEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createLiveness2DEstimatorE(
			_impl,
			file_path.c_str(),
			0,     // overridden keys size
			NULL,  // overridden keys
			NULL,  // overriden values
			&exception);

	checkException(exception, *_dll_handle);

	return Liveness2DEstimator::Ptr::make(_dll_handle, the_impl);
}

inline
Liveness2DEstimator::Ptr FacerecService::createLiveness2DEstimator(
	const pbio::FacerecService::Config config) const
{
	const std::string file_path = _facerec_conf_dir + config.config_filepath;

	std::vector<char const*> overridden_keys;
	std::vector<double> overridden_values;

	config.prepare(overridden_keys, overridden_values);

	void* exception = NULL;

	pbio::facerec::Liveness2DEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createLiveness2DEstimatorE(
			_impl,
			file_path.c_str(),
			overridden_keys.size(),
			overridden_keys.empty() ? NULL : &(overridden_keys[0]),
			overridden_values.empty() ? NULL : &(overridden_values[0]),
			&exception);

	checkException(exception, *_dll_handle);

	return Liveness2DEstimator::Ptr::make(_dll_handle, the_impl);
}

inline
FaceAttributesEstimator::Ptr FacerecService::createFaceAttributesEstimator(
	const std::string ini_file) const
{
	const std::string file_path = _facerec_conf_dir + ini_file;

	void* exception = NULL;

	pbio::facerec::FaceAttributesEstimatorImpl* const the_impl =
		_dll_handle->FacerecService_createFaceAttributesEstimator(
			_impl,
			file_path.c_str(),
			0,     // overridden keys size
			NULL,  // overridden keys
			NULL,  // overriden values
			&exception);

	checkException(exception, *_dll_handle);

	return FaceAttributesEstimator::Ptr::make(_dll_handle, the_impl);
}

//! @cond IGNORED

inline
ProcessingUnit::Ptr FacerecService::createProcessingUnit(
	const int block_type,
	const char* serializedConfig) const
{
	return ProcessingUnit::Ptr::make(_dll_handle, block_type, serializedConfig);
}

inline Context FacerecService::createContext() const
{
	return Context(_dll_handle);
}

inline ProcessingBlock FacerecService::createProcessingBlock(const Context& config) const
{
	return ProcessingBlock(_impl, _dll_handle, config);
}

//! @endcond

inline
FacerecService::LicenseState FacerecService::getLicenseState() const
{
	void* exception = NULL;

	void* struct_storage_impl = _dll_handle->FacerecService_getLicenseState(_impl, &exception);

	checkException(exception, *_dll_handle);

	const StructStorage struct_storage(_dll_handle, struct_storage_impl);

	LicenseState result;

	result.online = struct_storage.get_int64(StructStorageFields::license_state_online_t);

	result.android_app_id = (char const*) struct_storage.get_pointer(StructStorageFields::license_state_android_app_id_t);
	result.android_serial = (char const*) struct_storage.get_pointer(StructStorageFields::license_state_android_serial_t);
	result.ios_app_id     = (char const*) struct_storage.get_pointer(StructStorageFields::license_state_ios_app_id_t);
	result.hardware_reg   = (char const*) struct_storage.get_pointer(StructStorageFields::license_state_hardware_reg_t);

	result.counters.resize( struct_storage.get_int64(
		StructStorageFields::license_state_licenses_count_t) );

	for(size_t i = 0; i < result.counters.size(); ++i)
	{
		result.counters[i].name = (char const*) struct_storage.get_pointer(
			(i << 16) | size_t(StructStorageFields::license_state_licenses_names_int16_t) );

		result.counters[i].max_value = struct_storage.get_int64(
			(i << 16) | size_t(StructStorageFields::license_state_licenses_total_counts_int16_t) );

		result.counters[i].value = struct_storage.get_int64(
			(i << 16) | size_t(StructStorageFields::license_state_licenses_in_use_counts_int16_t) );
	}

	return result;
}


inline
void FacerecService::keepAlgorithmsCache() const
{
	void* exception = NULL;

	_dll_handle->FacerecService_toggleAlgorithmsCacheKepp(1, &exception);

	checkException(exception, *_dll_handle);
}

inline
void FacerecService::freeAlgorithmsCache() const
{
	void* exception = NULL;

	_dll_handle->FacerecService_toggleAlgorithmsCacheKepp(0, &exception);

	checkException(exception, *_dll_handle);
}



inline
InternalImageBuffer::Ptr FacerecService::createInternalImageBuffer(
	const int width,
	const int height,
	const RawImage::Format format)
{
	void* exception = NULL;

	int32_t data_size;
	void* data_ptr;
	void* imagetptr_ptr;

	void* const the_impl = _dll_handle->InternalImageBuffer_constructor(
		width,
		height,
		format,
		&data_size,
		&data_ptr,
		&imagetptr_ptr,
		&exception);

	checkException(exception, *_dll_handle);

	return InternalImageBuffer::Ptr::make(
		_dll_handle,
		the_impl,
		(unsigned char*) data_ptr,
		data_size,
		width,
		height,
		format,
		imagetptr_ptr);
}


#ifdef ANDROID
inline
InternalImageBuffer::Ptr FacerecService::createInternalImageBuffer(
	JNIEnv *env,
	jobject android_media_image)
{
	void* exception = NULL;

	int32_t width;
	int32_t height;
	int32_t format;
	int32_t data_size;
	void* data_ptr;
	void* imagetptr_ptr;

	void* const the_impl = _dll_handle->InternalImageBuffer_constructor_from_android_image(
		env,
		android_media_image,
		&width,
		&height,
		&format,
		&data_size,
		&data_ptr,
		&imagetptr_ptr,
		&exception);

	checkException(exception, *_dll_handle);

	return InternalImageBuffer::Ptr::make(
		_dll_handle,
		the_impl,
		(unsigned char*) data_ptr,
		data_size,
		width,
		height,
		(InternalImageBuffer::Format) format,
		imagetptr_ptr);
}
#endif

inline
void FacerecService::convertYUV2ARGB(
	const RawImage image,
	const bool downscale_x2,
	void* const result_buffer)
{
	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	_dll_handle->RawImage_convertYUV2ARGB(
		cdata.data,
		cdata.width,
		cdata.height,
		cdata.format,
		cdata.with_crop,
		cdata.crop_info_offset_x,
		cdata.crop_info_offset_y,
		cdata.crop_info_data_image_width,
		cdata.crop_info_data_image_height,
		downscale_x2,
		result_buffer,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
void FacerecService::convertYUV2RGB(
	const RawImage image,
	const bool downscale_x2,
	const int base_angle,
	void* const result_buffer)
{
	void* exception = NULL;

	const RawImage::CapiData cdata = image.makeCapiData();

	_dll_handle->RawImage_convertYUV2RGB(
		cdata.data,
		cdata.width,
		cdata.height,
		cdata.format,
		cdata.with_crop,
		cdata.crop_info_offset_x,
		cdata.crop_info_offset_y,
		cdata.crop_info_data_image_width,
		cdata.crop_info_data_image_height,
		downscale_x2,
		base_angle,
		result_buffer,
		&exception);

	checkException(exception, *_dll_handle);
}


}  // pbio namespace





#endif  // __PBIO_API__PBIO__FACEREC_SERVICE_H_
