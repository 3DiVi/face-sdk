/**
	\file VideoWorker.h
	\~English
	\brief
		VideoWorker is an interface object for tracking, processing and matching faces on multiple video streams.
	\~Russian
	\brief
		VideoWorker - интерфейсный объект для трекинга, обработки и распознавания лиц на нескольких видеопотоках. 
*/

#ifndef __PBIO_API__PBIO__VIDEOWORKER_H_
#define __PBIO_API__PBIO__VIDEOWORKER_H_


#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <stdexcept>



#include "ComplexObject.h"
#include "deprecated_macro.h"
#include "Error.h"
#include "ExceptionCheck.h"
#include "RawSample.h"
#include "Recognizer.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapIStreamBufferImpl.h"
#include "stl_wraps_impls/WrapIStreamImpl.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"
#include "AgeGenderEstimator.h"
#include "EmotionsEstimator.h"
#include "Template.h"
#include "DepthLivenessEstimator.h"
#include "DepthMapRaw.h"
#include "StructStorageFields.h"
#include "SampleCheckStatus.h"
#include "Config.h"
#include "util693bcd72/util.h"

namespace pbio
{

class FacerecService;

/** \~English
	\brief
		VideoWorker is an interface object for tracking, processing and matching faces on multiple video streams.
		We recommend you to use VideoWorker instead of Capturer for face tracking on video streams. When VideoWorker is created with 
		<i>matching_thread=0</i> and <i>processing_thread=0</i>, then the standard Capturer license is used.
	\~Russian	
	\brief
		VideoWorker - интерфейсный объект для трекинга, обработки 
		и распознавания лиц на нескольких видеопотоках. Рекомендуется использовать VideoWorker вместо 
		Capturer для детекции лиц на видеопотоках. Если при создании VideoWorker указаны параметры 
		<i>matching_thread=0</i> и <i>processing_thread=0</i>, то потребляется обычная лицензия Capturer.
*/
class VideoWorker : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to VideoWorker.
		\~Russian
		\brief Псевдоним для типа умного указателя на VideoWorker.
	*/
	typedef LightSmartPtr<VideoWorker>::tPtr Ptr;

	/** \~English
		\brief
		A special value for the person_id and element_id parameters in the MatchFoundCallback.
		\~Russian
		\brief
		Специальное значение для параметров person_id и element_id в MatchFoundCallback.
	*/
	static const uint64_t MATCH_NOT_FOUND_ID = (uint64_t) (-1);


	/** \~English
		\brief Parameters of the VideoWorker constructor.
		\~Russian
		\brief Параметры конструктора VideoWorker.
	*/
	struct Params
	{
		/**
			\~English \brief Default constructor, set strings to empty, numbers to zero, flags to false.
			\~Russian \brief Конструктор по умолчанию, устанавливает строки пустыми, числа нулем, флаги выключены.
		*/
		Params();

		/**
			\~English \brief Set the VideoWorker configuration file with optionally overridden parameters. \return *this
			\~Russian \brief Задать конфигурационный файл VideoWorker с опционально переопределенными параметрами. \return *this
		*/
		def_param_field_693bcd72(Params, pbio::Config                , video_worker_config);

		/**
			\~English \brief Set the name of the configuration file for Recognizer that will be used inside VideoWorker. \return *this
			\~Russian \brief Задать имя конфигурационного файла для используемого распознавателя (Recognizer). \return *this
		*/
		def_param_field_693bcd72(Params, std::string                 , recognizer_ini_file);

		/**
			\~English \brief Set the number of video streams. \return *this
			\~Russian \brief Задать количество видеопотоков. \return *this
		*/
		def_param_field_693bcd72(Params, int                         , streams_count);

		/**
			\~English \brief Set the number of threads for creating templates. \return *this
			\~Russian \brief Задать количество потоков для создания шаблонов. \return *this
		*/
		def_param_field_693bcd72(Params, int                         , processing_threads_count);

		/**
			\~English \brief Set the number of threads for matching templates with the database. \return *this
			\~Russian \brief Задать количество потоков для сравнения шаблонов, созданных из видеопотоков, с базой. \return *this
		*/
		def_param_field_693bcd72(Params, int                         , matching_threads_count);

		/**
			\~English \brief Set the number of threads for age_gender estimation. \return *this
			\~Russian \brief Задать количество потоков для определения пола и возраста. \return *this
		*/
		def_param_field_693bcd72(Params, int                         , age_gender_estimation_threads_count);

		/**
			\~English \brief Set the number of threads for estimation of emotions. \return *this
			\~Russian \brief Задать количество потоков для определения эмоций. \return *this
		*/
		def_param_field_693bcd72(Params, int                         , emotions_estimation_threads_count);

		/**
			\~English \brief Set the flag enabling \ref vw_sti "short time identification". \return *this
			\~Russian \brief Задать флаг, включающий \ref vw_sti "кратковременную индетификацию". \return *this
		*/
		def_param_field_693bcd72(Params, bool                        , short_time_identification_enabled);

		/**
			\~English \brief Set the recognition distance threshold for \ref vw_sti "short time identification". \return *this
			\~Russian \brief Задать порог распознавания для \ref vw_sti "кратковременной идентификации". \return *this
		*/
		def_param_field_693bcd72(Params, float                       , short_time_identification_distance_threshold);

		/**
			\~English \brief Set outdate_time in seconds for \ref vw_sti "short time identification". \return *this
			\~Russian \brief Задать в секундах длину временного интервала для \ref vw_sti "кратковременной идентификации". \return *this
		*/
		def_param_field_693bcd72(Params, float                       , short_time_identification_outdate_time_seconds);
	};


	/** \~English
		\brief Template search result (for  VideoWorker::MatchFoundCallbackFuncU).
		\~Russian
		\brief Результат поиска шаблона (для VideoWorker::MatchFoundCallbackFuncU).
	*/
	struct SearchResult
	{
		/** \~English
			\brief
				Unique id of the person.
			\~Russian
			\brief
				Уникальный идентификатор человека.
		*/
		uint64_t person_id;
		
		/** \~English
			\brief
				Unique id of the element.
			\~Russian
			\brief
				Уникальный идентификатор элемента.
		*/
		uint64_t element_id;

		/** \~English
			\brief Result of template matching.
			\~Russian
			\brief Результат сравнения шаблонов.
		*/
		Recognizer::MatchResult match_result;
	};


	/**
		\~English
		\brief
			Callback function type for a Tracking event.
			\warning
			This is a deprecated version (see the new one VideoWorker::TrackingCallbackU)

		\param[in]  stream_id
			see the description in VideoWorker::TrackingCallbackData

		\param[in]  frame_id
			see the description in VideoWorker::TrackingCallbackData

		\param[in]  samples
			see the description in VideoWorker::TrackingCallbackData

		\param[in]  samples_weak
			see the description in VideoWorker::TrackingCallbackData

		\param[in]  samples_quality
			see the description in VideoWorker::TrackingCallbackData

		\param[in]  userdata
			Pointer to data submitted by the user when calling 
			the VideoWorker::addTrackingCallback method.

		\~Russian
		\brief
			Тип функции коллбэка трекинга (Tracking).
			\warning
			Это устаревшая версия (см. новую VideoWorker::TrackingCallbackU)

		\param[in]  stream_id
			см. описание в VideoWorker::TrackingCallbackData

		\param[in]  frame_id
			см. описание в VideoWorker::TrackingCallbackData

		\param[in]  samples
			см. описание в VideoWorker::TrackingCallbackData

		\param[in]  samples_weak
			см. описание в VideoWorker::TrackingCallbackData

		\param[in]  samples_quality
			см. описание в VideoWorker::TrackingCallbackData

		\param[in]  userdata
			Указатель на  данные, поданные пользователем
			при вызове метода VideoWorker::addTrackingCallback.
	*/
	typedef
		void (*TrackingCallbackFunc)(
			const int stream_id,
			const int frame_id,
			const std::vector<RawSample::Ptr> &samples,
			const std::vector<bool> &samples_weak,
			const std::vector<float> &samples_quality,
			void* const userdata);

	/**
		\~English
		\brief
			Tracking callback data.
		\~Russian
		\brief
			Данные Tracking коллбэка.
	*/
	struct TrackingCallbackData
	{
		/**
			\~English
			\brief
				Integer id of the video stream
				(0 <= stream_id < streams_count).
			\~Russian
			\brief
				Целочисленный идентификатор видеопотока
				(0 <= stream_id < streams_count).
		*/
		int64_t stream_id;

		/**
			\~English
			\brief
				Integer id of the frame
				(that was returned by VideoWorker::addVideoFrame).
			\~Russian
			\brief
				Целочисленный идентификатор кадра
				(который был возвращен методом VideoWorker::addVideoFrame).
		*/
		int64_t frame_id;

		/**
			\~English
			\brief
				Vector of face samples found by the tracker.
				\n
				Most of the samples are received from the frame_id frame 
				but some samples can be received from the previous frames.
				\n
				Use RawSample::getFrameID to determine 
				which frame the sample belongs to.
			\~Russian
			\brief
				Вектор лиц, найденных трекером.
				\n
				Большинство сэмплов будут получены с кадра frame_id, 
				но некоторые могут быть получены с предыдущих кадров.
				\n
				Используйте метод RawSample::getFrameID, 
				чтобы определить, к какому кадру относится сэмпл.
		*/
		std::vector<RawSample::Ptr> samples;

		/**
			\~English
			\brief
				Vector of face IDs (track_id).
				track_id is equal to sample.getID() for the sample given in any VideoWorker callback.
				(samples_track_id.size() == samples.size())
			\~Russian
			\brief
				Вектор идентификаторов лиц (track_id).
				track_id равен sample.getID() для сэмпла в любом VideoWorker коллбэке.
				(samples_track_id.size() == samples.size())
		*/
		std::vector<int> samples_track_id;

		/**
			\~English
			\brief
				Since this is tracking, some samples may be false,
				so we mark the samples with the "weak" flag
				if they haven’t passed one of the rechecks (see: samples_good_face_size,
				samples_good_angles, samples_depth_liveness_confirmed,
				samples_detector_confirmed, samples_good_light_and_blur).
				"Weak" samples are not used for recognition
				(samples_weak.size() == samples.size())
			\~Russian
			\brief
				Так как это трекинг, то некоторые сэмплы могут быть ошибочными,
				поэтому мы помечаем сэмплы флагом "weak", если они не прошли
				одну из дополнительных проверок (см: samples_good_face_size,
				samples_good_angles, samples_depth_liveness_confirmed,
				samples_detector_confirmed, samples_good_light_and_blur).
				"weak" сэмплы не используются для распознавания
				(samples_weak.size() == samples.size())
		*/
		std::vector<bool> samples_weak;

