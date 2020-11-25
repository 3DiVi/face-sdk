##
#  \file IRLivenessEstimator.py
#  \~English
#     \brief IRLivenessEstimator is an interface object used to estimate face liveness in order to prevent spoofing attacks.
#  \~Russian
#     \brief IRLivenessEstimator - интерфейсный объект для определения принадлежности лица реальному человеку.

from enum import Enum
from ctypes import c_void_p, c_int, c_float

from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .raw_sample import RawSample
from .ir_frame_raw import IRFrameRaw


##
# \~English
#    \brief Estimated liveness.
# \~Russian
#    \brief Результат определения принадлежности лица живому человеку.
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
# \~English
#    \brief Interface object used to estimate face liveness in order to prevent spoofing attacks.
# \~Russian
#    \brief Интерфейсный объект для определения принадлежности лица реальному человеку.
class IRLivenessEstimator(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(IRLivenessEstimator, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Estimates liveness of an observed face.
    #      The given IR frame must be synchronized and aligned
    #      with the original image from which the sample was captured.
    #
    #    \param[in] sample
    #      Face sample.
    #
    #    \param[in] ir_frame
    #      IR Frame.
    #
    #    \return Estimated liveness (see Liveness).
    #
    # \~Russian
    #    \brief Определить принадлежность лица реальному человеку.
    #      Инфракрасный кадр должен быть синхронизирован и отрегистрирован
    #      в соответствии с исходным изображением, на котором было обнаружено лицо.
    #
    #    \param[in] sample
    #      Образец лица.
    #
    #    \param[in] ir_frame
    #      Инфракрасный кадр.
    #
    #    \return Результат (см. Liveness).
    def estimate_liveness(self, sample: RawSample, ir_frame: IRFrameRaw) -> Liveness:
        exception = make_exception()

        result = self._dll_handle.IRLivenessEstimator_estimateLiveness(
            self._impl,
            sample._impl,
            c_int(ir_frame.ir_frame_rows),
            c_int(ir_frame.ir_frame_cols),
            c_float(ir_frame.ir_frame_2_image_offset_x),
            c_float(ir_frame.ir_frame_2_image_offset_y),
            c_float(ir_frame.ir_frame_2_image_scale_x),
            c_float(ir_frame.ir_frame_2_image_scale_y),
            c_float(ir_frame.ir_horizontal_fov),
            c_float(ir_frame.ir_vertical_fov),
            ir_frame.ir_frame_data,
            c_int(ir_frame.ir_data_stride_in_bytes),
            exception)

        check_exception(exception, self._dll_handle)

        if result == 1:
            return Liveness.REAL

        if result == 2:
            return Liveness.FAKE

        return Liveness.NOT_ENOUGH_DATA
