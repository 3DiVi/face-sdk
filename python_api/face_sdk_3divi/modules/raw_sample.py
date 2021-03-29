##
#  \file RawSample.py
#  \~English
#     \brief RawSample - Interface object that stores a captured face sample.
#  \~Russian
#     \brief RawSample - Интерфейсный объект, хранящий образец лица.

from ctypes import c_void_p, py_object, c_float, byref, c_int, c_double, c_int32
from io import BytesIO
from typing import List
from enum import IntEnum
import numpy as np

from .wrap_funcs import assign_floats_vector_func, write_func
from .exception_check import check_exception, make_exception
from .complex_object import ComplexObject
from .dll_handle import DllHandle
from .point import Point
from .raw_image import Format, RawImage
from . import get_repr

## @defgroup PythonAPI
#  @{
## @defgroup RawSample
#  @{

##
# \~English
#    \brief Face orientation angles.
# \~Russian
#    \brief Углы ориентации лица.
class Angles:

    ##
    #  \~English
    #     \brief Yaw angle in degrees.
    #  \~Russian
    #     \brief Угол поворота вокруг вертикальной оси в градусах.
    yaw: float

    ##
    #  \~English
    #     \brief Pitch angle in degrees.
    #  \~Russian
    #     \brief Угол поворота вокруг горизонтальной оси в градусах.
    pitch: float

    ##
    #  \~English
    #     \brief Roll angle in degrees.
    #  \~Russian
    #     \brief Угол поворота в картинной плоскости в градусах.
    roll: float

    ##
    def __init__(self, yaw: float, pitch: float, roll: float):
        self.yaw = yaw
        self.pitch = pitch
        self.roll = roll

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief %Rectangle in an image.
# \~Russian
#    \brief Прямоугольник на изображении.
class Rectangle:

    ##
    #  \~English
    #     \brief X coordinate of the top-left corner.
    #  \~Russian
    #     \brief Координата X левого верхнего угла прямоугольника.
    x: int

    ##
    #  \~English
    #     \brief Y coordinate of the top-left corner.
    #  \~Russian
    #     \brief Координата Y левого верхнего угла прямоугольника.
    y: int

    ##
    #  \~English
    #     \brief Width of the rectangle.
    #  \~Russian
    #     \brief Ширина прямоугольника.
    width: int

    ##
    #  \~English
    #     \brief Height of the rectangle.
    #  \~Russian
    #     \brief Высота прямоугольника.
    height: int

    def __init__(self, x: int, y: int, width: int, height: int):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def __repr__(self):
        return get_repr(self)


##
# \~English
#    \brief Image formats for saving.
# \~Russian
#    \brief Форматы изображения для сохранения (сериализации).
class ImageFormat(IntEnum):

    #  \~English
    #     \brief JPEG (lossy compression).
    #  \~Russian
    #     \brief JPEG (сжатие с потерями).
    IMAGE_FORMAT_JPG = 0

    #  \~English
    #     \brief PNG (lossless compression).
    #  \~Russian
    #     \brief PNG (сжатие без потерь).
    IMAGE_FORMAT_PNG = 1

    #  \~English
    #     \brief TIFF (lossless compression).
    #  \~Russian
    #     \brief TIFF (сжатие без потерь).
    IMAGE_FORMAT_TIF = 2

    #  \~English
    #     \brief BMP (no compression).
    #  \~Russian
    #     \brief BMP (без сжатия).
    IMAGE_FORMAT_BMP = 3


##
# \~English
#    \brief Face cropping types.
# \~Russian
#    \brief Типы обрезки лица.
class FaceCutType(IntEnum):

    #  \~English
    #     \brief Unspecified cropping (any sample type).
    #  \~Russian
    #     \brief Базовый (подходит для образцов всех типов).
    FACE_CUT_BASE = 0,

    #  \~English
    #     \brief ISO/IEC 19794-5 Full Frontal (for ID, travel documents) (only frontal sample type).
    #  \~Russian
    #     \brief Полный фронтальный ТИЛ (тип изображения лица) по ГОСТ Р ИСО / МЭК 19794-5 (ISO/IEC 19794-5 Full Frontal) (только для фронтальных образцов).#
    FACE_CUT_FULL_FRONTAL = 1,

    #  \~English
    #     \brief ISO/IEC 19794-5 Token Frontal (fixed eye positions) (only frontal sample type).
    #  \~Russian
    #     \brief Условно фронтальный ТИЛ (тип изображения лица) по ГОСТ Р ИСО / МЭК 19794-5 (ISO/IEC 19794-5 Token Frontal) (только для фронтальных образцов).#
    FACE_CUT_TOKEN_FRONTAL = 2