		/**
			\~English
			\brief
				Quality value for the face sample.
				The same as from the FaceQualityEstimator
				(samples_quality.size() == samples.size())
			\~Russian
			\brief
				Качество сэмпла.
				Точно такое же, как и у FaceQualityEstimator
				(samples_quality.size() == samples.size())
		*/
		std::vector<float> samples_quality;

		/**
			\~English
			\brief
				The result of checking the sample for good lighting conditions and absence of a strong blur
				(samples_good_light_and_blur.size() == samples.size())
			\~Russian
			\brief
				Результат проверки сэмпла на хорошие условия освещения и отсутствие сильного размытия
				(samples_good_light_and_blur.size() == samples.size())
		*/
		std::vector<SampleCheckStatus::Verdict> samples_good_light_and_blur;

		/**
			\~English
			\brief
				The result of checking the sample for absence of too high yaw/pitch angles
				(samples_good_angles.size() == samples.size())
			\~Russian
			\brief
				Результат проверки сэмпла на отсутствие слишком больших углов поворота лица
				(samples_good_angles.size() == samples.size())
		*/
		std::vector<SampleCheckStatus::Verdict> samples_good_angles;

		/**
			\~English
			\brief
				The result of checking the sample for suitable face size,
				see the <i>min_template_generation_face_size</i> parameter in config file.
				(samples_good_face_size.size() == samples.size())
			\~Russian
			\brief
				Результат проверки сэмпла на приемлемый размер лица,
				см. параметр <i>min_template_generation_face_size</i> в конфигурационном файле.
				(samples_good_face_size.size() == samples.size())
		*/
		std::vector<SampleCheckStatus::Verdict> samples_good_face_size;

		/**
			\~English
			\brief
				The result of checking the sample with the frontal face detector
				(samples_detector_confirmed.size() == samples.size())
			\~Russian
			\brief
				Результат проверки сэмпла фронтальным детектором лиц
				(samples_detector_confirmed.size() == samples.size())
		*/
		std::vector<SampleCheckStatus::Verdict> samples_detector_confirmed;

		/**
			\~English
			\brief
				The result of checking the sample with DepthLivenessEstimator,
				depth frames are required, see VideoWorker::addDepthFrame.
				See DepthLivenessEstimator::Liveness for details.
				(samples_depth_liveness_confirmed.size() == samples.size())
			\~Russian
			\brief
				Результат проверки сэмпла посредством DepthLivenessEstimator,
				требуются кадры глубины, см. VideoWorker::addDepthFrame.
				См. также DepthLivenessEstimator::Liveness.
				(samples_depth_liveness_confirmed.size() == samples.size())
		*/
		std::vector<DepthLivenessEstimator::Liveness> samples_depth_liveness_confirmed;

		/**
			\~English
			\brief
				Flag indicating that age and gender were estimated for this track.
				(samples_track_age_gender_set.size() == samples.size())
			\~Russian
			\brief
				Флаг, обозначающий что пол и возраст определены для этого трека.
				(samples_track_age_gender_set.size() == samples.size())
		*/
		std::vector<bool> samples_track_age_gender_set;

		/**
			\~English
			\brief
				Estimated age and gender for this track.
				(samples_track_age_gender.size() == samples.size())
			\~Russian
			\brief
				Оценки пола и возраста для этого трека.
				(samples_track_age_gender.size() == samples.size())
		*/
		std::vector<AgeGenderEstimator::AgeGender> samples_track_age_gender;

		/**
			\~English
			\brief
				Flag indicating that emotions were estimated for this track.
				(samples_track_emotions_set.size() == samples.size())
			\~Russian
			\brief
				Флаг, обозначающий что эмоции определены для этого трека.
				(samples_track_emotions_set.size() == samples.size())
		*/
		std::vector<bool> samples_track_emotions_set;

		/**
			\~English
			\brief
				Estimated emotions for this track.
				(samples_track_emotions.size() == samples.size())
			\~Russian
			\brief
				Оценка эмоций для этого трека.
				(samples_track_emotions.size() == samples.size())
		*/
		std::vector<EmotionsEstimator::EstimatedEmotionsVector> samples_track_emotions;
	};

	/**
		\~English
		\brief
			Callback function type for a Tracking event.
			This callback is called every time the frame 
			has been processed by the tracker.
			Tracking callback will be called with frame_id = X not earlier than
			VideoWorker::addVideoFrame returns the value of X + N - 1, where N is the value
			returned by VideoWorker::getTrackingConveyorSize.

			Tracking callbacks with the same stream_id are called in 
			ascending frame_id order. So, if you receive a callback with stream_id=2 
			and frame_id=102 right after a callback with stream_id=2 
			and frame_id=100, then the frame with frame_id=101 was skipped for the video stream 2.

		\param[in]  data
			Tracking callback data.

		\param[in]  userdata
			Pointer to data submitted by the user when calling 
			the VideoWorker::addTrackingCallbackU method.

		\~Russian
		\brief
			Тип функции коллбэка трекинга (Tracking).
			Этот коллбэк вызывается каждый раз, 
			когда кадр был обработан трекером.
			Tracking коллбэк будет вызван с frame_id = X не раньше, чем
			VideoWorker::addVideoFrame вернет значение X + N - 1, где N - значение,
			которое вернул VideoWorker::getTrackingConveyorSize.

			Tracking коллбэки с одинаковым stream_id вызываются в порядке 
			возрастания значения frame_id.  Поэтому если вы получили коллбэк 
			со значениями stream_id=2 и frame_id=102 сразу после коллбэка 
			со значениями stream_id=2 и frame_id=100, значит, 
			кадр со значением frame_id=101 был пропущен для видеопотока 2.

		\param[in]  data
			Данные Tracking коллбэка.

		\param[in]  userdata
			Указатель на данные, поданные пользователем
			при вызове метода VideoWorker::addTrackingCallbackU.
	*/
	typedef
		void (*TrackingCallbackU)(
			const TrackingCallbackData &data,
			void* const userdata);
	
	/**
		\~English
		\brief
			Callback function type for a Tracking event.
			\warning
			This is a deprecated version (see the new one VideoWorker::TrackingCallbackU)

		\~Russian
		\brief
			Тип функции коллбэка трекинга (Tracking).
			\warning
			Это устаревшая версия (см. новую VideoWorker::TrackingCallbackU)
	*/
	PBIO_API_DEPRECATED_744e321c
	typedef 
		void (*TrackingCallbackFuncU)(
			const TrackingCallbackData &data,
			void* const userdata);

	/**
		\~English
		\brief
			Callback function type for a TemplateCreated event.
			\warning
			This is a deprecated version (see the new one VideoWorker::TemplateCreatedCallbackU)

		\param[in]  stream_id
			see the description in VideoWorker::TemplateCreatedCallbackData

		\param[in]  frame_id
			see the description in VideoWorker::TemplateCreatedCallbackData

		\param[in]  sample
			see the description in VideoWorker::TemplateCreatedCallbackData

		\param[in]  quality
			see the description in VideoWorker::TemplateCreatedCallbackData

		\param[in]  templ
			see the description in VideoWorker::TemplateCreatedCallbackData

		\param[in]  userdata
			Pointer to data submitted by the user 
			when calling the VideoWorker::addTemplateCreatedCallback method.

		\~Russian
		\brief
			Тип функции коллбэка создания шаблона (TemplateCreated).
			\warning
			Это устаревшая версия (см. новую VideoWorker::TemplateCreatedCallbackU)

		\param[in]  stream_id
			см. описание в VideoWorker::TemplateCreatedCallbackData

		\param[in]  frame_id
			см. описание в VideoWorker::TemplateCreatedCallbackData

		\param[in]  sample
			см. описание в VideoWorker::TemplateCreatedCallbackData

		\param[in]  quality
			см. описание в VideoWorker::TemplateCreatedCallbackData

		\param[in]  templ
			см. описание в VideoWorker::TemplateCreatedCallbackData

		\param[in]  userdata
			Указатель на  данные, поданные пользователем
			при вызове метода VideoWorker::addTemplateCreatedCallback.
	*/
	typedef
		void (*TemplateCreatedCallbackFunc)(
			const int stream_id,
			const int frame_id,
			const RawSample::Ptr &sample,
			const float quality,
			const Template::Ptr &templ,
			void* const userdata);

	/**
		\~English
		\brief
			TemplateCreated callback data.
		\~Russian
		\brief
			Данные TemplateCreated коллбэка.
	*/
	struct TemplateCreatedCallbackData
	{
		/**
			\~English
			\brief
				Integer id of the video stream
				(0 <= stream_id < streams_count).
			\~Russian
				Целочисленный идентификатор видеопотока
				(0 <= stream_id < streams_count).
			\brief
		*/
		int64_t stream_id;

		/**
			\~English
			\brief
				Integer id of the frame
				(that was returned by VideoWorker::addVideoFrame).
			\~Russian
				Целочисленный идентификатор кадра
				(который был возвращен методом VideoWorker::addVideoFrame).
			\brief
		*/
		int64_t frame_id;

		/**
			\~English
			\brief
				Face sample.
			\~Russian
				Сэмпл лица.
			\brief
		*/
		RawSample::Ptr sample;

		/**
			\~English
			\brief
				Face sample quality.
				The same as from the FaceQualityEstimator.
			\~Russian
				Качество сэмпла.
				Точно такое же, как и у FaceQualityEstimator.
			\brief
		*/
		double quality;

		/**
			\~English
			\brief
				Template created for this sample.
			\~Russian
				Шаблон, созданный из sample.
			\brief
		*/
		Template::Ptr templ;
	};


	/**
		\~English
		\brief
			Callback function type for a TemplateCreated event.

			This callback is called whenever a template is created within the VideoWorker.

			It is guaranteed that this callback will be called 
			after at least one Tracking callback and before the TrackingLost 
			callback with the same stream_id and track_id (track_id = sample->getID()).

		\param[in]  data
			TemplateCreated callback data.

		\param[in]  userdata
			Pointer to data submitted by the user 
			when calling the VideoWorker::addTemplateCreatedCallbackU method.

		\~Russian
		\brief
			Тип функции коллбэка создания шаблона (TemplateCreated).

			Этот коллбэк вызывается каждый раз, когда в VideoWorker создается шаблон.

			Гарантируется, что этот коллбэк будет вызван после как
			минимум одного Tracking коллбэка и перед TrackingLost коллбэком
			с тем же stream_id и track_id (track_id = sample->getID()).

		\param[in]  data
			Данные TemplateCreated коллбэка.

		\param[in]  userdata
			Указатель на данные, поданные пользователем
			при вызове метода VideoWorker::addTemplateCreatedCallbackU.
	*/
	typedef
		void (*TemplateCreatedCallbackU)(
			const TemplateCreatedCallbackData &data,
			void* const userdata);

