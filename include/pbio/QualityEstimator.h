/**
	\file QualityEstimator.h
	\~English
	\brief QualityEstimator - Interface object used to estimate sample quality.
	\~Russian
	\brief QualityEstimator - Интерфейсный объект для определения качества образца лица.
*/

#ifndef __PBIO_API__PBIO__QUALITY_ESTIMATOR_H_
#define __PBIO_API__PBIO__QUALITY_ESTIMATOR_H_

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
	\brief Interface object used to estimate sample quality.
	\~Russian
	\brief Интерфейсный объект для определения качества образца лица.
*/
class QualityEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to QualityEstimator.
		\~Russian
		\brief Псевдоним для типа умного указателя на QualityEstimator.
	*/
	typedef LightSmartPtr<QualityEstimator>::tPtr Ptr;

	/** \~English
		\brief
			Set of quality parameters.
			Each parameter is in range 1 to 5.
			'1' stands for 'best qualit', '5' stands for 'worst quality'.

		\~Russian
		\brief
			Набор характеристик качества.
			Каждая характеристика находится в диапазоне от 1 до 5.
			'1' означает 'лучшее качество', '5' - 'худшее качество'.
	*/
	struct Quality
	{
		/** \~English
			\brief Total quality score.
			\~Russian
			\brief Показатель общего качества.
		*/
		int total;

		/** \~English
			\brief Lighting uniformity.
			\~Russian
			\brief Равномерность освещения.
		*/
		int lighting;

		/** \~English
			\brief Noise.
			\~Russian
			\brief Зашумленность.
		*/
		int noise;

		/** \~English
			\brief Sharpness.
			\~Russian
			\brief Резкость.
		*/
		int sharpness;

		/** \~English
			\brief Flare.
			\~Russian
			\brief Засветка.
		*/
		int flare;
	};


	/**
		\~English
		\brief
			Estimates quality of a given face sample.

		\param[in]  sample
			Face sample.

		\return
			Estimated quality.

		\~Russian
		\brief
			Определить качество поданного образца лица.

		\param[in]  sample
			Образец лица.

		\return
			Определенное качество.
	*/
	Quality estimateQuality(const pbio::RawSample&) const;

private:

	QualityEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<QualityEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
QualityEstimator::QualityEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
QualityEstimator::Quality QualityEstimator::estimateQuality(const pbio::RawSample &sample) const
{
	void* exception = NULL;

	Quality result;

	_dll_handle->QualityEstimator_estimateQuality(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&result.lighting,
		&result.noise,
		&result.sharpness,
		&result.flare,
		&exception);

	// total is the worst quality, i.e. the max value

	result.total = 1;
	result.total = std::max<int>(result.total, result.lighting);
	result.total = std::max<int>(result.total, result.noise);
	result.total = std::max<int>(result.total, result.sharpness);
	result.total = std::max<int>(result.total, result.flare);

	checkException(exception, *_dll_handle);

	return result;
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__QUALITY_ESTIMATOR_H_
