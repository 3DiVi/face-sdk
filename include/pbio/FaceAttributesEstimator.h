/**
	\file FaceAttributesEstimator.h
	\~English
	\brief FaceAttributesEstimator is an interface object used to estimate attributes of the face.
	\~Russian
	\brief FaceAttributesEstimator - интерфейсный объект для определения аттрибутов лица.
*/

#ifndef __PBIO_API__PBIO__FACE_ATTRIBUTES_ESTIMATOR_H_
#define __PBIO_API__PBIO__FACE_ATTRIBUTES_ESTIMATOR_H_


#include "ComplexObject.h"
#include "RawSample.h"
#include "SmartPtr.h"


namespace pbio
{

class FacerecService;

/** \~English
	\brief Interface object used to estimate attributes of the face.
	\~Russian
	\brief Интерфейсный объект для определения аттрибутов лица.
*/
class FaceAttributesEstimator : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to FaceAttributesEstimator.
		\~Russian
		\brief Псевдоним для типа умного указателя на FaceAttributesEstimator.
	*/
	typedef LightSmartPtr<FaceAttributesEstimator>::tPtr Ptr;

	/** \~English
		\brief The result of determining the presence of the required attribute on the face.
		\~Russian
		\brief Результат определения наличия требуемого атрибута на лице.
	*/
	struct Attribute
	{
		/** \~English
			\brief Verdict.
			\~Russian
			\brief Вердикт.
		*/
		bool verdict = false;

		/** \~English
			\brief Probability of a required attribute.
			\~Russian
			\brief Вероятность наличия атрибута.
		*/
		float score = 0.0f;

		/** \~English
			\brief Detailed description for attribute - medical mask .
			\~Russian
			\brief Детальное описание для атрибута - медицинская маска.
		*/
		enum MaskAttribute{
			NOT_COMPUTED = 0,
			NO_MASK = 1,
			HAS_MASK = 2
		} mask_attribute;
	};

	/**
		\~English
		\brief
			Estimates if there is is a required attribute on a given face sample.

		\param[in]  sample
			Face sample.

		\return
		Estimated verdict and score (see FaceAttributesEstimator.Attribute for details)..

		\~Russian
		\brief
			Оценивает, есть ли нужный аттрибут на данном сэмпле лица.

		\param[in]  sample
			Лицо.

		\return
			Определенный результат с коэффициентами уверенности (см. FaceAttributesEstimator::Attribute).
	*/
	Attribute estimate(const pbio::RawSample &sample) const;

private:

	FaceAttributesEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<FaceAttributesEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{


inline
FaceAttributesEstimator::FaceAttributesEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
FaceAttributesEstimator::Attribute FaceAttributesEstimator::estimate(const pbio::RawSample &sample) const
{
	void* exception = NULL;

	Attribute result;
	int32_t verdict;

	_dll_handle->FaceAttributesEstimator_estimate(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&verdict,
		&result.score,
		&exception);

	checkException(exception, *_dll_handle);

	result.verdict = verdict;

	result.mask_attribute = verdict ? Attribute::MaskAttribute::HAS_MASK: Attribute::MaskAttribute::NO_MASK;

	return result;
}


}  // pbio namespace

#endif  // __PBIO_API__PBIO__FACE_ATTRIBUTES_ESTIMATOR_H_