	/**
		\brief
			Callback function type for a TemplateCreated event.
			\warning
			This is a deprecated version (see the new one VideoWorker::TemplateCreatedCallbackU)

		\~Russian
		\brief
			Тип функции коллбэка создания шаблона (TemplateCreated).
			\warning
			Это устаревшая версия (см. новую VideoWorker::TemplateCreatedCallbackU)
	*/
	PBIO_API_DEPRECATED_744e321c
	typedef
		void (*TemplateCreatedCallbackFuncU)(
			const TemplateCreatedCallbackData &data,
			void* const userdata);

	/**
		\~English
		\brief
			Callback function type for a MatchFound event.
			\warning
			This is a deprecated version (see the new one VideoWorker::MatchFoundCallbackU)

		\param[in]  stream_id
			see the description in VideoWorker::MatchFoundCallbackData.

		\param[in]  frame_id
			see the description in VideoWorker::MatchFoundCallbackData.

		\param[in]  person_id
			see <i>search_results</i> in VideoWorker::MatchFoundCallbackData

		\param[in]  element_id
			see <i>search_results</i> in VideoWorker::MatchFoundCallbackData.

		\param[in]  sample
			see the description in VideoWorker::MatchFoundCallbackData.

		\param[in]  quality
			see the description in VideoWorker::MatchFoundCallbackData.

		\param[in]  templ
			see the description in VideoWorker::MatchFoundCallbackData.

		\param[in]  match_result
			see <i>search_results</i> in VideoWorker::MatchFoundCallbackData.

		\param[in]  userdata
			Pointer to data submitted by the user 
			when calling the VideoWorker::addMatchFoundCallback method.

		\~Russian
		\brief
			Тип функции коллбэка распознавания (MatchFound).
			\warning
			Это устаревшая версия (см. новую VideoWorker::MatchFoundCallbackU)

		\param[in]  stream_id
			см. описание в VideoWorker::MatchFoundCallbackData

		\param[in]  frame_id
			см. описание в VideoWorker::MatchFoundCallbackData

		\param[in]  person_id
			см. <i>search_results</i> в VideoWorker::MatchFoundCallbackData

		\param[in]  element_id
			см. <i>search_results</i> в VideoWorker::MatchFoundCallbackData

		\param[in]  sample
			см. описание в VideoWorker::MatchFoundCallbackData

		\param[in]  quality
			см. описание в VideoWorker::MatchFoundCallbackData

		\param[in]  templ
			см. описание в VideoWorker::MatchFoundCallbackData

		\param[in]  match_result
			см. <i>search_results</i> в VideoWorker::MatchFoundCallbackData

		\param[in]  userdata
			Указатель на данные, поданные пользователем
			при вызове метода VideoWorker::addMatchFoundCallback.
	*/
	typedef
		void (*MatchFoundCallbackFunc)(
			const int stream_id,
			const int frame_id,
			const uint64_t person_id,
			const uint64_t element_id,
			const RawSample::Ptr &sample,
			const float quality,
			const Template::Ptr &templ,
			const Recognizer::MatchResult match_result,
			void* const userdata);

	/**
		\~English
		\brief
			Callback function type for a MatchFound event.
			\warning
			This is a deprecated version (see the new one VideoWorker::MatchFoundCallbackU)

			Similar to VideoWorker::MatchFoundCallbackFunc but returns 
			all elements with a distance less than distance_threshold 
			(but not more than <i>k</i> nearest elements, the value of <i>k</i> is set 
			in the configuration file tagged as <i>search_k</i>).

			When called after N consecutive mismatches, 
			the first element in search_result will have a zero distance, 
			and person_id and element_id are equal to VideoWorker::MATCH_NOT_FOUND_ID.

		\~Russian
		\brief
			Тип функции коллбэка распознавания (MatchFound).
			\warning
			Это устаревшая версия (см. новую VideoWorker::MatchFoundCallbackU)

			Аналогичен VideoWorker::MatchFoundCallbackFunc, но возвращает все элементы 
			с расстоянием меньше distance_threshold (но не более k ближайших элементов; 
			величина k установлена в конфигурационном файле под тегом search_k).

			При вызове после N последовательных несовпадений первый элемент в search_result
			будет иметь нулевое расстояние, а person_id и element_id равны
			VideoWorker::MATCH_NOT_FOUND_ID.
	*/
	typedef
		void (*MatchFoundCallbackFuncExt)(
			const int stream_id,
			const int frame_id,
			const RawSample::Ptr &sample,
			const float quality,
			const Template::Ptr &templ,
			const std::vector<SearchResult> &search_result,
			void* const userdata);

	/**
		\~English
		\brief
			MatchFound callback data.
		\~Russian
		\brief
			Данные MatchFound коллбэка.
	*/
	struct MatchFoundCallbackData
	{
		/**
			\~English
			\brief
				Integer id of the video stream
				(0 <= stream_id < streams_count).
			\~Russian
			\brief
				Целочисленный идентификатор видеопотока
				(0 <= stream_id < streams_count).
		*/
		int64_t stream_id;

		/**
			\~English
			\brief
				Integer id of the frame
				(that was returned by VideoWorker::addVideoFrame).
			\~Russian
			\brief
				Целочисленный идентификатор кадра
				(который был возвращен методом VideoWorker::addVideoFrame).
		*/
		int64_t frame_id;

		/**
			\~English
			\brief
				Face sample.
			\~Russian
			\brief
				Сэмпл лица.
		*/
		RawSample::Ptr sample;

		/**
			\~English
			\brief
				Face sample quality.
				The same as from the FaceQualityEstimator.
			\~Russian
			\brief
				Качество сэмпла.
				Точно такое же, как и у FaceQualityEstimator.
		*/
		double quality;

		/**
			\~English
			\brief
				Template created for this sample.
			\~Russian
			\brief
				Шаблон, созданный из sample.
		*/
		Template::Ptr templ;

		/**
			\~English
			\brief
				Found elements with a distance less than distance_threshold 
				(but not more than <i>k</i> nearest elements, the value of <i>k</i> is set 
				in the configuration file tagged as <i>search_k</i> tag).

				When called after N consecutive mismatches, 
				the first element in search_results will have a zero distance, 
				and person_id and element_id are equal to VideoWorker::MATCH_NOT_FOUND_ID.
			\~Russian
			\brief
				Найденные элементы с расстоянием меньше distance_threshold
				(но не более k ближайших элементов; величина k установлена
				в конфигурационном файле под тегом search_k).

				При вызове после N последовательных несовпадений первый элемент в search_results
				будет иметь нулевое расстояние, а person_id и element_id равны
				VideoWorker::MATCH_NOT_FOUND_ID.
		*/
		std::vector<SearchResult> search_results;
	};

	/**
		\~English
		\brief
			Callback function type for a MatchFound event.

			When a template for a tracked face is created, 
			it is compared with each template in the dataset, 
			and if the distance to the nearest element is less 
			than the distance_threshold specified in this element, then a match is fixed.

			MatchFoundCallbackData::search_results contains all elements with a distance less than distance_threshold 
			(but not more than <i>k</i> nearest elements, the value of <i>k</i> is set 
			in the configuration file tagged as <i>search_k</i> tag).

			This callback is called after N sequential matches 
			with the elements that belong to the same person.

			The N number can be set in a config file using the
			\<consecutive_match_count_for_match_found_callback\> tag.

			It is guaranteed that this callback will be called after at
			least one Tracking callback and before TrackingLost callback
			with the same stream_id and track_id (track_id = sample->getID()).

			Also you can set the value of '1' for the 
			\<not_found_match_found_callback\> tag to enable this callback 
			after N consecutive mismatches (i.e. when the closest element is beyond its distance_threshold).
			In this case, match_result of the first element in MatchFoundCallbackData::search_results is at zero distance, 
			and person_id and element_id are equal to VideoWorker::MATCH_NOT_FOUND_ID.

		\param[in]  data
			MatchFound callback data.

		\param[in]  userdata
			Pointer to data submitted by the user 
			when calling the VideoWorker::addMatchFoundCallbackU method.

		\~Russian
		\brief
			Тип функции коллбэка распознавания (MatchFound).

			Когда для отслеживаемого лица создается шаблон, 
			он сравнивается с каждым шаблоном из базы, и если расстояние 
			до ближайшего элемента оказывается меньше порога distance_threshold, 
			указанного в этом элементе, то фиксируется совпадение.

			MatchFoundCallbackData::search_results содержит все элементы 
			с расстоянием меньше distance_threshold (но не более k ближайших элементов; 
			величина k установлена в конфигурационном файле под тегом search_k).

			Этот коллбэк вызывается после N последовательных совпадений 
			с элементами, относящимися к одному и тому же человеку.

			Число N может быть задано в конфигурационном файле в тэге
			\<consecutive_match_count_for_match_found_callback\>.

			Гарантируется, что этот коллбэк будет вызван после как
			минимум одного Tracking коллбэка и перед TrackingLost коллбэком
			с тем же stream_id и track_id (track_id = sample->getID()).

			Также, установив значение ‘1’ для тэга \<not_found_match_found_callback\> 
			можно включить вызов этого коллбэка после N последовательных несовпадений 
			(т.е. когда ближайший элемент оказывается дальше его порога distance_threshold).
			В этом случае match_result первого элемента в MatchFoundCallbackData::search_results будет с нулевым расстоянием,
			а person_id и element_id будут равны VideoWorker::MATCH_NOT_FOUND_ID.

		\param[in]  data
			Данные MatchFound коллбэка.

		\param[in]  userdata
			Указатель на  данные, поданные пользователем
			при вызове метода VideoWorker::addMatchFoundCallbackU.
	*/
	typedef
		void (*MatchFoundCallbackU)(
			const MatchFoundCallbackData &data,
			void* const userdata);

	/**
		\~English
		\brief
			Callback function type for a MatchFound event.
			\warning
			This is a deprecated version (see the new one VideoWorker::MatchFoundCallbackU)

		\~Russian
		\brief
			Тип функции коллбэка распознавания (MatchFound).
			\warning
			Это устаревшая версия (см. новую VideoWorker::MatchFoundCallbackU)
	*/
	PBIO_API_DEPRECATED_744e321c
	typedef
		void (*MatchFoundCallbackFuncU)(
			const MatchFoundCallbackData &data,
			void* const userdata);

