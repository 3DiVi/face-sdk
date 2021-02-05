##
#  \file liveness_2d_estimator.py
#  \~English
#      \brief Liveness2DEstimator is an interface object used to estimate face liveness in order to prevent spoofing attacks.
#  \~Russian
#      \brief Liveness2DEstimator - интерфейсный объект для определения принадлежности лица реальному человеку.

from ctypes import c_void_p, byref, c_int, c_float

from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from enum import Enum

from . import get_repr


##
#  \~English
#     \brief Estimated liveness.
#  \~Russian
#     \brief Результат определения принадлежности лица живому человеку.
class Liveness(Enum):

    ##
    #  \~English
    #     \brief Not enough information.
    #  \~Russian
    #     \brief Недостаточно информации.
    NOT_ENOUGH_DATA = 0

    ##
    #  \~English
    #     \brief The observed face belongs to a real person.
    #  \~Russian
    #     \brief Лицо принадлежит реальному человеку.
    REAL = 1

    ##
    #  \~English
    #     \brief The observed face is taken from a photo.
    #  \~Russian
    #     \brief Лицо было взято с фото.
    FAKE = 2

    ##
    #  \~English
    #     \brief The observed face was not checked.
    #  \~Russian
    #     \brief Лицо не было проверено.
    NOT_COMPUTED = 3


##
#  \~English
#     \brief Estimated liveness and score.
#  \~Russian
#     \brief Результат определения и вероятность принадлежности лица живому человеку.
class LivenessAndScore:

    ##
    #  \~English
    #     \brief Estimated liveness (see Liveness for details).
    #  \~Russian
    #     \brief Результат определения принадлежности лица живому человеку (см. Liveness)
    liveness: Liveness

    ##
    #  \~English
    #     \brief Liveness score.
    #  \~Russian
    #     \brief Вероятность принадлежности лица живому человеку.
    score: float

    def __init__(self, liveness, score):
        self.liveness = liveness
        self.score = score

    def __repr__(self):
        return get_repr(self)


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
    #    \return liveness (see Liveness for details).
    #
    # \~Russian
    #    \brief Определить принадлежность лица реальному человеку.
    #
    #    \param[in] sample
    #      Образец лица.
    #
    #    \return Результат (см. Liveness).
    def estimate_liveness(self, sample: RawSample) -> Liveness:
        exception = make_exception()

        verdict = c_int()
        score = c_float()

        self._dll_handle.Liveness2DEstimator_estimateLiveness(
            self._impl,
            sample._impl,
            byref(verdict),
            byref(score),
            exception)

        check_exception(exception, self._dll_handle)

        if verdict.value == 1:
            return Liveness.REAL

        if verdict.value == 2:
            return Liveness.FAKE

        return Liveness.NOT_ENOUGH_DATA

    ##
    # \~English
    #    \brief liveness and score of an observed face.
    #
    #    \param[in] sample
    #      Face sample.
    #
    #    \return liveness and score (see LivenessAndScore for details).
    #
    # \~Russian
    #    \brief Определение и вероятность принадлежности лица реальному человеку.
    #
    #    \param[in] sample
    #      Образец лица.
    #
    #    \return Результат (см. LivenessAndScore).
    def estimate(self, sample: RawSample) -> LivenessAndScore:
        exception = make_exception()

        verdict = c_int()
        score = c_float()

        self._dll_handle.Liveness2DEstimator_estimateLiveness(
            self._impl,
            sample._impl,
            byref(verdict),
            byref(score),
            exception)

        check_exception(exception, self._dll_handle)

        result = LivenessAndScore(Liveness.NOT_ENOUGH_DATA, score.value)
        if verdict.value == 1:
            result.liveness = Liveness.REAL

        if verdict.value == 2:
            result.liveness = Liveness.FAKE

        return result
