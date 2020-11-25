##
#  \file liveness_2d_estimator.py
#  \~English
#      \brief Liveness2DEstimator is an interface object used to estimate face liveness in order to prevent spoofing attacks.
#  \~Russian
#      \brief Liveness2DEstimator - интерфейсный объект для определения принадлежности лица реальному человеку.

from ctypes import c_void_p

from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from enum import Enum


##
#  \~English Interface object used to estimate face liveness in order to prevent spoofing attacks.
#     \brief
#  \~Russian
#     \brief Интерфейсный объект для определения принадлежности лица реальному человеку.
class Liveness(Enum):

    ##
    #  \~English Not enough information.
    #     \brief
    #  \~Russian
    #     \brief Недостаточно информации.
    NOT_ENOUGH_DATA = 0

    ##
    #  \~English The observed face belongs to a real person.
    #     \brief
    #  \~Russian
    #     \brief Лицо принадлежит реальному человеку.
    REAL = 1

    ##
    #  \~English The observed face is taken from a photo.
    #     \brief
    #  \~Russian
    #     \brief Лицо было взято с фото.
    FAKE = 2

    ##
    #  \~English The observed face was not checked.
    #     \brief
    #  \~Russian
    #     \brief Лицо не было проверено.
    NOT_COMPUTED = 3


##
# \~English
#    \brief Interface object used to estimate face liveness in order to prevent spoofing attacks.
# \~Russian
#    \brief Интерфейсный объект для определения принадлежности лица реальному человеку.
class Liveness2DEstimator(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super().__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief liveness of an observed face.
    #
    #    \param[in] sample
    #      Face sample.
    #
    #    \return liveness (see liveness_2d_estimator.Liveness for details).
    #
    # \~Russian
    #    \brief Определить принадлежность лица реальному человеку.
    #
    #    \param[in] sample
    #      Образец лица.
    #
    #    \return Результат (см. liveness_2d_estimator.Liveness).
    def estimate_liveness(self, sample: RawSample) -> Liveness:
        exception = make_exception()

        result = self._dll_handle.Liveness2DEstimator_estimateLiveness(
            self._impl,
            sample._impl,
            exception)

        check_exception(exception, self._dll_handle)

        if result == 1:
            return Liveness.REAL

        if result == 2:
            return Liveness.FAKE

        return Liveness.NOT_ENOUGH_DATA