	/**
		\~English
		\brief
			Callback function type for a TrackingLost event
			(e.g. when a person leaves the frame).
			\warning
			This is a deprecated version (see the new one VideoWorker::TrackingLostCallbackU).

		\param[in]  stream_id
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  first_frame_id
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  last_frame_id
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality_frame_id
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality_sample
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality_templ
			see the description in VideoWorker::TrackingLostCallbackData.

		\param[in]  userdata
			Pointer to the data submitted by the user
			when calling the VideoWorker::addTrackingLostCallback method. 

		\~Russian
		\brief
			Тип функции коллбэка потери трекинга лица (TrackingLost)
			(например, когда человек вышел из кадра).
			\warning
			Это устаревшая версия (см. новую VideoWorker::TrackingLostCallbackU).

		\param[in]  stream_id
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  first_frame_id
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  last_frame_id
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality_frame_id
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality_sample
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  best_quality_templ
			см. описание в VideoWorker::TrackingLostCallbackData.

		\param[in]  userdata
			Указатель на  данные, поданные пользователем
			при вызове метода VideoWorker::addTrackingLostCallback.
	*/
	typedef
		void (*TrackingLostCallbackFunc)(
			const int stream_id,
			const int first_frame_id,
			const int last_frame_id,
			const float best_quality,
			const int best_quality_frame_id,
			const RawSample::Ptr &best_quality_sample,
			const Template::Ptr &best_quality_templ,
			void* const userdata);

	/**
		\~English
		\brief
			TrackingLost callback data.
		\~Russian
		\brief
			Данные TrackingLost коллбэка.
	*/
	struct TrackingLostCallbackData
	{
		/**
			\~English
			\brief
				Integer id of the video stream
				(0 <= stream_id < streams_count).
			\~Russian
			\brief
				Целочисленный идентификатор видеопотока
				(0 <= stream_id < streams_count).
		*/
		int64_t stream_id;

		/**
			\~English
			\brief
				Integer id of the frame on which the face was first detected.
			\~Russian
			\brief
				Целочисленный идентификатор кадра, на котором лицо было замечено впервые.
		*/
		int64_t first_frame_id;

		/**
			\~English
			\brief
				Integer id of the frame after which the face tracking was lost.
			\~Russian
			\brief
				Целочисленный идентификатор кадра, после которого трекинг лица был потерян.
		*/
		int64_t last_frame_id;

		/**
			\~English
			\brief
				Best sample quality over all frames.
			\~Russian
			\brief
				Лучшее качество среди всех кадров.
		*/
		double best_quality;

		/**
			\~English
			\brief
				Integer id of the frame of the best quality.
			\~Russian
			\brief
				Целочисленный идентификатор кадра с наилучшим качеством.
		*/
		int64_t best_quality_frame_id;

		/**
			\~English
			\brief
				The best quality sample from the best_quality_frame_id frame.
				Will be NULL if "weak_tracks_in_tracking_callback" is enabled and all samples with that track_id 
				are flagged as "weak=true".
			\~Russian
			\brief
				Лучший сэмпл лица с кадра best_quality_frame_id.
				Будет равен NULL, если включен флаг "weak_tracks_in_tracking_callback" и все сэмплы с данным track_id 
				помечены флагом "weak=true".
		*/
		RawSample::Ptr best_quality_sample;

		/**
			\~English
			\brief
				Template created from best_quality_sample.
				Will be NULL if processing_threads_count is zero or best_quality_sample is NULL.
			\~Russian
			\brief
				Шаблон, созданный из best_quality_sample.
				Будет равен NULL при нулевом количестве потоков обработки или best_quality_sample, равном NULL.
		*/
		Template::Ptr best_quality_templ;

		/**
			\~English
			\brief
				ID of the lost face (track_id).
				track_id is equal to sample.getID() for a sample given in any VideoWorker callback.
			\~Russian
			\brief
				Идентификатор потерянного лица (track_id).
				track_id равен sample.getID() для сэмпла в любом VideoWorker коллбэке.
		*/
		int64_t track_id;

		/**
			\~English
			\brief
				Flag indicating that sti_person_id is set. sti_person_id is not set, if short time identification is disabled
				or if no templates were generated for this track.
			\~Russian
			\brief
				Флаг, обозначающий что sti_person_id определен. Значение sti_person_id не определено, если кратковременная идентификация отключена,
				либо если для данного трека не было сгенерировано ни одного шаблона.
		*/
		bool sti_person_id_set;

		/**
			\~English
			\brief
				ID of "sti_person", which is a set of tracks formed by short time identification.
				sti_person_id is equal to track_id of the first mebmer of this "sti_person" set of tracks.
			\~Russian
			\brief
				Идентификатор "sti_person" - набора треков, объединенных кратковременной идентификацией.
				Значение sti_person_id совпадает с track_id первого элемента, сформировавшего этот набор "sti_person".
		*/
		int64_t sti_person_id;
	};

	/**
		\~English
		\brief
			Callback function type for a TrackingLost event
			(e.g. when a person leaves the frame).

			It is guaranteed that this is the last callback for the
			pair <stream_id, track_id> (track_id is equal to sample.getID() for a sample given in any VideoWorker callback).

			So, after this callback, no Tracking, MatchFound or TrackingLost callback 
			for the video stream stream_id can contain a sample with the same track_id.

			Also it is guaranteed, that for each pair <stream_id, track_id> that was
			mentioned in the Tracking callback there will be exactly one TrackingLost callback.

		\param[in]  data
			TrackingLost callback data.

		\param[in]  userdata
			Pointer to the data submitted by the user
			when calling the VideoWorker::addTrackingLostCallbackU method. 

		\~Russian
		\brief
			Тип функции коллбэка потери трекинга лица (TrackingLost)
			(например, когда человек вышел из кадра).

			Гарантируется, что этот коллбэк будет последним для
			пары <stream_id, track_id> (track_id равен sample.getID() для сэмпла в любом VideoWorker коллбэке).

			Т.е. после него ни один Tracking, MatchFound или TrackingLost коллбэк
			для видеопотока stream_id не может содержать сэмпла с этим же track_id.

			Также гарантируется, что для каждой пары <stream_id, track_id>, 
			которая была упомянута в Tracking коллбэке, будет только один TrackingLost коллбэк.

		\param[in]  data
			Данные TrackingLost коллбэка.

		\param[in]  userdata
			Указатель на данные, поданные пользователем
			при вызове метода VideoWorker::addTrackingLostCallbackU.
	*/
	typedef
		void (*TrackingLostCallbackU)(
			const TrackingLostCallbackData &data,
			void* const userdata);

	/**
		\~English
		\brief
			Callback function type for a TrackingLost event
			(e.g. when a person leaves the frame).
			\warning
			This is a deprecated version (see the new one VideoWorker::TrackingLostCallbackU).

		\~Russian
		\brief
			Тип функции коллбэка потери трекинга лица (TrackingLost)
			(например, когда человек вышел из кадра).
			\warning
			Это устаревшая версия (см. новую VideoWorker::TrackingLostCallbackU).
	*/
	PBIO_API_DEPRECATED_744e321c
	typedef 
		void (*TrackingLostCallbackFuncU)(
			const TrackingLostCallbackData &data,
			void* const userdata);



	/**
		\~English
		\brief
			StiPersonOutdated callback data.
		\~Russian
		\brief
			Данные StiPersonOutdated коллбэка.
	*/
	struct StiPersonOutdatedCallbackData
	{
		/**
			\~English
			\brief
				Integer id of the video stream
				(0 <= stream_id < streams_count).
			\~Russian
			\brief
				Целочисленный идентификатор видеопотока
				(0 <= stream_id < streams_count).
		*/
		int64_t stream_id;

		/**
			\~English
			\brief
				ID of "sti_person", which is a set of tracks formed by short time identification.
				sti_person_id is equal to track_id of the first mebmer of this "sti_person" set of tracks.
			\~Russian
			\brief
				Идентификатор "sti_person" - набора треков, объединенных кратковременной идентификацией.
				Значение sti_person_id совпадает с track_id первого элемента, сформировавшего этот набор "sti_person".
		*/
		int64_t sti_person_id;
	};



	/**
		\~English
		\brief
			Callback function type for an StiPersonOutdated event.

		\param[in]  data
			StiPersonOutdated callback data.

		\param[in]  userdata
			Pointer to the data submitted by the user
			when calling the VideoWorker::addStiPersonOutdatedCallbackU method.

		\~Russian
		\brief
			Тип функции коллбэка StiPersonOutdated.

		\param[in]  data
			Данные StiPersonOutdated коллбэка.

		\param[in]  userdata
			Указатель на данные, поданные пользователем
			при вызове метода VideoWorker::addStiPersonOutdatedCallbackU.
	*/
	typedef
		void (*StiPersonOutdatedCallbackU)(
			const StiPersonOutdatedCallbackData &data,
			void* const userdata);



	/** \~English
		\brief
			Database element.
			If you have several face templates for one person, 
			you should create one element for each template 
			with the same person_id but different element_id.

		\~Russian
		\brief
			Элемент базы.
			Если имеется несколько шаблонов для одного человека, 
			нужно создать по одному элементу для каждого шаблона 
			с одинаковыми person_id, но разными element_id. 
	*/
	struct DatabaseElement
	{
		/** \~English
			\brief
				Unique id of the element.
			\~Russian
			\brief
				Уникальный идентификатор элемента.
		*/
		uint64_t element_id;

		/** \~English
			\brief
				Unique id of the person.
			\~Russian
			\brief
				Уникальный идентификатор человека.
		*/
		uint64_t person_id;

		/** \~English
			\brief
				Face template.
			\~Russian
			\brief
				Шаблон лица.
		*/
		Template::Ptr face_template;

		/** \~English
			\brief
				Distance threshold for a MatchFound event.

			Setting a different recognition threshold for different persons may be useful in some cases.
			For example, if there is a particular criminal in the blacklist, who is more dangerous
			than the other criminals, and you want to increase the recognition probability for this very person,
			despite the higher probability of a false acceptance error.

			\~Russian
			\brief
				Порог распознавания.

			Установка разных порогов распознавания для разных людей может быть полезной в некоторых случаях. 
			Например, в том случае, если определенный преступник из черного списка опаснее остальных,
			и вы хотите увеличить вероятность его распознавания,
			несмотря на увеличение вероятности ложного распознавания.
		*/
		float distance_threshold;
	};


