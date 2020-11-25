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

from ctypes import CDLL
from ctypes import c_char_p, c_void_p, py_object, c_float, c_uint32

from typing import List

from .wrap_funcs import assign_pointers_vector_func
from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .raw_sample import RawSample
from .raw_image import RawImage
from .error import Error
from typing import Union


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
    #    \warning Gray-scale images are not supported by the objects created with some configuration files
    #      (see <a href="https://github.com/3DiVi/face-sdk-docs/blob/master/doc/en/development/#face_capturing.md#capturer-class-reference">Capturer Class Reference</a>).
    #
    # \~Russian
    #    \brief Детектировать (и отследить) лица на изображении (кадре видео).
    #
    #    \param[in] data Изображение или кадр видео.
    #
    #    \return Вектор найденных лиц.
    #
    #    \warning Черно-белые изображения не поддерживаются объектами, созданными с некоторыми конфигурационными файлами
    #      (см. <a href="https://github.com/3DiVi/face-sdk-docs/blob/master/doc/ru/development/#face_capturing.md#класс-capturer">Класс Capturer</a>).
    def capture(self, data: Union[RawImage, bytes]) -> List[RawSample]:
        if isinstance(data, RawImage):
            data = data.data
        elif isinstance(data, bytes):
            data = data
        else:
            raise Error(0xfe6034d4, "Wrong type of data")
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
            data = data.data
        elif isinstance(data, bytes):
            data = data
        else:
            raise Error(0x15736ad6, "Wrong type of data")

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
