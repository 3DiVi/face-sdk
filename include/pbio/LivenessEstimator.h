/**
	\file LivenessEstimator.h
	\~English
	\brief LivenessEstimator - Interface object used to estimate face liveness to prevent spoofing attacks.
	\~Russian
	\brief LivenessEstimator - Интерфейсный объект для определения принадлежности лица реальному человеку.
*/

#ifndef __PBIO_API__PBIO__LIVENESS_ESTIMATOR_H_
#define __PBIO_API__PBIO__LIVENESS_ESTIMATOR_H_

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
	\brief Interface object used to estimate face liveness to prevent spoofing attacks.
	\~Russian
	\brief Интерфейсный объект для определения принадлежности лица реальному человеку.
*/
class LivenessEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to LivenessEstimator.
		\~Russian
		\brief  Псевдоним для типа умного указателя на LivenessEstimator.
	*/
	typedef LightSmartPtr<LivenessEstimator>::tPtr Ptr;

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
			\brief The observed face belongs to a living person.
			\~Russian
			\brief Лицо принадлежит реальному человеку.
		*/
		REAL = 1,  

		/** \~English
			\brief The observed face is taken from a photo.
			\~Russian
			\brief Лицо взято с фото.
		*/
		FAKE = 2              
	};



	/**
		\~English
		\brief
			Add a sample of the observed face from the new frame.

		\param[in]  sample
			Face sample.

		\~Russian
		\brief
			Добавить образец наблюдаемого лица с нового кадра.

		\param[in]  sample
			Образец лица с нового кадра.
	*/
	void addSample(const pbio::RawSample&);


	/**
		\~English
		\brief
			Estimate liveness of the observed face.

		\return
			Estimated liveness.

		\~Russian
		\brief
			Определить принадлежность лица реальному человеку.

		\return
			Результат (см. Liveness).
	*/
	Liveness estimateLiveness();

private:

	LivenessEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<LivenessEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
LivenessEstimator::LivenessEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
LivenessEstimator::Liveness LivenessEstimator::estimateLiveness()
{
	void* exception = NULL;

	const int result = _dll_handle->LivenessEstimator_estimateLiveness(
		_impl,
		&exception);

	checkException(exception, *_dll_handle);

	if(result == 1)
		return REAL;

	if(result == 2)
		return FAKE;

	return NOT_ENOUGH_DATA;
}



inline
void LivenessEstimator::addSample(const pbio::RawSample &sample)
{
	void* exception = NULL;

	_dll_handle->LivenessEstimator_addSample(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&exception);

	checkException(exception, *_dll_handle);
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__LIVENESS_ESTIMATOR_H_