	/**
		\~English
		\brief
			Get a method name of the recognizer used.
			Thread-safe.

		\return
			The name of the method.
		
		\~Russian
		\brief
			Получить имя метода используемого распознавателя.
			Потокобезопасный.

		\return
			Имя метода.
	*/
	std::string getMethodName() const;


	/**
		\~English
		\brief
			Get a number of video streams.
			Thread-safe.

		\return
			Number of video streams.

		\~Russian
		\brief
			Получить количество видеопотоков.
			Потокобезопасный.

		\return
			Количество видеопотоков.
	*/
	int getStreamsCount() const;


	/**
		\~English
		\brief
			Get a size N of tracking conveyor.
			Tracking callback will be called with frame_id = X not earlier than
			VideoWorker::addVideoFrame returns the value X + N - 1, where N is the value
			returned by VideoWorker::getTrackingConveyorSize.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\return
			Size of tracking conveyor.

		\~Russian
		\brief
			Получить размер N конвейера трекинга.
			Tracking коллбэк будет вызван с frame_id = X не раньше, чем
			VideoWorker::addVideoFrame вернет значение X + N - 1, где N - значение,
			которое вернул VideoWorker::getTrackingConveyorSize.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).

		\return
			Размер конвейера трекинга.
	*/
	int getTrackingConveyorSize(const int stream_id) const;


	/**
		\~English
		\brief
			Set / replace the database (can be called at any time).
			Available only if matching_threads_count > 0.
			Thread-safe.

		\param[in]  elements
			Vector of database elements.

		\param[in]  acceleration
			Acceleration type.

		\~Russian
		\brief
			Задать / заменить базу (можно вызывать в любое время).
			Доступно только при ненулевом количестве потоков сравнения (matching_threads_count).
			Потокобезопасный.

		\param[in]  elements
			Вектор элементов базы.

		\param[in]  acceleration
			Тип ускорения поиска.
	*/
	void setDatabase(
		const std::vector<DatabaseElement> &elements,
		const Recognizer::SearchAccelerationType
			acceleration = Recognizer::SEARCH_ACCELERATION_1);

	/**
		\~English
		\brief
			Add a new video frame for a specific video stream.
			Tracking and recognition results will be returned asynchronously
				via Tracking, MatchFound and TrackingLost callbacks.
			Thread-safe.

		\param[in]  frame
			Video frame.
			Only colored images are allowed
			(i.e. IRawImage::FORMAT_RGB, IRawImage::FORMAT_BGR, IRawImage::FORMAT_YUV_NV21 and IRawImage::FORMAT_YUV_NV12).

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\param[in] timestamp_microsec
			Timestamp of the frame in microseconds

		\return
			Integer id for this frame, unique for this video stream.
			This id will be used to identify this frame in the callbacks.

		\~Russian
		\brief
			Подать новый кадр определенного видеопотока.
			Результаты трекинга и распознавания будут возвращаться асинхронно 
			в коллбэках Tracking, MatchFound и TrackingLost. 
			Потокобезопасный.

		\param[in]  frame
			Изображение кадра.
			Допустимы только цветные изображения
			(т.е. IRawImage::FORMAT_RGB, IRawImage::FORMAT_BGR, IRawImage::FORMAT_YUV_NV21 и IRawImage::FORMAT_YUV_NV12).

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).

		\param[in] timestamp_microsec
			Временная метка кадра в микросекундах

		\return
			Целочисленный идентификатор кадра, уникальный для этого видеопотока,
			который будет использоваться в коллбэках для обозначения этого кадра.
	*/
	int addVideoFrame(
		const RawImage frame,
		const int stream_id,
		const uint64_t timestamp_microsec = uint64_t(-1));

	/**
		\~English
		\brief
			Add a new depth frame for a specific stream.
			Thread-safe.

		\param[in]  depth_frame
			Depth frame (DepthMapRaw) that contains depth data.
			
		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\param[in] timestamp_microsec
			Timestamp of the frame in microseconds

		\~Russian
		\brief
			Подать кадр определенного видеопотока c данными глубины.
			Потокобезопасный.

		\param[in] depth_frame
			Кадр глубины (DepthMapRaw).

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).

		\param[in] timestamp_microsec
			Временная метка кадра в микросекундах

	*/
	void addDepthFrame(
		const DepthMapRaw &depth_frame,
		const int stream_id,
		const uint64_t timestamp_microsec);

	/**
		\~English
		\brief
			Reset tracker state for a specified video stream.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Сбросить состояние трекера для указанного видеопотока.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void resetTrackerOnStream(const int stream_id);



	/**
		\~English
		\brief
			Reset work state for the stream.
			It waits for the callbacks, which are currently being executed, therefore, do not synchronize 
			the resetStream call with the code inside the callbacks, otherwise it can cause a deadlock.
			After resetStream is executed, no callbacks related to the previous frames or tracks will be called.
			<b>It is necessary</b> to release the memory allocated for all tracks with track_id less than the return
			value, since <b>there will be no TrackingLost callback</b> for the tracks removed during this reset.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\return
			Integer track_id_threshold, which means that all tracks that were
			removed during resetStream had track_id < track_id_threshold,
			and all new tracks will have track_id >= track_id_threshold.

		\~Russian
		\brief
			Сбросить состояние работы для указанного видеопотока.
			В процессе происходит ожидание выполняющихся в данный момент коллбэков, поэтому
			не синхронизируйте вызов resetStream с кодом внутри коллбэков, иначе возможен deadlock.
			После возврата управления от resetStream не будет вызвано ни одного
			коллбэка, относящегося к предыдущим кадрам или трекам.
			<b>Необходимо</b> освободить память, выделенную для треков, track_id которых меньше возвращенного
			значения, так как для треков, удаленных в процессе этого сброса, <b>TrackingLost коллбэк вызван не будет</b>.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).

		\return
			Целое число track_id_threshold, означающее, что все удаленные в
			ходе resetStream треки имели track_id < track_id_threshold,
			а все новые треки будут иметь track_id >= track_id_threshold.

	*/
	int resetStream(const int stream_id);


	/**
		\~English
		\brief
			Exceptions from the callbacks and inner threads 
			are rethrown when this method is called. 
			Thread-safe.

		\~Russian
		\brief
			Исключения, выброшенные в коллбэках и внутренних потоках, 
			будут выброшены повторно при вызове данного метода.
			Потокобезопасный.
	*/
	void checkExceptions();

	/**
		\~English
		\brief
			Add a handler for a Tracking event.
			Thread-safe.
			\warning
			This is a deprecated version (see the new one VideoWorker::addTrackingCallbackU)

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeTrackingCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события трекинга (Tracking).
			Потокобезопасный.
			\warning
			Это устаревшая версия (см. новую VideoWorker::addTrackingCallbackU)

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeTrackingCallback, чтобы отписаться.
	*/
	PBIO_API_DEPRECATED_744e321c
	int addTrackingCallback(
		const TrackingCallbackFunc callback,
		void* const userdata);

	/**
		\~English
		\brief
			Add a handler for a Tracking event.
			Thread-safe.

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeTrackingCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события трекинга (Tracking).
			Потокобезопасный.

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeTrackingCallback, чтобы отписаться.
	*/
	int addTrackingCallbackU(
		const TrackingCallbackU callback,
		void* const userdata);

	/**
		\~English
		\brief
			Add a handler for a TemplateCreated event.
			Thread-safe.
			\warning
			This is a deprecated version (see the new one VideoWorker::addTemplateCreatedCallbackU)

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeTemplateCreatedCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события создания шаблона (TemplateCreated).
			Потокобезопасный.
			\warning
			Это устаревшая версия (см. новую VideoWorker::addTemplateCreatedCallbackU)

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeTemplateCreatedCallback, чтобы отписаться.
	*/
	PBIO_API_DEPRECATED_744e321c
	int addTemplateCreatedCallback(
		const TemplateCreatedCallbackFunc callback,
		void* const userdata);

	/**
		\~English
		\brief
			Add a handler for a TemplateCreated event.
			Thread-safe.

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeTemplateCreatedCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события создания шаблона (TemplateCreated).
			Потокобезопасный.

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeTemplateCreatedCallback, чтобы отписаться.
	*/
	int addTemplateCreatedCallbackU(
		const TemplateCreatedCallbackU callback,
		void* const userdata);

	/**
		\~English
		\brief 
			Add a handler for a MatchFound event.
			Thread-safe.
			\warning
			This is a deprecated version (see the new one VideoWorker::addMatchFoundCallbackU)

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeMatchFoundCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события распознавания (MatchFound).
			Потокобезопасный.
			\warning
			Это устаревшая версия (см. новую VideoWorker::addMatchFoundCallbackU)

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeMatchFoundCallback, чтобы отписаться.
	*/
	PBIO_API_DEPRECATED_744e321c
	int addMatchFoundCallback(
		const MatchFoundCallbackFunc callback,
		void* const userdata);


	/**
		\~English
		\brief
			Add a handler for a MatchFound event.
			Thread-safe.
			\warning
			This is a deprecated version (see the new one VideoWorker::addMatchFoundCallbackU)

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeMatchFoundCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события распознавания (MatchFound).
			Потокобезопасный.
			\warning
			Это устаревшая версия (см. новую VideoWorker::addMatchFoundCallbackU)

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeMatchFoundCallback, чтобы отписаться.
	*/
	PBIO_API_DEPRECATED_744e321c
	int addMatchFoundCallbackExt(
		const MatchFoundCallbackFuncExt callback,
		void* const userdata);

	/**
		\~English
		\brief
			Add a handler for a MatchFound event.
			Thread-safe.

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeMatchFoundCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события распознавания (MatchFound).
			Потокобезопасный.

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeMatchFoundCallback, чтобы отписаться.
	*/
	int addMatchFoundCallbackU(
		const MatchFoundCallbackU callback,
		void* const userdata);


	/**
		\~English
		\brief
			Add a handler for an StiPersonOutdated event.
			Thread-safe.

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			When the callback is called, this pointer is passed as a userdata argument.

		\return
			Integer id for this callback.
			To unsubscribe, pass it to VideoWorker::removeStiPersonOutdatedCallback.

		\~Russian
		\brief
			Добавить обработчик события StiPersonOutdated.
			Потокобезопасный.

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Чтобы отписаться, передайте его в VideoWorker::removeStiPersonOutdatedCallback.
	*/
	int addStiPersonOutdatedCallbackU(
		const StiPersonOutdatedCallbackU callback,
		void* const userdata);


