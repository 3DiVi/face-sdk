##
#  \file age_gender_estimator.py
#  \~English
#      \brief AgeGenderEstimator - interface object for age and gender estimation
#        \warning
#        This is a deprecated version (see the new Age and Gender estimators in Processing Block API). It's support will end in 2024.
#  \~Russian
#      \brief AgeGenderEstimator - интерфейсный объект для определения пола и возраста
#        \warning
#        Это устаревшая версия (см. новые блоки в Processing Block API). Поддержка будет прекращена в 2024 году.

from enum import Enum
from ctypes import c_int, c_void_p, c_float, byref

from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from . import get_repr

## @defgroup PythonAPI
#  @{
#  \~English
#    \brief Python API interface.
#  \~Russian
#    \brief Python API интерфейс.


## @defgroup AgeGenderEstimator
#  @{


##
#  \~English
#    \brief Age groups.
#  \~Russian
#    \brief Возрастные группы.
class Age(Enum):

    ##
    #  \~English
    #      \brief Age in the range of [0; 18) years.
    #  \~Russian
    #      \brief Возраст в диапазоне [0; 18).
    AGE_KID = 0

    ##
    #  \~English
    #      \brief Age in the range of [18; 37) years.
    #  \~Russian
    #      \brief Возраст в диапазоне [18; 37).
    AGE_YOUNG = 1

    ##
    #  \~English
    #      \brief Age in the range of [37; 55) years.
    #  \~Russian
    #      \brief Возраст в диапазоне [37; 55).
    AGE_ADULT = 2

    ##
    #  \~English
    #      \brief Age in the range of [55; inf) years.
    #  \~Russian
    #      \brief Возраст в диапазоне [55; inf).
    AGE_SENIOR = 3


##
#  \~English
#    \brief Gender groups.
#  \~Russian
#    \brief Пол.
class Gender(Enum):

    ##
    #  \~English
    #      \brief Female.
    #  \~Russian
    #      \brief Женский.
    GENDER_FEMALE = 0

    ##
    #  \~English
    #      \brief Male.
    #  \~Russian
    #      \brief Мужской.
    GENDER_MALE = 1


##
#  \~English
#    \brief Age and gender.
#  \~Russian
#    \brief Пол и возраст.
class AgeGender:

    ##
    #  \~English
    #      \brief Age.
    #  \~Russian
    #      \brief Возраст.
    age: Age

    ##
    #  \~English
    #      \brief Gender.
    #  \~Russian
    #      \brief Пол.
    gender: Gender

    ##
    #  \~English
    #      \brief Age estmated as number.
    #  \~Russian
    #      \brief Возраст в виде числа.
    age_years: float

    def __init__(self, age, gender, age_years: float):
        self.age = age
        self.gender = gender
        self.age_years = age_years

    def __repr__(self):
        return get_repr(self)


##
#  \~English
#    \brief Interface object for age & gender estimation.
#        \warning
#        This is a deprecated version (see the new Age and Gender estimators in Processing Block API). It's support will end in 2024.
#  \~Russian
#    \brief Интерфейсный объект для возраста и пола.
#        \warning
#        Это устаревшая версия (см. новые блоки в Processing Block API). Поддержка будет прекращена в 2024 году.

class AgeGenderEstimator(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(AgeGenderEstimator, self).__init__(dll_handle, impl)

    ##
    #  \~English
    #    \brief To estimate age and gender of a given face sample.
    #    \param[in]  sample Face sample.
    #    \return Estimated age and gender.
    #
    #  \~Russian
    #     \brief Определить пол и возраст поданного лица.
    #     \param[in]  sample Лицо.
    #     \return Определенные значения пола и возраста.
    def estimate_age_gender(self, sample: RawSample) -> AgeGender:
        exception = make_exception()

        age_class = c_int()
        gender = c_int()
        age_years = c_float()

        self._dll_handle.AgeGenderEstimator_estimateAgeGender(
            self._impl,
            sample._impl,
            byref(age_class),
            byref(gender),
            byref(age_years),
            exception)

        check_exception(exception, self._dll_handle)

        return AgeGender(age_class.value, gender.value, age_years.value)
