##
#  \file capturer.py
#  \~English
#      \brief Capturer - Interface object for detecting or tracking of faces in the images or video sequences.
#  \~Russian
#      \brief Capturer - Интерфейсный объект для детекции и трекинга лиц на изображениях или видеопоследовательностях.

#  \~English
#      \brief Interface object for detecting or tracking of faces in the images or video sequences.
#  \~Russian
#      \brief Интерфейсный объект для детекции и трекинга лиц на изображениях или видеопоследовательностях.
from enum import Enum
from ctypes import CDLL
from ctypes import c_char_p, c_void_p, py_object, c_float, c_uint32, c_double

from typing import List

from .wrap_funcs import assign_pointers_vector_func
from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .raw_sample import RawSample
from .raw_image import RawImage, CapiData
from .error import Error
from typing import Union


## @defgroup PythonAPI
#  @{

## @defgroup Capturer
#  @{

##
# \~English
#    \brief Capturer types (see also RawSample.SampleType).
# \~Russian
#    \brief Типы детекторов (см. также RawSample.SampleType).
class CapturerType(Enum):
    ##
    # \~English
    #    \brief Only frontal-oriented faces will be captured.
    # \~Russian
    #    \brief Детектирует только лица в анфас.
    CAPTURER_TYPE_FRONTAL = 0

    ##
    # \~English
    #    \brief Only left-profile-oriented faces will be captured.
    # \~Russian
    #    \brief Детектирует только лица в профиль, смотрящие влево.
    CAPTURER_TYPE_LEFT_PROFILE = 1

    ##
    # \~English
    #    \brief Only right-profile-oriented faces will be captured.
    # \~Russian
    #    \brief Детектирует только лица в профиль, смотрящие вправо.
    CAPTURER_TYPE_RIGHT_PROFILE = 2


