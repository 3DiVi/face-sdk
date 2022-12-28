/**
	\file FaceQualityEstimator.h
	\~English
	\brief FaceQualityEstimator - Interface object for sample quality estimation.
		\warning
		This is a deprecated version (see the new one in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief FaceQualityEstimator - Интерфейсный объект для определения качества образца лица.
		\warning
		Это устаревшая версия (см. новый блок в Processing Block API). Поддержка будет прекращена в 2024 году.
*/

#ifndef __PBIO_API__PBIO__FACE_QUALITY_ESTIMATOR_H_
#define __PBIO_API__PBIO__FACE_QUALITY_ESTIMATOR_H_

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
	\brief Interface object for sample quality estimation.
		\warning
		This is a deprecated version (see the new one in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief Интерфейсный объект для определения качества образца лица.
		\warning
		Это устаревшая версия (см. новый блок в Processing Block API). Поддержка будет прекращена в 2024 году.
*/
class FaceQualityEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to FaceQualityEstimator.
		\~Russian
		\brief  Псевдоним для типа умного указателя на FaceQualityEstimator.
	*/
	typedef LightSmartPtr<FaceQualityEstimator>::tPtr Ptr;


	/**
		\~English
		\brief
			Estimates quality of a given face sample.

		\param[in]  sample
			Face sample.

		\return
			Estimated quality value (the greater the value is, the better the quality is).
			Aggregates sample usability (i.e. pose, occlusion, noise, blur and lighting)
			for face recognition in a single real value.

		\~Russian
		\brief
			Определить качество поданного образца лица.

		\param[in]  sample
			Образец лица.

		\return
			Определенное качество - вещественное число (чем больше, тем выше качество),
			агрегирующее качество освещения, шум, размытие и ракурс.
	*/
	float estimateQuality(const pbio::RawSample&) const;

private:

	FaceQualityEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<FaceQualityEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
FaceQualityEstimator::FaceQualityEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
float FaceQualityEstimator::estimateQuality(const pbio::RawSample &sample) const
{
	void* exception = NULL;

	const float result = _dll_handle->FaceQualityEstimator_estimateQuality(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}



}  // pbio namespace

#endif  // __PBIO_API__PBIO__FACE_QUALITY_ESTIMATOR_H_
