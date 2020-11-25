##
#  \file quality_estimator.py
#  \~English
#     \brief QualityEstimator - Interface object used to estimate sample quality.
#  \~Russian
#     \brief QualityEstimator - Интерфейсный объект для определения качества образца лица.

from ctypes import c_int, c_void_p, byref

from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from . import get_repr


##
# \~English
#    \brief Set of quality parameters.
#      Each parameter is in range 1 to 5.
#      '1' stands for 'best quality', '5' stands for 'worst quality'.
#
# \~Russian
#    \brief Набор характеристик качества.
#      Каждая характеристика находится в диапазоне от 1 до 5.
#      '1' означает 'лучшее качество', '5' - 'худшее качество'.
class Quality:

    ##
    #  \~English
    #     \brief Total quality score.
    #  \~Russian
    #     \brief Показатель общего качества.

    total: int

    ##
    #  \~English
    #      \brief Lighting uniformity.
    #  \~Russian
    #      \brief Равномерность освещения.

    lighting: int

    ##
    #  \~English
    #     \brief Noise.
    #  \~Russian
    #     \brief Зашумленность.

    noise: int

    ##
    #  \~English
    #      \brief Sharpness.
    #  \~Russian
    #      \brief Резкость.

    sharpness: int

    ##
    #  \~English
    #      \brief Flare.
    #  \~Russian
    #      \brief Засветка.

    flare: int

    def __init__(self, lighting, noise, sharpness, flare):
        self.lighting = lighting
        self.noise = noise
        self.sharpness = sharpness
        self.flare = flare
        self.total = max([lighting, noise, sharpness, flare])

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Alias for the type of a smart pointer to QualityEstimator.
# \~Russian
#    \brief Псевдоним для типа умного указателя на QualityEstimator.
class QualityEstimator(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super().__init__(dll_handle, impl)

    ##
    #  \~English
    #     \brief Estimates quality of a given face sample.
    #
    #     \param[in] sample
    #       Face sample.
    #
    #     \return Estimated quality.
    #
    #  \~Russian
    #     \brief Определить качество поданного образца лица.
    #
    #     \param[in] sample
    #       Образец лица.
    #
    #     \return Определенное качество.
    def estimate_quality(self, sample: RawSample) -> Quality:
        exception = make_exception()

        lighting = c_int()
        noise = c_int()
        sharpness = c_int()
        flare = c_int()

        self._dll_handle.QualityEstimator_estimateQuality(
            self._impl,
            sample._impl,
            byref(lighting),
            byref(noise),
            byref(sharpness),
            byref(flare),
            exception)

        quality = Quality(lighting.value, noise.value, sharpness.value, flare.value)

        check_exception(exception, self._dll_handle)

        return quality
