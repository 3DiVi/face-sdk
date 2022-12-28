/**
	\file EmotionsEstimator.h
	\~English
	\brief EmotionsEstimator - Interface object for estimation of emotions.
		\warning
		This is a deprecated version (see the new one in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief EmotionsEstimator - интерфейсный объект для определения эмоций.
		\warning
		Это устаревшая версия (см. новый блок в Processing Block API). Поддержка будет прекращена в 2024 году.
*/

#ifndef __PBIO_API__PBIO__EMOTIONS_ESTIMATOR_H_
#define __PBIO_API__PBIO__EMOTIONS_ESTIMATOR_H_

#include <sstream>
#include <vector>


#include "ComplexObject.h"
#include "RawSample.h"
#include "SmartPtr.h"
#include "Error.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"


namespace pbio
{

class FacerecService;

/** \~English
	\brief Interface object for estimation of emotions.
		\warning
		This is a deprecated version (see the new one in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief Интерфейсный объект для определения эмоций.
		\warning
		Это устаревшая версия (см. новый блок в Processing Block API). Поддержка будет прекращена в 2024 году.
*/
class EmotionsEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to EmotionsEstimator.
		\~Russian
		\brief Псевдоним для типа умного указателя на EmotionsEstimator.
	*/
	typedef LightSmartPtr<EmotionsEstimator>::tPtr Ptr;

	/** \~English
		\brief Emotions.
		\~Russian
		\brief Эмоции.
	*/
	enum Emotion
	{
		/** \~English
			\brief Neutral.
			\~Russian
			\brief Нейтральный.
		*/
		EMOTION_NEUTRAL = 0,

		/** \~English
			\brief Happy.
			\~Russian
			\brief Счастливый.
		*/
		EMOTION_HAPPY = 1,
		
		/** \~English
			\brief Angry.
			\~Russian
			\brief Злой.
		*/
		EMOTION_ANGRY = 2,

		/** \~English
			\brief Surprised.
			\~Russian
			\brief Удивленный.
		*/
		EMOTION_SURPRISE = 3,

		/** \~English
			\brief Disgusted.
			\~Russian
			\brief Испытывающий отвращение.
		*/
		EMOTION_DISGUSTED = 4,

		/** \~English
			\brief Scared.
			\~Russian
			\brief Напуганный.
		*/
		EMOTION_SCARED = 5,

		/** \~English
			\brief Sad.
			\~Russian
			\brief Грустный.
		*/
		EMOTION_SAD = 6,
	};

	/** \~English
	 	\brief Emotion with confidence.
		\~Russian
		\brief Эмоция с коэффициентом уверенности.
	*/
	struct EmotionConfidence
	{
		/** \~English
			\brief Emotion.
			\~Russian
			\brief Эмоция.
		*/
		Emotion emotion;

		/** \~English
			\brief Positive real number in the range of [0; 1].
			\~Russian
			\brief Положительное вещественное число из диапазона [0; 1].
		*/
		float confidence;
	};

	/**
		\~English \brief
			Vector of estimated emotions with confidence coefficients.
			In descending confidence order.
			The sum of all confidence coefficients equals 1.

		\~Russian \brief
			Определенные эмоции с коэффициентами уверенности.
			В порядке уменьшения уверенности.
			Сумма всех коэффициентов равна 1.
	*/
	typedef std::vector<EmotionConfidence> EstimatedEmotionsVector;

	/**
		\~English
		\brief
			Estimates the emotion of a given face sample.

		\param[in]  sample
			Face sample.

		\return
		Vector of estimated emotions with confidence coefficients.
		In descending confidence order.
		The sum of all confidence coefficients equals 1.

		\~Russian
		\brief
			Определить эмоцию поданного лица.

		\param[in]  sample
			Лицо.

		\return
			Определенные эмоции с коэффициентами уверенности.
			В порядке уменьшения уверенности.
			Сумма всех коэффициентов равна 1.
	*/
	EstimatedEmotionsVector estimateEmotions(const pbio::RawSample &sample) const;

private:

	EmotionsEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<EmotionsEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
EmotionsEstimator::EmotionsEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
std::vector<EmotionsEstimator::EmotionConfidence> EmotionsEstimator::estimateEmotions(
	const pbio::RawSample &sample) const
{
	void* exception = NULL;

	std::ostringstream data_oss;

	stl_wraps::WrapOStreamImpl data_oss_w(data_oss);

	const int count = _dll_handle->EmotionsEstimator_estimateEmotions(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&data_oss_w,
		pbio::stl_wraps::WrapOStreamImpl::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	std::istringstream data_iss(data_oss.str());

	std::vector<EmotionConfidence> result(count);

	for(int i = 0; i < count; ++i)
	{
		int32_t emotion;
		float confidence;

		data_iss.read((char*)&emotion, sizeof(emotion));
		data_iss.read((char*)&confidence, sizeof(confidence));

		result[i].emotion = (Emotion) emotion;
		result[i].confidence = confidence;
	}

	return result;
}



}  // pbio namespace

#endif  // __PBIO_API__PBIO__EMOTIONS_ESTIMATOR_H_