	/**
		\~English
		\brief
			Add a handler for a TrackingLost event.
			Thread-safe.
			\warning
			This is a deprecated version (see the new one VideoWorker::addTrackingLostCallbackU)

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeTrackingLostCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события потери трекинга лица (TrackingLost).
			Потокобезопасный.
			\warning
			Это устаревшая версия (см. новую VideoWorker::addTrackingLostCallbackU)

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeTrackingLostCallback, чтобы отписаться.
	*/
	PBIO_API_DEPRECATED_744e321c
	int addTrackingLostCallback(
		const TrackingLostCallbackFunc callback,
		void* const userdata);

	/**
		\~English
		\brief
			Add a handler for a TrackingLost event.
			Thread-safe.

		\param[in]  callback
			Callback function.

		\param[in]  userdata
			Any pointer.
			It will be passed to the callback as a userdata argument.

		\return
			Integer id for this callback.
			Use it for passing to VideoWorker::removeTrackingLostCallback to unsubscribe.

		\~Russian
		\brief
			Добавить обработчик события потери трекинга лица (TrackingLost).
			Потокобезопасный.

		\param[in]  callback
			Функция коллбэка.

		\param[in]  userdata
			Любой указатель.
			При вызове коллбэка он будет передан через аргумент userdata.

		\return
			Целочисленный идентификатор коллбэка.
			Используйте его для передачи в VideoWorker::removeTrackingLostCallback, чтобы отписаться.
	*/
	int addTrackingLostCallbackU(
		const TrackingLostCallbackU callback,
		void* const userdata);

	/**
		\~English
		\brief
			Remove a handler for Tracking event.
			Thread-safe.

		\param[in]  callback_id
			Integer callback id returned by VideoWorker::addTrackingCallback.

		\~Russian
		\brief
			Удалить обработчик события трекинга (Tracking).
			Потокобезопасный.

		\param[in]  callback_id
			Идентификатор коллбэка, полученный от VideoWorker::addTrackingCallback.
	*/
	void removeTrackingCallback(const int callback_id);


	/**
		\~English
		\brief
			Remove a handler for TemplateCreated event.
			Thread-safe.

		\param[in]  callback_id
			Integer callback id returned by VideoWorker::addTemplateCreatedCallback.

		\~Russian
		\brief
			Удалить обработчик события создания шаблона (TemplateCreated).
			Потокобезопасный.

		\param[in]  callback_id
			Идентификатор коллбэка, полученный от VideoWorker::addTemplateCreatedCallback.
	*/
	void removeTemplateCreatedCallback(const int callback_id);


	/**
		\~English
		\brief
			Remove a handler for MatchFound event.
			Thread-safe.

		\param[in]  callback_id
			Integer callback id returned by VideoWorker::addMatchFoundCallback
			or VideoWorker::addMatchFoundCallbackExt.

		\~Russian
		\brief
			Удалить обработчик события распознавания (MatchFound).
			Потокобезопасный.

		\param[in]  callback_id
			Идентификатор коллбэка, полученный от VideoWorker::addMatchFoundCallback
			или VideoWorker::addMatchFoundCallbackExt.
	*/
	void removeMatchFoundCallback(const int callback_id);


	/**
		\~English
		\brief
			Remove a handler for an StiPersonOutdated event.
			Thread-safe.

		\param[in]  callback_id
			Integer callback id returned by VideoWorker::addStiPersonOutdatedCallbackU.

		\~Russian
		\brief
			Удалить обработчик события StiPersonOutdated.
			Потокобезопасный.

		\param[in]  callback_id
			Целочисленный идентификатор коллбэка, полученный от VideoWorker::addStiPersonOutdatedCallbackU.
	*/
	void removeStiPersonOutdatedCallback(const int callback_id);

	/**
		\~English
		\brief
			Remove a handler for TrackingLost event.
			Thread-safe.

		\param[in]  callback_id
			Integer callback id returned by VideoWorker::addTrackingLostCallback.

		\~Russian
		\brief
			Удалить обработчик события потери трекинга лица (TrackingLost).
			Потокобезопасный.

		\param[in]  callback_id
			Идентификатор коллбэка, полученный от VideoWorker::addTrackingLostCallback.
	*/
	void removeTrackingLostCallback(const int callback_id);


	/**
		\~English
		\brief
			Disable the creation of templates for a given stream.
			After that, the matching stops as well.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Отключить генерацию шаблонов для данного потока.
			Соответственно, сравнения также остановятся.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void disableProcessingOnStream(const int stream_id);


	/**
		\~English
		\brief
			Enable the creation of templates for a given stream.
			After that, the matching is resumed as well.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Включить генерацию шаблонов для данного потока.
			Соответственно, сравнения также возобновятся.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void enableProcessingOnStream(const int stream_id);



	/**
		\~English
		\brief
			Disable age and gender estimation for a given stream.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Отключить определение пола и возраста для данного потока.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void disableAgeGenderEstimationOnStream(const int stream_id);



	/**
		\~English
		\brief
			Enable age and gender estimation for a given stream.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Включить определение пола и возраста для данного потока.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void enableAgeGenderEstimationOnStream(const int stream_id);

	/**
		\~English
		\brief
			Disable estimation of emotions for a given stream.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Отключить определение эмоций для данного потока.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void disableEmotionsEstimationOnStream(const int stream_id);



	/**
		\~English
		\brief
			Enable estimation of emotions for a given stream.
			Thread-safe.

		\param[in]  stream_id
			Integer id of the video stream
			(0 <= stream_id < streams_count).

		\~Russian
		\brief
			Включить определение эмоций для данного потока.
			Потокобезопасный.

		\param[in]  stream_id
			Целочисленный идентификатор видеопотока
			(0 <= stream_id < streams_count).
	*/
	void enableEmotionsEstimationOnStream(const int stream_id);

//! @cond IGNORED
	void setParameter(
		const std::string parameter,
		const double value);
//! @endcond

private:

	static
	void STrackingCallback(
		void* err_stream,
		void* this_vw,
		void const* callback_data,

		const int32_t callbacks_count,
		void* const* const callbacks_func,
		void* const* const callbacks_userdata,
		const int32_t u_callbacks_count,
		void* const* const u_callbacks_func,
		void* const* const u_callbacks_userdata);

	static
	void STemplateCreatedCallback(
		void* err_stream,
		void* this_vw,
		void const* callback_data,

		const int32_t callbacks_count,
		void* const* const callbacks_func,
		void* const* const callbacks_userdata,
		const int32_t u_callbacks_count,
		void* const* const u_callbacks_func,
		void* const* const u_callbacks_userdata);

	static
	void SMatchFoundCallback(
		void* err_stream,
		void* this_vw,
		void const* callback_data,

		const int32_t callbacks_count,
		void* const* const callbacks_func,
		void* const* const callbacks_userdata,
		const int32_t ext_callbacks_count,
		void* const* const ext_callbacks_func,
		void* const* const ext_callbacks_userdata,
		const int32_t u_callbacks_count,
		void* const* const u_callbacks_func,
		void* const* const u_callbacks_userdata);


	static
	void STrackingLostCallback(
		void* err_stream,
		void* this_vw,
		void const* callback_data,

		const int32_t callbacks_count,
		void* const* const callbacks_func,
		void* const* const callbacks_userdata,
		const int32_t u_callbacks_count,
		void* const* const u_callbacks_func,
		void* const* const u_callbacks_userdata);

	static
	void SStiPersonOutdatedCallback(
		void* err_stream,
		void* this_vw__,
		void const* callback_data,

		const int32_t callbacks_count,
		void* const* const callbacks_func,
		void* const* const callbacks_userdata);


	void toggleSomething(
		const int stream_id,
		const int something);