##
#  \~English
#    \brief Interface object for detecting or tracking of faces in the images or video sequences.
#  \~Russian
#    \brief Интерфейсный объект для детекции и трекинга лиц на изображениях или видеопоследовательностях.
class Capturer(ComplexObject):

    def __init__(self, dll_handle: CDLL, impl: c_void_p):
        super(Capturer, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Capture faces in a given image or video frame.
    #
    #    \param[in] data Image or videoframe.
    #
    #    \return Vector of captured face samples.
    #
    #    \warning Gray-scale images are not supported by the objects created with some configuration files.
    #
    # \~Russian
    #    \brief Детектировать (и отследить) лица на изображении (кадре видео).
    #
    #    \param[in] data Изображение или кадр видео.
    #
    #    \return Вектор найденных лиц.
    #
    #    \warning Черно-белые изображения не поддерживаются объектами, созданными с некоторыми конфигурационными файлами.
    def capture(self, data: Union[RawImage, bytes]) -> List[RawSample]:
        if isinstance(data, RawImage):
            return self.__capture_raw_image_with_crop(data)
        elif isinstance(data, bytes):
            return self.__capture_encoded_image(data)
        else:
            raise Error(0xfe6034d4, "Wrong type of data")

    def __capture_encoded_image(self, data: bytes) -> List[RawSample]:
        void_result = list()

        exception = make_exception()

        self._dll_handle.Capturer_capture_encoded_image(
            self._impl,
            c_char_p(data),
            c_uint32(len(data)),
            py_object(void_result),
            assign_pointers_vector_func,
            exception)

        check_exception(exception, self._dll_handle)

        result = list()
        for el in void_result:
            result.append(RawSample(self._dll_handle, c_void_p(el)))

        return result

    def __capture_raw_image_with_crop(self, image: RawImage) -> List[RawSample]:
        void_result = list()

        cdata: CapiData = image.make_c_api_data()

        exception = make_exception()

        self._dll_handle.Capturer_capture_raw_image_with_crop(
            self._impl,
            cdata.data,
            cdata.width,
            cdata.height,
            cdata.format,
            cdata.with_crop,
            cdata.crop_info_offset_x,
            cdata.crop_info_offset_y,
            cdata.crop_info_data_image_width,
            cdata.crop_info_data_image_height,
            py_object(void_result),
            assign_pointers_vector_func,
            exception)

        check_exception(exception, self._dll_handle)
        result = list()
        for el in void_result:
            result.append(RawSample(self._dll_handle, c_void_p(el)))

        return result

    ##
    #  \~English
    #    \brief Create a sample manually with the marked eye positions.
    #
    #    \param[in] image
    #      Image.
    #
    #    \param[in] left_eye_x
    #      X coordinate of the left eye.
    #
    #    \param[in] left_eye_y
    #      Y coordinate of the left eye.
    #
    #    \param[in] right_eye_x
    #      X coordinate of the right eye.
    #
    #    \param[in] right_eye_y
    #      Y coordinate of the right eye.
    #
    #    \return Captured face sample.
    #
    # \~Russian
    #    \brief Создать сэмпл вручную с отмеченными позициями глаз.
    #
    #    \param[in] image
    #      Изображение.
    #
    #    \param[in] left_eye_x
    #      Координата x левого глаза.
    #
    #    \param[in] left_eye_y
    #      Координата y левого глаза.
    #
    #    \param[in] right_eye_x
    #      Координата x правого глаза.
    #
    #    \param[in] right_eye_y
    #      Координата y правого глаза.
    #
    #    \return Созданный сэмпл.
    def manual_capture(
            self,
            data: Union[RawImage, bytes],
            left_eye_x: float,
            left_eye_y: float,
            right_eye_x: float,
            right_eye_y: float) -> RawSample:
        if isinstance(data, RawImage):
            return self.__manual_capture_raw_image(data, left_eye_x, left_eye_y, right_eye_x, right_eye_y)
        elif isinstance(data, bytes):
            return self.__manual_capture_encoded_image(data, left_eye_x, left_eye_y, right_eye_x, right_eye_y)
        else:
            raise Error(0x15736ad6, "Wrong type of data")


    def __manual_capture_encoded_image(
            self,
            data:  bytes,
            left_eye_x: float,
            left_eye_y: float,
            right_eye_x: float,
            right_eye_y: float) -> RawSample:
        exception = make_exception()

        sample_impl = self._dll_handle.Capturer_manualCapture_encoded_image_eyes_points(
            self._impl,
            c_char_p(data),
            c_uint32(len(data)),
            c_float(left_eye_x),
            c_float(left_eye_y),
            c_float(right_eye_x),
            c_float(right_eye_y),
            exception)

        check_exception(exception, self._dll_handle)

        return RawSample(self._dll_handle, c_void_p(sample_impl))

    def __manual_capture_raw_image(
            self,
            image: RawImage,
            left_eye_x: float,
            left_eye_y: float,
            right_eye_x: float,
            right_eye_y: float) -> RawSample:

        cdata: CapiData = image.make_c_api_data()

        exception = make_exception()

        sample_impl = self._dll_handle.Capturer_manualCapture_raw_image_eyes_points_with_crop(
            self._impl,
            cdata.data,
            cdata.width,
            cdata.height,
            cdata.format,
            cdata.with_crop,
            cdata.crop_info_offset_x,
            cdata.crop_info_offset_y,
            cdata.crop_info_data_image_width,
            cdata.crop_info_data_image_height,
            c_float(left_eye_x),
            c_float(left_eye_y),
            c_float(right_eye_x),
            c_float(right_eye_y),
            exception)

        check_exception(exception, self._dll_handle)

        return RawSample(self._dll_handle, c_void_p(sample_impl))

    ##
    # \~English
    #    \brief Get a capturer type.
    #
    #    \return
    #       Type of the capturer.
    #
    # \~Russian
    #    \brief Получить тип детектора.
    #
    #    \return
    #       Тип детектора.
    def get_type(self) -> CapturerType:

        exception = make_exception()

        result = self._dll_handle.Capturer_getType(self._impl, exception)

        check_exception(exception, self._dll_handle)

        return CapturerType(result)

    ##
    # \~English
    #    \brief Forget all previous frames and faces (only affects trackers).
    #
    # \~Russian
    #    \brief Удалить из истории все кадры и лица (только для трекеров).
    def reset_history(self):

        exception = make_exception()

        self._dll_handle.Capturer_resetHistory(self._impl, exception)

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Change the parameter value in runtime.
    #
    #    \param[in] parameter
    #      Parameter name (tag name in the .xml config file).
    #
    #    \param[in] value
    #      New parameter value.
    #
    # \~Russian
    #    \brief Изменить значение параметра в runtime.
    #
    #    \param[in] parameter
    #      Имя параметра (имя тэга из .xml конфигурационного файла).
    #
    #    \param[in] value
    #      Новое значение параметра.
    def set_parameter(self, parameter: str, value: float):

        exception = make_exception()

        self._dll_handle.Capturer_setParameter(
            self._impl,
            c_char_p(bytes(parameter, "ascii")),
            c_double(value),
            exception
        )

        check_exception(exception, self._dll_handle)
