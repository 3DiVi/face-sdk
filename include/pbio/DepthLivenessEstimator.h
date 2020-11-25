/**
	\file DepthLivenessEstimator.h
	\~English
	\brief DepthLivenessEstimator - Interface object used to estimate face liveness in order to prevent spoofing attacks.
	\~Russian
	\brief DepthLivenessEstimator - Интерфейсный объект для определения принадлежности лица реальному человеку.
*/

#ifndef __PBIO_API__PBIO__DEPTH_LIVENESS_ESTIMATOR_H_
#define __PBIO_API__PBIO__DEPTH_LIVENESS_ESTIMATOR_H_

#include <sstream>
#include <vector>


#include "ComplexObject.h"
#include "DepthMapRaw.h"
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
class DepthLivenessEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to DepthLivenessEstimator.
		\~Russian
		\brief Псевдоним для типа умного указателя на DepthLivenessEstimator.
	*/
	typedef LightSmartPtr<DepthLivenessEstimator>::tPtr Ptr;

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
			Estimates liveness of the observed face.
			The given depth map must be synchronized and aligned 
			with the original image from which the sample was captured.

		\param[in]  sample
			Face sample.

		\param[in]  depth_map
			Depth map.

		\return
			Estimated liveness (see DepthLivenessEstimator.Liveness for details).

		\~Russian
		\brief
			Определить принадлежность лица реальному человеку.
			Карта глубины должна быть синхронизирована и отрегистрирована 
			в соответствии с исходным изображением, на котором было обнаружено лицо.

		\param[in]  sample
			Образец лица.

		\param[in]  depth_map
			Карта глубины.

		\return
			Результат (см. DepthLivenessEstimator::Liveness).
	*/
	Liveness estimateLiveness(
		const pbio::RawSample& sample,
		const pbio::DepthMapRaw depth_map);

private:

	DepthLivenessEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<DepthLivenessEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
DepthLivenessEstimator::DepthLivenessEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
DepthLivenessEstimator::Liveness DepthLivenessEstimator::estimateLiveness(
	const pbio::RawSample& sample,
	const pbio::DepthMapRaw depth_map)
{
	void* exception = NULL;

	const int result = _dll_handle->DepthLivenessEstimator_estimateLiveness(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		depth_map.depth_map_rows,
		depth_map.depth_map_cols,
		depth_map.depth_map_2_image_offset_y,
		depth_map.depth_map_2_image_offset_x,
		depth_map.depth_map_2_image_scale_x,
		depth_map.depth_map_2_image_scale_y,
		depth_map.horizontal_fov,
		depth_map.vertical_fov,
		depth_map.depth_unit_in_millimeters,
		depth_map.depth_data,
		depth_map.depth_data_stride_in_bytes,
		&exception);

	checkException(exception, *_dll_handle);

	if(result == 1)
		return REAL;

	if(result == 2)
		return FAKE;

	return NOT_ENOUGH_DATA;
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__DEPTH_LIVENESS_ESTIMATOR_H_
