/**
	\file Liveness2DEstimator.h
	\~English
	\brief Liveness2DEstimator is an interface object used to estimate face liveness in order to prevent spoofing attacks.
	\~Russian
	\brief Liveness2DEstimator - интерфейсный объект для определения принадлежности лица реальному человеку.
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
	\~Russian
	\brief Интерфейсный объект для определения принадлежности лица реальному человеку.
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
		NOT_COMPUTED    = 3
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

	const int result = _dll_handle->Liveness2DEstimator_estimateLiveness(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&exception);

	checkException(exception, *_dll_handle);

	if(result == 1)
		return REAL;

	if(result == 2)
		return FAKE;

	return NOT_ENOUGH_DATA;
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__LIVENESS_2D_ESTIMATOR_H_
