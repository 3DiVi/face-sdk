/**
	\file AgeGenderEstimator.h
	\~English
	\brief AgeGenderEstimator - interface object for age and gender estimation
		\warning
		This is a deprecated version (see the new Age and Gender estimators in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief AgeGenderEstimator - интерфейсный объект для определения пола и возраста
		\warning
		Это устаревшая версия (см. новые блоки в Processing Block API). Поддержка будет прекращена в 2024 году.
*/

#ifndef __PBIO_API__PBIO__AGE_GENDER_ESTIMATOR_H_
#define __PBIO_API__PBIO__AGE_GENDER_ESTIMATOR_H_

#include <sstream>
#include <vector>


#include "ComplexObject.h"
#include "RawSample.h"
#include "SmartPtr.h"
#include "Error.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"


/** \~English
	\brief C++ interface
	\~Russian
	\brief C++ интерфейс
*/
namespace pbio
{

class FacerecService;

/** \~English
	\brief Interface object for age and gender estimation. 
		\warning
		This is a deprecated version (see the new Age and Gender estimators in Processing Block API). It's support will end in 2024.
	\~Russian
	\brief Интерфейсный объект для определения пола и возраста.
		\warning
		Это устаревшая версия (см. новые блоки в Processing Block API). Поддержка будет прекращена в 2024 году.
*/
class AgeGenderEstimator : public ComplexObject
{
public:

	/** \~English  
		\brief Alias for the type of a smart pointer to AgeGenderEstimator.
		\~Russian
		\brief Псевдоним для типа умного указателя на AgeGenderEstimator.
	*/
	typedef LightSmartPtr<AgeGenderEstimator>::tPtr Ptr;

	/** \~English
		\brief Age groups.
		\~Russian
		\brief Возрастные группы.
	*/
	enum Age
	{
		/** \~English
			\brief Age in the range of [0; 18) years.
			\~Russian
			\brief Возраст в диапазоне [0; 18).
		*/
		AGE_KID = 0, 

		/** \~English
			\brief Age in the range of [18; 37) years.
			\~Russian
			\brief Возраст в диапазоне [18; 37).
		*/
		AGE_YOUNG = 1,

		/** \~English
			\brief Age in the range of [37; 55) years.
			\~Russian
			\brief Возраст в диапазоне [37; 55).
		*/
		AGE_ADULT = 2,

		/** \~English
			\brief Age in the range of [55; inf) years.
			\~Russian
			\brief Возраст в диапазоне [55; inf).
		*/
		AGE_SENIOR = 3  
	};

	/** \~English
		\brief Gender groups.
		\~Russian
		\brief Пол.
	*/
	enum Gender
	{
		/** \~English
			\brief Female. 
			\~Russian
			\brief Женский.
		*/
		GENDER_FEMALE = 0,

		/** \~English
			\brief Male. 
			\~Russian
			\brief Мужской.
		*/
		GENDER_MALE = 1
	};

	/** \~English
		\brief Age and gender.
		\~Russian
		\brief Пол и возраст.
	*/
	struct AgeGender
	{
		/** \~English
			\brief Age.
			\~Russian
			\brief Возраст.
		*/
		Age age;

		/** \~English
			\brief Gender.
			\~Russian
			\brief Пол.
		*/
		Gender gender;  

		/** \~English
			\brief Age estmated as number.
			\~Russian
			\brief Возраст в виде числа.
		*/
		float age_years;
	};

	/** \~English
		\brief
			To estimate age and gender of a given face sample.

		\param[in]  sample
			Face sample.

		\return
			Estimated age and gender.
		
		\~Russian
		\brief
			Определить пол и возраст поданного лица.

		\param[in]  sample
			Лицо.

		\return
			Определенные значения пола и возраста.
	*/
	AgeGender estimateAgeGender(const pbio::RawSample &sample) const;

private:

	AgeGenderEstimator(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<AgeGenderEstimator>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline
AgeGenderEstimator::AgeGenderEstimator(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
AgeGenderEstimator::AgeGender AgeGenderEstimator::estimateAgeGender(const pbio::RawSample &sample) const
{
	void* exception = NULL;

	int32_t age_class, gender;
	float age_years;

	_dll_handle->AgeGenderEstimator_estimateAgeGender(
		_impl,
		(pbio::facerec::RawSampleImpl const*) sample._impl,
		&age_class,
		&gender,
		&age_years,
		&exception);

	checkException(exception, *_dll_handle);

	AgeGender result;
	result.age = (Age) age_class;
	result.gender = (Gender) gender;
	result.age_years = age_years;

	return result;
}



}  // pbio namespace

#endif  // __PBIO_API__PBIO__AGE_GENDER_ESTIMATOR_H_