DIMENSIONS_NUMBER = 3


##
#  \~English
#     \brief Interface object that stores a captured face sample.
#  \~Russian
#     \brief Интерфейсный объект, хранящий образец лица.
class RawSample(ComplexObject):

    def __init__(self, dll_handle: DllHandle, impl: c_void_p):
        super(RawSample, self).__init__(dll_handle, impl)

    ##
    # \~English
    #    \brief Get a point of the left eye. Only frontal samples. Thread-safe.
    #
    #    \return Point of the left eye.
    #
    # \~Russian
    #    \brief Получить позицию левого глаза. Только для фронтальных образцов. Потокобезопасный.
    #
    #    \return Позиция левого глаза на оригинальном изображении.
    def get_left_eye(self) -> Point:
        exception = make_exception()

        c_x = c_float()
        c_y = c_float()
        self._dll_handle.RawSample_getLeftEye(
            self._impl,
            byref(c_x),
            byref(c_y),
            exception)

        check_exception(exception, self._dll_handle)

        return Point(c_x.value, c_y.value)

    ##
    # \~English
    #    \brief Get a point of the right eye. Only frontal samples. Thread-safe.
    #
    #    \return Point of the right eye.
    #
    # \~Russian
    #    \brief Получить позицию правого глаза. Только для фронтальных образцов. Потокобезопасный.
    #
    #    \return Позиция правого глаза на оригинальном изображении.
    def get_right_eye(self) -> Point:
        exception = make_exception()

        x = c_float()
        y = c_float()
        self._dll_handle.RawSample_getRightEye(
            self._impl,
            byref(x),
            byref(y),
            exception)

        check_exception(exception, self._dll_handle)

        return Point(x.value, y.value)

    ##
    # \~English
    #    \brief Get a face bounding rectangle. Thread-safe.
    #
    #    \return Rectangle bounding face on the original image.
    #
    # \~Russian
    #   \brief Получить ограничивающий прямоугольник лица. Потокобезопасный.
    #
    #   \return Прямоугольник, ограничивающий лицо на оригинальном изображении.
    def get_rectangle(self) -> Rectangle:
        exception = make_exception()

        x = c_int()
        y = c_int()
        width = c_int()
        height = c_int()

        self._dll_handle.RawSample_getRectangle(
            self._impl,
            byref(x),
            byref(y),
            byref(width),
            byref(height),
            exception)

        check_exception(exception, self._dll_handle)

        return Rectangle(x.value, y.value, width.value, height.value)

    ##
    # \~English
    #    \brief Get anthropometric landmarks. Only frontal samples. Thread-safe.
    #
    #    \return The vector of anthropometric points in the original image.
    #
    # \~Russian
    #    \brief Получить антропометрические точки лица. Только для фронтальных образцов. Потокобезопасный.
    #
    #    \return Вектор позиций антропометрических точек на оригинальном изображении.
    def get_landmarks(self) -> List[Point]:
        coordinates = list()

        exception = make_exception()

        self._dll_handle.RawSample_getLandmarks(
            self._impl,
            py_object(coordinates),
            assign_floats_vector_func,
            exception)

        check_exception(exception, self._dll_handle)

        points = list()

        for i in range(int(len(coordinates) / DIMENSIONS_NUMBER)):
            points.append(Point(coordinates[i * DIMENSIONS_NUMBER + 0],
                                coordinates[i * DIMENSIONS_NUMBER + 1],
                                coordinates[i * DIMENSIONS_NUMBER + 2]))

        return points

    ##
    # \~English
    #    \brief Get the characteristic points of the eyes. Only frontal samples. Thread-safe.
    #
    #    \return The vector of the positions of the points of the pupils and the boundaries of the eyelids in the original image.
    #
    # \~Russian
    #    \brief Получить характерные точки глаз. Только для фронтальных образцов. Потокобезопасный.
    #
    #    \return Вектор позиций точек зрачков и контуров век на оригинальном изображении.
    def get_iris_landmarks(self) -> List[Point]:
        coordinates = list()

        exception = make_exception()

        self._dll_handle.RawSample_getIrisLandmarks(
            self._impl,
            py_object(coordinates),
            assign_floats_vector_func,
            exception)

        check_exception(exception, self._dll_handle)

        points = list()

        for i in range(int(len(coordinates) / DIMENSIONS_NUMBER)):
            points.append(Point(coordinates[i * DIMENSIONS_NUMBER + 0],
                                coordinates[i * DIMENSIONS_NUMBER + 1],
                                coordinates[i * DIMENSIONS_NUMBER + 2]))

        return points

    ##
    #  \~English
    #     \brief Get a face orientation. Only frontal samples. Thread-safe.
    #     \return Face orientation angles in degrees.
    #
    #  \~Russian
    #     \brief Получить углы ориентации лица. Только для фронтальных образцов. Потокобезопасный.
    #     \return Углы ориентации лица.
    def get_angles(self) -> Angles:
        exception = make_exception()

        yaw = c_float()
        pitch = c_float()
        roll = c_float()

        self._dll_handle.RawSample_getAngles(
            self._impl,
            byref(yaw),
            byref(pitch),
            byref(roll),
            exception)

        check_exception(exception, self._dll_handle)

        return Angles(yaw.value, pitch.value, roll.value)

    ##
    #  \~English
    #    \brief Does the sample store the original image
    #      (True for samples made with VideoWorker with enabled store_original_frame_in_raw_sample).
    #      Thread-safe.
    #
    #    \return True, if it has original image, otherwise false.
    #
    #  \~Russian
    #    \brief Хранит ли образец оригинальное изображение, с которого он был получен
    #      (True для образцов, полученных с помощью VideoWorker с включенным
    #      параметром store_original_frame_in_raw_sample).
    #      Потокобезопасный.
    #
    #    \return True, если содержит, иначе False.
    def has_original_image(self) -> int:
        exception = make_exception()

        result = self._dll_handle.RawSample_hasOriginalImage(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    #  \~English
    #  \brief Get the score of the detected face (for samples made with supported single-shot Capturers). Thread-safe.
    #
    #    \return One if this RawSample was made with an unsupported detector,
    #            otherwise - a number in the range [0 ... 1].
    #
    #    \~Russian
    #    \brief Получить уверенность детектирования лица (для образцов, полученных с помощью поддерживаемого Capturer).
    #           Потокобезопасный.
    #
    #    \return Один, если образец был получен через неподдерживаемый Capturer, иначе - число в диапазоне [0 ... 1].
    def get_score(self) -> float:
        exception = make_exception()

        result = self._dll_handle.RawSample_getScore(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Get a sample type. Thread-safe.
    #
    #    \return Type of the sample.
    #
    # \~Russian
    #    \brief Получить тип образца. Потокобезопасный.
    #
    #    \return Тип образца.
    def get_type(self) -> int:
        exception = make_exception()
        result = self._dll_handle.RawSample_getType(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Get an ID of the frame, in which this RawSample is detected
    #      (for samples made with VideoWorker). Thread-safe.
    #
    #    \return Zero if this RawSample was made not with VideoWorker,
    #      \n
    #      otherwise it's a nonnegative integer - id (that was returned
    #      by the VideoWorker.add_video_frame) of the frame,
    #      in which this RawSample is detected.
    #
    # \~Russian
    #    \brief Получить ID кадра, на котором был обнаружен образец
    #      (для образцов, полученных с помощью VideoWorker). Потокобезопасный.
    #
    #    \return Ноль, если образец был получен не через VideoWorker,
    #      \n
    #      иначе - неотрицательное целое число -
    #      идентификатор (который был возвращен методом VideoWorker.add_video_frame)
    #      кадра, на котором был обнаружен образец.
    def get_frame_id(self) -> int:
        exception = make_exception()
        result = self._dll_handle.RawSample_getFrameID(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    #  \~English
    #     \brief Get a face ID. Thread-safe.
    #     \return - A non-negative track id, if this sample was captured by a tracker, or
    #       - Any meaningless negative value, otherwise.
    #
    #  \~Russian
    #     \brief Получить целочисленный идентификатор. Потокобезопасный.
    #     \return - Неотрицательное число, если образец был получен с помощью трекера, или
    #       - Любое отрицательное число в противном случае.
    def get_id(self) -> int:
        exception = make_exception()
        result = self._dll_handle.RawSample_getID(
            self._impl,
            exception)

        check_exception(exception, self._dll_handle)

        return result

    ##
    # \~English
    #    \brief Crop the face and save in the specified format. Thread-safe.
    #
    #    \param[in]  binary_stream
    #      Output stream object for image saving.
    #      The file stream (BytesIO) (if any)
    #
    #    \param[in]  format
    #      Image encoding format.
    #
    #    \param[in]  cut_type
    #      Face cropping types.
    #
    # \~Russian
    #    \brief Обрезать лицо и сохранить в указанном формате. Потокобезопасный.
    #
    #    \param[in]  binary_stream
    #      Поток для сохранения закодированного изображения.
    #      Поток файла (BytesIO).
    #
    #    \param[in]  format
    #      Формат для сохранения изображения.
    #
    #    \param[in]  cut_type
    #      Тип обрезки.
    def cut_face_image(self, binary_stream: BytesIO,
                       image_format: ImageFormat = ImageFormat.IMAGE_FORMAT_JPG,
                       cut_type: FaceCutType = FaceCutType.FACE_CUT_BASE):
        width = c_int()
        height = c_int()

        exception = make_exception()
        self._dll_handle.RawSample_cutFaceImage(
            self._impl,
            py_object(binary_stream),
            write_func,
            byref(width),
            byref(height),
            c_int32(image_format.value),
            c_int32(-1),
            c_int32(cut_type.value),
            exception)
        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Crop face in RawImage format (with raw pixels). Thread-safe.
    #
    #    \param[in]  color_model
    #      Image color model.
    #
    #    \param[in]  cut_type
    #      Face cropping types.
    #
    #    \return  RawImage with cropped face
    #
    # \~Russian
    #    \brief Обрезать лицо и выдать в формате RawImage (с декодированными пикселями). Потокобезопасный.
    #
    #    \param[in]  color_model
    #      Цветовая модель изображения.
    #
    #    \param[in]  cut_type
    #      Тип обрезки.
    #
    #    \return  RawImage c кропом лица
    def cut_face_raw_image(self, color_model: Format,
                           cut_type: FaceCutType = FaceCutType.FACE_CUT_BASE):
        binary_stream = BytesIO()

        width = c_int()
        height = c_int()

        exception = make_exception()
        self._dll_handle.RawSample_cutFaceImage(
            self._impl,
            py_object(binary_stream),
            write_func,
            byref(width),
            byref(height),
            c_int32(-1),
            c_int32(color_model.value),
            c_int32(cut_type.value),
            exception)
        check_exception(exception, self._dll_handle)

        binary_stream.seek(0)
        result = RawImage(
                    width.value,
                    height.value,
                    color_model.value,
                    binary_stream.read())
        binary_stream.close()

        return result

    ##
    #  \~English
    #     \brief Save (serialize) the sample. Original image will not be saved (i.e. only the part of an image containing the face is saved).
    #        The format is platform-independent.
    #        Thread-safe.
    #        After saving, you can load it with FacerecService.load_raw_sample.
    #
    #     \param[in]  binary_stream
    #        Output stream object for saving the raw sample. The file stream (BytesIO) (if any).
    #
    #     \param[in]  format
    #        Image encoding format.
    #
    #  \~Russian
    #     \brief Сохранить (сериализовать) образец. Оригинальное изображение не сохраняется. (т.е. сохраняется только часть изображения, содержащая лицо).
    #        Формат платформонезависимый.
    #        Потокобезопасный.
    #        Десериализовать образец можно через FacerecService.load_raw_sample.
    #
    #     \param[out]  binary_stream
    #        Объект потока.
    #        Поток файла (BytesIO).
    #
    #     \param[in]  format
    #        Формат кодирования изображения.
    def save(self, binary_stream: BytesIO, image_format: ImageFormat = ImageFormat.IMAGE_FORMAT_JPG):
        exception = make_exception()

        self._dll_handle.RawSample_save(
            self._impl,
            py_object(binary_stream),
            write_func,
            c_int32(image_format.value),
            exception
        )

        check_exception(exception, self._dll_handle)

    ##
    # \~English
    #    \brief Get a cropping rectangle (for preview) that will be used in cutFaceImage. Thread-safe.
    #
    #    \return 4 points that are corners of a rectangle in the counterclockwise order starting from the upper left corner.
    #
    #    \param[in]  cut_type
    #      Face cropping types.
    #
    # \~Russian
    #    \brief Получить прямоугольник (для предпросмотра обрезки). Потокобезопасный.
    #
    #    \return 4 точки - углы прямоугольника в порядке против часовой стрелки, начиная с левого верхнего угла.
    #
    #    \param[in]  cut_type
    #      Тип обрезки.
    def get_face_cut_rectangle(self, cut_type: FaceCutType = FaceCutType.FACE_CUT_BASE) -> List[Point]:
        exception = make_exception()
        corners = (c_float * 8)()

        self._dll_handle.RawSample_getFaceCutRectangle(
            self._impl,
            c_int32(cut_type.value),
            corners,
            exception
        )

        check_exception(exception, self._dll_handle)

        points = list()
        corners = corners[:]

        for i in range(4):
            points.append(Point(corners[i * 2 + 0],
                                corners[i * 2 + 1]))
        return points

    ##
    # \~English
    #    \brief Save (serialize) all sample information except the image. You have to store the image
    #      on your own in order to pass it to the FacerecService.load_raw_sample_without_image for deserialization.
    #      If you transformed the image in soma way, you can set the parameters of coordinate space transformation:
    #      (x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
    #      that will be applied to the information about face position.
    #      Note that you must set the same transformation only once - either during serialization or during deserialization,
    #      otherwise transformation will be applied twice, which is wrong.
    #      The format is platform-independent.
    #      Thread-safe.
    #      After saving, you can load it with FacerecService.load_raw_sample_without_image.
    #
    #    \param[in]  binary_stream
    #      Output stream object for saving the raw sample.
    #      The file stream (BytesIO) (if any).
    #
    #    \param[in]  space_translation_x
    #      Coordinate space transform parameter.
    #
    #    \param[in]  space_translation_y
    #      Coordinate space transform parameter.
    #
    #    \param[in]  space_scale
    #      Coordinate space transform parameter.
    #
    # \~Russian
    #    \brief Сохранить (сериализовать) всю информацию, кроме изображения. Вы должны сохранить изображение
    #      самостоятельно для того, чтобы подать его в FacerecService.load_raw_sample_without_image для десериализации.
    #      В случае, если над изображением была выполнена трансформация,
    #      вы можете указать параметры преобразования пространства координат:
    #      (x, y) -> ( (x + space_translation_x) * space_scale, (y + space_translation_y) * space_scale )
    #      которое будет применено к информации о позиции лица.
    #      Заметьте, что параметры одного и того же преобразования нужно передавать только один раз - либо
    #      при сериализации, либо при десериализации, иначе преобразование будет применено дважды, что неверно.
    #      Формат платформонезависимый.
    #      Потокобезопасный.
    #      Десериализовать образец можно через FacerecService.load_raw_sample_without_image.
    #
    #    \param[in]  binary_stream
    #      Объект потока для сохранения "сырого" изображения.
    #      Поток файла (BytesIO).
    #
    #    \param[in]  space_translation_x
    #      Параметр преобразования пространства координат.
    #
    #    \param[in]  space_translation_y
    #      Параметр преобразования пространства координат.
    #
    #    \param[in]  space_scale
    #      параметр преобразования пространства координат.
    def save_without_image(self, binary_stream: BytesIO,
                           space_translation_x: float = 0,
                           space_translation_y: float = 0,
                           space_scale: float = 1):
        exception = make_exception()

        self._dll_handle.RawSample_save_without_image(
            self._impl,
            py_object(binary_stream),
            write_func,
            c_double(space_translation_x),
            c_double(space_translation_y),
            c_double(space_scale),
            exception
        )
        check_exception(exception, self._dll_handle)