	VideoWorker(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<VideoWorker>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{



inline
VideoWorker::VideoWorker(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_setThisVW(
		_impl,
		this,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
std::string VideoWorker::getMethodName() const
{
	std::ostringstream name_stream;
	pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

	void* exception = NULL;

	_dll_handle->VideoWorker_getMethodName(
		_impl,
		&name_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	return name_stream.str();
}


inline
int VideoWorker::getStreamsCount() const
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_getStreamsCount(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
int VideoWorker::getTrackingConveyorSize(const int stream_id) const
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_getTrackingConveyorSize(
		_impl,
		stream_id,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
void VideoWorker::setDatabase(
	const std::vector<DatabaseElement> &elements,
	const Recognizer::SearchAccelerationType acceleration)
{
	std::vector<uint64_t> element_ids_v(elements.size());
	std::vector<uint64_t> person_ids_v(elements.size());
	std::vector<pbio::facerec::TemplateImpl const*> templates_v(elements.size());
	std::vector<float> thresholds_v(elements.size());

	for(size_t i = 0; i < elements.size(); ++i)
	{
		element_ids_v[i] = elements[i].element_id;
		person_ids_v[i] = elements[i].person_id;
		templates_v[i] = (pbio::facerec::TemplateImpl const*) elements[i].face_template->_impl;
		thresholds_v[i] = elements[i].distance_threshold;
	}

	void* exception = NULL;

	_dll_handle->VideoWorker_setDatabase(
		_impl,
		acceleration,
		elements.size(),
		element_ids_v.data(),
		person_ids_v.data(),
		templates_v.data(),
		thresholds_v.data(),
		&exception);

	checkException(exception, *_dll_handle);
}

inline
int VideoWorker::addVideoFrame(
	const RawImage frame,
	const int stream_id,
	const uint64_t timestamp_microsec)
{
	void* exception = NULL;

	const RawImage::CapiData cdata = frame.makeCapiData();

	const int result = _dll_handle->VideoWorker_addVideoFrameWithTimestamp_with_crop(
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

		stream_id,
		timestamp_microsec,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
void VideoWorker::addDepthFrame(
	const DepthMapRaw &depth_frame,
	const int stream_id,
	const uint64_t timestamp_microsec)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_addDepthFrame(
		_impl,
		depth_frame.depth_map_rows,
		depth_frame.depth_map_cols,
		depth_frame.depth_map_2_image_offset_x,
		depth_frame.depth_map_2_image_offset_y,
		depth_frame.depth_map_2_image_scale_x,
		depth_frame.depth_map_2_image_scale_y,
		depth_frame.horizontal_fov,
		depth_frame.vertical_fov,
		depth_frame.depth_unit_in_millimeters,
		depth_frame.depth_data,
		depth_frame.depth_data_stride_in_bytes,
		stream_id,
		timestamp_microsec,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
void VideoWorker::resetTrackerOnStream(const int stream_id)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_resetTrackerOnStream(
		_impl,
		stream_id,
		&exception);

	checkException(exception, *_dll_handle);
}


inline
int VideoWorker::resetStream(const int stream_id)
{
	void* exception = NULL;

	const int threshold_track_id = _dll_handle->VideoWorker_resetStream(
		_impl,
		stream_id,
		&exception);

	checkException(exception, *_dll_handle);

	return threshold_track_id;
}


inline
void VideoWorker::checkExceptions()
{
	void* exception = NULL;

	_dll_handle->VideoWorker_checkExceptions(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
int VideoWorker::addTrackingCallback(
	const TrackingCallbackFunc callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addTrackingCallback(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
int VideoWorker::addTrackingCallbackU(
	const TrackingCallbackU callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addTrackingCallbackU(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}



inline
int VideoWorker::addTemplateCreatedCallback(
	const TemplateCreatedCallbackFunc callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addTemplateCreatedCallback(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
int VideoWorker::addTemplateCreatedCallbackU(
	const TemplateCreatedCallbackU callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addTemplateCreatedCallbackU(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
int VideoWorker::addMatchFoundCallback(
	const MatchFoundCallbackFunc callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addMatchFoundCallback(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
int VideoWorker::addMatchFoundCallbackExt(
	const MatchFoundCallbackFuncExt callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addMatchFoundCallbackExt(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
int VideoWorker::addMatchFoundCallbackU(
	const MatchFoundCallbackU callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addMatchFoundCallbackU(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
int VideoWorker::addTrackingLostCallback(
	const TrackingLostCallbackFunc callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addTrackingLostCallback(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
int VideoWorker::addTrackingLostCallbackU(
	const TrackingLostCallbackU callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addTrackingLostCallbackU(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
int VideoWorker::addStiPersonOutdatedCallbackU(
	const StiPersonOutdatedCallbackU callback,
	void* const userdata)
{
	void* exception = NULL;

	const int result = _dll_handle->VideoWorker_addStiPersonOutdatedCallbackU(
		_impl,
		reinterpret_cast<void*>(callback),
		userdata,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}



inline
void VideoWorker::removeTrackingCallback(
	const int callback_id)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_removeTrackingCallback(
		_impl,
		callback_id,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
void VideoWorker::removeTemplateCreatedCallback(
	const int callback_id)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_removeTemplateCreatedCallback(
		_impl,
		callback_id,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
void VideoWorker::removeMatchFoundCallback(
	const int callback_id)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_removeMatchFoundCallback(
		_impl,
		callback_id,
		&exception);

	checkException(exception, *_dll_handle);
}


inline
void VideoWorker::removeStiPersonOutdatedCallback(
	const int callback_id)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_removeStiPersonOutdatedCallback(
		_impl,
		callback_id,
		&exception);

	checkException(exception, *_dll_handle);
}

inline
void VideoWorker::removeTrackingLostCallback(
	const int callback_id)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_removeTrackingLostCallback(
		_impl,
		callback_id,
		&exception);

	checkException(exception, *_dll_handle);
}



#define __0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions(name) \
	catch(const std::exception &e) \
	{ \
		const std::string error = \
			"VideoWorker " name " catch std::excetion: '" + \
			std::string( e.what() ) + "'."; \
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__); \
		this_vw._dll_handle->VideoWorker_errStreamWriteFunc(err_stream, error.c_str(), error.length()); \
	} \
	catch(...) \
	{ \
		const std::string error = "VideoWorker " name " catch '...'."; \
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__); \
		this_vw._dll_handle->VideoWorker_errStreamWriteFunc(err_stream, error.c_str(), error.length()); \
	}


// static
inline
void VideoWorker::STrackingCallback(
	void* err_stream,
	void* this_vw__,
	void const* callback_data,

	const int32_t callbacks_count,
	void* const* const callbacks_func,
	void* const* const callbacks_userdata,
	const int32_t u_callbacks_count,
	void* const* const u_callbacks_func,
	void* const* const u_callbacks_userdata)
{
	try
	{
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__);

		void* exception = NULL;

		const int64_t stream_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_stream_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t frame_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_frame_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t samples_count = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_samples_count_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		pbio::facerec::RawSampleImpl* const* const samples_impls = static_cast<pbio::facerec::RawSampleImpl* const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		int32_t const* const weak_samples = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_weak_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		float const* const samples_quality = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_samples_quality_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_good_light_and_blur = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_good_light_and_blur_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_good_angles = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_good_angles_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_good_face_size = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_good_face_size_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);


		int32_t const* const samples_detector_confirmed = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_detector_confirmed_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_depth_liveness_confirmed = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_depth_liveness_confirmed_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);



		int32_t const* const samples_track_age_gender_set = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_age_gender_set_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_track_gender = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_gender_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_track_age = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_age_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		float const* const samples_track_age_years = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_age_years_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_track_emotions_set = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_emotions_set_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_track_emotions_count = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_emotions_count_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		float const* const samples_track_emotions_confidence = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_emotions_confidence_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);

		int32_t const* const samples_track_emotions_emotion = static_cast<int32_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				pbio::StructStorageFields::video_worker_samples_track_emotions_emotion_t,
				&exception));
		checkException(exception, *this_vw._dll_handle);



		TrackingCallbackData data;

		data.stream_id = stream_id;
		data.frame_id = frame_id;
		data.samples.resize(samples_count);
		data.samples_track_id.resize(samples_count);
		data.samples_weak.resize(samples_count);
		data.samples_quality.resize(samples_count);
		data.samples_good_light_and_blur.resize(samples_count);
		data.samples_good_angles.resize(samples_count);
		data.samples_good_face_size.resize(samples_count);
		data.samples_detector_confirmed.resize(samples_count);
		data.samples_depth_liveness_confirmed.resize(samples_count);
		data.samples_track_age_gender_set.resize(samples_count);
		data.samples_track_age_gender.resize(samples_count);
		data.samples_track_emotions_set.resize(samples_count);
		data.samples_track_emotions.resize(samples_count);

		for(int i = 0, emotions_i = 0; i < samples_count; ++i)
		{
			data.samples[i] = RawSample::Ptr( RawSample::Ptr::make(this_vw._dll_handle, samples_impls[i]) );
			data.samples_track_id[i] = data.samples[i]->getID();

			data.samples_weak[i] = weak_samples[i];
			data.samples_quality[i] = samples_quality[i];

			data.samples_good_light_and_blur[i]      = (SampleCheckStatus::Verdict)samples_good_light_and_blur[i];
			data.samples_good_angles[i]              = (SampleCheckStatus::Verdict)samples_good_angles[i];
			data.samples_good_face_size[i]           = (SampleCheckStatus::Verdict)samples_good_face_size[i];
			data.samples_detector_confirmed[i]       = (SampleCheckStatus::Verdict)samples_detector_confirmed[i];

			data.samples_depth_liveness_confirmed[i] = (DepthLivenessEstimator::Liveness)samples_depth_liveness_confirmed[i];


			data.samples_track_age_gender_set[i]          = samples_track_age_gender_set[i];

			data.samples_track_age_gender[i].gender       = (AgeGenderEstimator::Gender) samples_track_gender[i];
			data.samples_track_age_gender[i].age          = (AgeGenderEstimator::Age) samples_track_age[i];
			data.samples_track_age_gender[i].age_years    = samples_track_age_years[i];

			data.samples_track_emotions_set[i]            = samples_track_emotions_set[i];

			const int emotions_count = samples_track_emotions_count[i];

			for(int k = 0; k < emotions_count; ++k, ++emotions_i)
			{
				pbio::EmotionsEstimator::EmotionConfidence ec;

				ec.emotion = (EmotionsEstimator::Emotion) samples_track_emotions_emotion[emotions_i];
				ec.confidence = samples_track_emotions_confidence[emotions_i];

				data.samples_track_emotions[i].push_back(ec);
			}
		}


		for(int i = 0; i < u_callbacks_count; ++i)
		{
			try
			{
				const TrackingCallbackU u_callback_function_i =
					reinterpret_cast<TrackingCallbackU>(u_callbacks_func[i]);

				u_callback_function_i(
					data,
					u_callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TrackingCallbackU")
		}


		for(int i = 0; i < callbacks_count; ++i)
		{
			try
			{
				const TrackingCallbackFunc callback_function_i =
					reinterpret_cast<TrackingCallbackFunc>(callbacks_func[i]);

				callback_function_i(
					data.stream_id,
					data.frame_id,
					data.samples,
					data.samples_weak,
					data.samples_quality,
					callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TrackingCallback")
		}
	}
	__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TrackingCallback_")
}




// static
inline
void VideoWorker::STemplateCreatedCallback(
	void* err_stream,
	void* this_vw__,
	void const* callback_data,

	const int32_t callbacks_count,
	void* const* const callbacks_func,
	void* const* const callbacks_userdata,
	const int32_t u_callbacks_count,
	void* const* const u_callbacks_func,
	void* const* const u_callbacks_userdata)
{
	try
	{
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__);

		void* exception = NULL;

		const int64_t stream_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_stream_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t frame_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_frame_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const double quality = this_vw._dll_handle->StructStorage_get_double(
			callback_data,
			StructStorageFields::video_worker_quality_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		pbio::facerec::RawSampleImpl* const sample_impl = static_cast<pbio::facerec::RawSampleImpl* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		pbio::facerec::TemplateImpl* const templ_impl = static_cast<pbio::facerec::TemplateImpl* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_templ_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		const RawSample::Ptr sample( RawSample::Ptr::make(this_vw._dll_handle, sample_impl));

		const Template::Ptr templ( Template::Ptr::make(this_vw._dll_handle, templ_impl));

		TemplateCreatedCallbackData data;

		data.stream_id = stream_id;
		data.frame_id  = frame_id;
		data.quality   = quality;
		data.sample    = sample;
		data.templ     = templ;




		for(int i = 0; i < u_callbacks_count; ++i)
		{
			try
			{
				const TemplateCreatedCallbackU u_callback_function_i =
					reinterpret_cast<TemplateCreatedCallbackU>(u_callbacks_func[i]);

				u_callback_function_i(
					data,
					u_callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TemplateCreatedCallbackU")
		}



		for(int i = 0; i < callbacks_count; ++i)
		{
			try
			{
				const TemplateCreatedCallbackFunc callback_function_i =
					reinterpret_cast<TemplateCreatedCallbackFunc>(callbacks_func[i]);

				callback_function_i(
					data.stream_id,
					data.frame_id,
					data.sample,
					data.quality,
					data.templ,
					callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TemplateCreatedCallback")
		}


	}
	__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TemplateCreatedCallback_")
}





// static
inline
void VideoWorker::SMatchFoundCallback(
	void* err_stream,
	void* this_vw__,
	void const* callback_data,

	const int32_t callbacks_count,
	void* const* const callbacks_func,
	void* const* const callbacks_userdata,
	const int32_t ext_callbacks_count,
	void* const* const ext_callbacks_func,
	void* const* const ext_callbacks_userdata,
	const int32_t u_callbacks_count,
	void* const* const u_callbacks_func,
	void* const* const u_callbacks_userdata)
{
	try
	{
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__);

		void* exception = NULL;

		const int64_t stream_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_stream_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t frame_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_frame_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const double quality = this_vw._dll_handle->StructStorage_get_double(
			callback_data,
			StructStorageFields::video_worker_quality_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		pbio::facerec::RawSampleImpl* const sample_impl = static_cast<pbio::facerec::RawSampleImpl* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		pbio::facerec::TemplateImpl* const templ_impl = static_cast<pbio::facerec::TemplateImpl* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_templ_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		const int64_t search_result_size = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_search_result_size_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		uint64_t const* const search_result_person_ids = static_cast<uint64_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_search_result_person_ids_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		uint64_t const* const search_result_element_ids = static_cast<uint64_t const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_search_result_element_ids_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		float const* const search_result_match_result_distances = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_search_result_match_result_distance_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		float const* const search_result_match_result_far_s = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_search_result_match_result_fa_r_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		float const* const search_result_match_result_frr_s = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_search_result_match_result_fr_r_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		float const* const search_result_match_result_scores = static_cast<float const* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_search_result_match_result_score_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);


		const RawSample::Ptr sample( RawSample::Ptr::make(this_vw._dll_handle, sample_impl));

		const Template::Ptr templ( Template::Ptr::make(this_vw._dll_handle, templ_impl));

		std::vector<SearchResult> search_results(search_result_size);

		for(int i = 0; i < search_result_size; ++i)
		{
			search_results[i].person_id = search_result_person_ids[i];
			search_results[i].element_id = search_result_element_ids[i];
			search_results[i].match_result.distance = search_result_match_result_distances[i];
			search_results[i].match_result.fa_r = search_result_match_result_far_s[i];
			search_results[i].match_result.fr_r = search_result_match_result_frr_s[i];
			search_results[i].match_result.score = search_result_match_result_scores[i];
		}

		MatchFoundCallbackData data;
		data.stream_id = stream_id;
		data.frame_id  = frame_id;
		data.quality   = quality;
		data.sample    = sample;
		data.templ     = templ;
		data.search_results = search_results;





		for(int i = 0; i < u_callbacks_count; ++i)
		{
			try
			{
				const MatchFoundCallbackU u_callback_function_i =
					reinterpret_cast<MatchFoundCallbackU>(u_callbacks_func[i]);

				u_callback_function_i(
					data,
					u_callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("MatchFoundCallbackU")
		}


		for(int i = 0; i < ext_callbacks_count; ++i)
		{
			try
			{
				const MatchFoundCallbackFuncExt ext_callback_function_i =
					reinterpret_cast<MatchFoundCallbackFuncExt>(ext_callbacks_func[i]);

				ext_callback_function_i(
					data.stream_id,
					data.frame_id,
					data.sample,
					data.quality,
					data.templ,
					data.search_results,
					ext_callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("MatchFoundCallbackExt")
		}


		for(int i = 0; i < callbacks_count; ++i)
		{
			try
			{
				const MatchFoundCallbackFunc callback_function_i =
					reinterpret_cast<MatchFoundCallbackFunc>(callbacks_func[i]);

				callback_function_i(
					data.stream_id,
					data.frame_id,
					data.search_results[0].person_id,
					data.search_results[0].element_id,
					data.sample,
					data.quality,
					data.templ,
					data.search_results[0].match_result,
					callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("MatchFoundCallback")
		}

	}
	__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("MatchFoundCallback_")
}


// static
inline
void VideoWorker::SStiPersonOutdatedCallback(
	void* err_stream,
	void* this_vw__,
	void const* callback_data,

	const int32_t callbacks_count,
	void* const* const callbacks_func,
	void* const* const callbacks_userdata)
{
	try
	{
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__);

		void* exception = NULL;

		const int64_t stream_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_stream_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t sti_person_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_sti_person_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);


		StiPersonOutdatedCallbackData data;
		data.stream_id = stream_id;
		data.sti_person_id = sti_person_id;

		for(int i = 0; i < callbacks_count; ++i)
		{
			try
			{
				const StiPersonOutdatedCallbackU callback_function_i =
					reinterpret_cast<StiPersonOutdatedCallbackU>(callbacks_func[i]);

				callback_function_i(
					data,
					callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("StiPersonOutdated")
		}
	}
	__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("StiPersonOutdated")
}

// static
inline
void VideoWorker::STrackingLostCallback(
	void* err_stream,
	void* this_vw__,
	void const* callback_data,

	const int32_t callbacks_count,
	void* const* const callbacks_func,
	void* const* const callbacks_userdata,
	const int32_t u_callbacks_count,
	void* const* const u_callbacks_func,
	void* const* const u_callbacks_userdata)
{
	try
	{
		const VideoWorker &this_vw = *reinterpret_cast<VideoWorker const*>(this_vw__);

		void* exception = NULL;

		const int64_t stream_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_stream_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t first_frame_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_first_frame_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t last_frame_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_last_frame_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const double best_quality = this_vw._dll_handle->StructStorage_get_double(
			callback_data,
			StructStorageFields::video_worker_best_quality_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		const int64_t best_quality_frame_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_best_qaulity_frame_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);

		pbio::facerec::RawSampleImpl* const best_quality_sample_impl = static_cast<pbio::facerec::RawSampleImpl* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_samples_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		const RawSample::Ptr sample =
			best_quality_sample_impl ?
				RawSample::Ptr::make(this_vw._dll_handle, best_quality_sample_impl) :
				RawSample::Ptr();


		pbio::facerec::TemplateImpl* const best_quality_template_impl = static_cast<pbio::facerec::TemplateImpl* const>(
			this_vw._dll_handle->StructStorage_get_pointer(
				callback_data,
				StructStorageFields::video_worker_templ_t,
				&exception));

		checkException(exception, *this_vw._dll_handle);

		const Template::Ptr templ =
			best_quality_template_impl ?
				Template::Ptr( Template::Ptr::make(this_vw._dll_handle, best_quality_template_impl)) :
				Template::Ptr();


		const int64_t track_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_track_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);


		const int64_t sti_person_id = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_sti_person_id_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);


		const bool sti_person_id_set = this_vw._dll_handle->StructStorage_get_int64(
			callback_data,
			StructStorageFields::video_worker_sti_person_id_set_t,
			&exception);

		checkException(exception, *this_vw._dll_handle);



		TrackingLostCallbackData data;
		data.stream_id = stream_id;
		data.first_frame_id = first_frame_id;
		data.last_frame_id = last_frame_id;
		data.best_quality = best_quality;
		data.best_quality_frame_id = best_quality_frame_id;
		data.best_quality_sample = sample;
		data.best_quality_templ = templ;
		data.track_id = track_id;
		data.sti_person_id = sti_person_id;
		data.sti_person_id_set = sti_person_id_set;





		for(int i = 0; i < u_callbacks_count; ++i)
		{
			try
			{
				const TrackingLostCallbackU u_callback_function_i =
					reinterpret_cast<TrackingLostCallbackU>(u_callbacks_func[i]);

				u_callback_function_i(
					data,
					u_callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TrackingLostCallbackU")
		}


		for(int i = 0; i < callbacks_count; ++i)
		{
			try
			{
				const TrackingLostCallbackFunc callback_function_i =
					reinterpret_cast<TrackingLostCallbackFunc>(callbacks_func[i]);

				callback_function_i(
					data.stream_id,
					data.first_frame_id,
					data.last_frame_id,
					data.best_quality,
					data.best_quality_frame_id,
					data.best_quality_sample,
					data.best_quality_templ,
					callbacks_userdata[i]);
			}
			__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TrackingLostCallback")
		}

	}
	__0x6ce24ef9_VideoWorker_static_callback_functions_catch_exceptions("TrackingLostCallback_")
}


inline
void VideoWorker::toggleSomething(
	const int stream_id,
	const int something)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_toggleSomething(
		_impl,
		stream_id,
		something,
		&exception);

	checkException(exception, *_dll_handle);
}


inline
void VideoWorker::disableProcessingOnStream(const int stream_id)
{
	toggleSomething(stream_id, pbio::facerec::capi::VideoWorker_something_Processing_Disable);
}


inline
void VideoWorker::enableProcessingOnStream(const int stream_id)
{
	toggleSomething(stream_id, pbio::facerec::capi::VideoWorker_something_Processing_Enable);
}


inline
void VideoWorker::disableAgeGenderEstimationOnStream(const int stream_id)
{
	toggleSomething(stream_id, pbio::facerec::capi::VideoWorker_something_AgeGender_Disable);
}

inline
void VideoWorker::enableAgeGenderEstimationOnStream(const int stream_id)
{
	toggleSomething(stream_id, pbio::facerec::capi::VideoWorker_something_AgeGender_Enable);
}

inline
void VideoWorker::disableEmotionsEstimationOnStream(const int stream_id)
{
	toggleSomething(stream_id, pbio::facerec::capi::VideoWorker_something_Emotions_Disable);
}

inline
void VideoWorker::enableEmotionsEstimationOnStream(const int stream_id)
{
	toggleSomething(stream_id, pbio::facerec::capi::VideoWorker_something_Emotions_Enable);
}




inline
VideoWorker::Params::Params()
: _video_worker_config(std::string())
, _recognizer_ini_file()
, _streams_count(0)
, _processing_threads_count(0)
, _matching_threads_count(0)
, _age_gender_estimation_threads_count(0)
, _emotions_estimation_threads_count(0)
, _short_time_identification_enabled(false)
, _short_time_identification_distance_threshold(0)
, _short_time_identification_outdate_time_seconds(0)
{
	// nothing else
}


inline
void VideoWorker::setParameter(
	const std::string parameter,
	const double value)
{
	void* exception = NULL;

	_dll_handle->VideoWorker_setParameter(
		_impl,
		parameter.c_str(),
		value,
		&exception);

	checkException(exception, *_dll_handle);
}


}  // pbio namespace


#endif  // __PBIO_API__PBIO__VIDEOWORKER_H_
