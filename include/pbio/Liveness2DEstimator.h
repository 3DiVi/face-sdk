/**
	\file Liveness2DEstimator.h
	\~English
	\brief Liveness2DEstimator is an interface object used to estimate face liveness in order to prevent spoofing attacks.
		\warning
		This is a deprecated version (see the new one in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief Liveness2DEstimator - интерфейсный объект для определения принадлежности лица реальному человеку.
		\warning
		Это устаревшая версия (см. новый блок в Processing Block API). Поддержка будет прекращена в 2024 году.
*/

#ifndef __PBIO_API__PBIO__LIVENESS_2D_ESTIMATOR_H_
#define __PBIO_API__PBIO__LIVENESS_2D_ESTIMATOR_H_

#include <sstream>
#include <vector>


#include "ComplexObject.h"
#include "Error.h"
#include "RawSample.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"


namespace pbio
{

class FacerecService;

/** \~English
	\brief Interface object used to estimate face liveness in order to prevent spoofing attacks. 
		\warning
		This is a deprecated version (see the new one in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief Интерфейсный объект для определения принадлежности лица реальному человеку.
		\warning
		Это устаревшая версия (см. новый блок в Processing Block API). Поддержка будет прекращена в 2024 году.
*/
class Liveness2DEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to Liveness2DEstimator.
		\~Russian
		\brief Псевдоним для типа умного указателя на Liveness2DEstimator.
	*/
	typedef LightSmartPtr<Liveness2DEstimator>::tPtr Ptr;

	/** \~English
		\brief Estimated liveness.
		\~Russian
		\brief Результат определения принадлежности лица живому человеку.
	*/
	enum Liveness
	{
		/** \~English
			\brief Not enough information.
			\~Russian
			\brief Недостаточно информации.
		*/
		NOT_ENOUGH_DATA = 0, 

		/** \~English
			\brief The observed face belongs to a real person.
			\~Russian
			\brief Лицо принадлежит реальному человеку.
		*/
		REAL = 1,

		/** \~English
			\brief The observed face is taken from a photo.
			\~Russian
			\brief Лицо было взято с фото.
		*/
		FAKE = 2,

		/** \~English
			\brief The observed face was not checked.
			\~Russian
			\brief Лицо не было проверено.
		*/
		NOT_COMPUTED = 3,

		/** \~English
			\brief The observed face is out of image boundaries.
			\~Russian
			\brief Лицо выходит за рамки изображения.
		*/
		FACE_NOT_FULLY_FRAMED = 4,

		/** \~English
			\brief More than one face detected on the input image.
			\~Russian
			\brief В кадре находится больше одного лица.
		*/
		MULTIPLE_FACE_FRAMED = 5,

		/** \~English
			\brief The observed face is not frontal and turned right
			\~Russian
			\brief Лицо повернуто вправо.
		*/
		FACE_TURNED_RIGHT = 6,

		/** \~English
			\brief The observed face is not frontal and turned left
			\~Russian
			\brief Лицо повернуто влево.
		*/
		FACE_TURNED_LEFT = 7,

		/** \~English
			\brief The Observed face is not frontal and turned up
			\~Russian
			\brief Лицо повернуто вверх.
		*/
		FACE_TURNED_UP = 8,

		/** \~English
			\brief The Observed face is not frontal and turned down
			\~Russian
			\briefЛицо повернуто вниз.
		*/
		FACE_TURNED_DOWN = 9,

		/** \~English
			\brief Input image has bad lighting
			\~Russian
			\brief Недостаточные условия освещения.
		*/
		BAD_IMAGE_LIGHTING = 10,

		/** \~English
			\brief Input image is too noisy
			\~Russian
			\brief Исходное изображение слишком зашумлено.
		*/
		BAD_IMAGE_NOISE = 11,

		/** \~English
			\brief Input image is too blurry
			\~Russian
			\brief Исходное изображение слишком размыто
		*/
		BAD_IMAGE_BLUR = 12,

		/** \~English
			\brief Input image is too flared
			\~Russian
			\brief Исходное изображение слишком яркое
		*/
		BAD_IMAGE_FLARE = 13,
	};

	/** \~English
		\brief Estimated liveness and score.
		\~Russian
		\brief Результат определения и вероятность принадлежности лица живому человеку.
	*/
	struct LivenessAndScore
	{
		/** \~English
			\brief Estimated liveness (see Liveness for details).
			\~Russian
			\brief Результат определения принадлежности лица живому человеку (см. Liveness).
		*/
		Liveness liveness;

		/** \~English
			\brief Liveness score.
			\~Russian
			\brief Вероятность принадлежности лица живому человеку.
		*/
		float score;
	};


	/**
		\~English
		\brief
			Estimates liveness of an observed face.

		\param[in]  sample
			Face sample.

		\return
			Estimated liveness (see Liveness2DEstimator.Liveness for details).

		\~Russian
		\brief
			Определить принадлежность лица реальному человеку.

		\param[in]  sample
			Образец лица.

		\return
			Результат (см. Liveness2DEstimator::Liveness).
	*/
	Liveness estimateLiveness(const pbio::RawSample& sample);


	/**
		\~English
		\brief
			liveness and score of an observed face.

		\param[in]  sample
			Face sample.

		\return
			liveness and score (see LivenessAndScore for details).

		\~Russian
		\brief
			Определение и вероятность принадлежности лица реальному человеку.

		\param[in]  sample
			Образец лица.

		\return
			Результат (см. LivenessAndScore).
	*/
	LivenessAndScore estimate(const pbio::RawSample& sample);

private:

	Liveness2DEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<Liveness2DEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
Liveness2DEstimator::Liveness2DEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
Liveness2DEstimator::Liveness Liveness2DEstimator::estimateLiveness(const pbio::RawSample& sample)
{
	void* exception = NULL;
	int32_t verdict;
	float score;

	_dll_handle->Liveness2DEstimator_estimateLiveness(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&verdict,
		&score,
		&exception);

	checkException(exception, *_dll_handle);

	return static_cast<Liveness>(verdict);
}


inline
Liveness2DEstimator::LivenessAndScore Liveness2DEstimator::estimate(const pbio::RawSample& sample)
{
	void* exception = NULL;
	int32_t verdict;
	float score;

	_dll_handle->Liveness2DEstimator_estimateLiveness(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&verdict,
		&score,
		&exception);

	checkException(exception, *_dll_handle);

	LivenessAndScore result;
	result.liveness = static_cast<Liveness>(verdict);
	result.score = score;

	return result;
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__LIVENESS_2D_ESTIMATOR_H_
