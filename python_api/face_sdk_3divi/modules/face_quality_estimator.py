##
#  \file face_quality_estimator.py
#  \~English
#      \brief FaceQualityEstimator - Interface object for sample quality estimation.
#  \~Russian
#      \brief FaceQualityEstimator - Интерфейсный объект для определения качества образца лица.

from ctypes import c_void_p

from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample

## @defgroup PythonAPI
#  @{
## @defgroup FaceQualityEstimator
#  @{

##
# \~English
#    \brief Interface object for sample quality estimation.
# \~Russian
#    \brief Интерфейсный объект для определения качества образца лица.
class FaceQualityEstimator(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super().__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Estimates quality of a given face sample.
    #
    #    \param[in] sample
    #      Face sample.
    #
    #    \return Estimated quality value (the greater the value is, the better the quality is).
    #      Aggregates sample usability (i.e. pose, occlusion, noise, blur and lighting)
    #      for face recognition in a single real value.
    #
    # \~Russian
    #    \brief Определить качество поданного образца лица.
    #
    #    \param[in] sample
    #      Образец лица.
    #
    #    \return Определенное качество - вещественное число (чем больше, тем выше качество),
    #      агрегирующее качество освещения, шум, размытие и ракурс.
    def estimate_quality(self, sample: RawSample) -> float:
        exception = make_exception()

        result = self._dll_handle.FaceQualityEstimator_estimateQuality(
            self._impl,
            sample._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result
