##
# 	\file raw_image.py
# 	\~English
# 	    \brief RawImage - Raw image interface.
# 	\~Russian
# 	    \brief RawImage - интерфейс изображения в сыром формате

from enum import Enum
from ctypes import c_int32, c_char_p
import numpy as np
import cv2
from . import get_repr


class CapiData:

    def __init__(self, data, width, height, format, with_crop, crop_info_offset_x,
                 crop_info_offset_y, crop_info_data_image_width, crop_info_data_image_height):
        self.data = data
        self.width = width
        self.height = height
        self.format = format
        self.with_crop = with_crop
        self.crop_info_offset_x = crop_info_offset_x
        self.crop_info_offset_y = crop_info_offset_y
        self.crop_info_data_image_width = crop_info_data_image_width
        self.crop_info_data_image_height = crop_info_data_image_height

## @defgroup PythonAPI
#  @{
## @defgroup RawImage
#  @{


##
# \~English
#    \brief %Rectangle in an image.
# \~Russian
#    \brief Прямоугольник на изображении.
class Rectangle:
    ##
    # \~English
    #    \brief X coordinate of the top-left corner.
    # \~Russian
    #    \brief Координата X левого верхнего угла прямоугольника.
    x: int

    ##
    # \~English
    #    \brief Y coordinate of the top-left corner.
    # \~Russian
    #    \brief Координата Y левого верхнего угла прямоугольника.
    y: int

    ##
    # \~English
    #    \brief Width of the rectangle.
    # \~Russian
    #    \brief Ширина прямоугольника.
    width: int

    ##
    # \~English
    #    \brief Height of the rectangle.
    # \~Russian
    #    \brief Высота прямоугольника.
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
#    \brief Format of image data.
# \~Russian
#    \brief Формат данных изображения.
class Format(Enum):

    ##
    # \~English
    #    \brief Grayscale, 8 bit per pixel.
    # \~Russian
    #    \brief Оттенки серого, 8 бит на пиксел.
    FORMAT_GRAY = 0

    ##
    # \~English
    #    \brief RGB, 24 bit per pixel, 8 bit per channel.
    # \~Russian
    #    \brief RGB, 24 бита на пиксел, 8 бит на канал.
    FORMAT_RGB = 1

    ##
    # \~English
    #    \brief BGR, 24 bit per pixel, 8 bit per channel.
    # \~Russian
    #    \brief BGR, 24 бита на пиксел, 8 бит на канал.
    FORMAT_BGR = 2

    ##
    # \~English
    #    \brief NV21 format in the YUV color coding system, the standard image format used on the Android camera preview.
    # \~Russian
    #    \brief Формат NV21 в цветовой системе кодирования YUV, стандартный формат изображений, используемых в превью Android-камер.
    FORMAT_YUV_NV21 = 3

    ##
    # \~English
    #    \brief NV12 format in the YUV color coding system.
    # \~Russian
    #    \brief Формат NV12 в цветовой системе кодирования YUV.
    FORMAT_YUV_NV12 = 4


##
# \~English
#    \brief Raw image interface.
#      To use this interface, you need to inherit it and create your own implementation for a specific image type.
#      See an example of implementation in face_sdk_3divi/example/cv_raw_image.py.
#
#      <strong>
#      WARNING:
#      </strong>
#      Member functions RawImage.data, RawImage.width, RawImage.height and RawImage.format
#      can't throw any exceptions, since they will be called from the dynamic library
#      and throwing an exception can cause a crash.
#      <br>
#      So instead of throwing an exception you can return some invalid values:
#      - NULL pointer - from the RawImage.data,
#      - non-positive number - from the RawImage.width and RawImage.height,
#      - negative number - from the RawImage.format.
#      <br>
#      The facerec library will handle incorrect data and will throw a Error exception object
#      that you will be able to catch.
#
# \~Russian
#    \brief Интерфейс изображения в сыром формате.
#      Для использования необходимо создать собственную реализацию этого интерфейса.
#      Пример реализации см. в face_sdk_3divi/example/cv_raw_image.py.
#
#      <strong>
#      ПРЕДУПРЕЖДЕНИЕ:
#      </strong>
#      Методы  RawImage.data, RawImage.width, RawImage.height и RawImage.format
#      не могут выбрасывать исключения, так как они будут вызываться из динамически загруженной библиотеки,
#      и выброшенное в них исключение будет невозможно перехватить, что станет причиной сбоя.
#      <br>
#      Поэтому вместо выбрасывания исключения необходимо возвращать некорректные значения:
#      - Нулевой указатель - из метода RawImage.data,
#      - Неположительное число - из метода RawImage.width и RawImage.height,
#      - Отрицательное число - из метода RawImage.format.
#      <br>
#      Обнаружив некорректное значения, библиотека libfacerec выбросит исключение Error, которое вы сможете перехватить.

class RawImage:

    ##
    # \~English
    #    \brief Get a pointer to the image data buffer.
    #      All pixels must be stored continuously row by row, without gaps at the end of each row.
    #      Can't throw an exception (see Warning in Class description).
    #    \return
    #      Pointer to the image data buffer.
    #
    # \~Russian
    #    \brief Получить указатель на данные изображения.
    #      Все пикселы должны быть сохранены последовательно, строка за строкой, без разрывов.
    #      Не может выбрасывать исключения (см. предупреждение).
    #    \return
    #      Указатель на данные изображения.
    data: bytes

    ##
    # \~English
    #    \brief Get image width.
    #      Can't throw an exception (see Warning in Class description).
    #    \return
    #      Number of columns in the image.
    #
    # \~Russian
    #    \brief Получить ширину изображения.
    #      Не может выбрасывать исключения (см. предупреждение).
    #    \return
    #      Ширина изображения в пикселах.
    width: int

    ##
    # \~English
    #    \brief Get image height.
    #      Can't throw an exception (see Warning in Class description).
    #    \return
    #      Number of rows in the image.
    #
    # \~Russian
    #    \brief Получить высоту изображения.
    #      Не может выбрасывать исключения (см. предупреждение).
    #    \return
    #      Высота изображения в пикселах.
    height: int

    ##
    # \~English
    #    \brief Get image format.
    #      Can't throw an exception (see Warning in Class description).
    #    \return
    #      Number of the image format (see face_sdk_3divi.modules.raw_image.Format).
    #
    # \~Russian
    #    \brief Получить формат изображения.
    #      Не может выбрасывать исключения (см. предупреждение).
    #    \return
    #      Формат изображения (см. face_sdk_3divi.modules.raw_image.Format).
    format: Format

    ##
    def __init__(self, width: int, height: int, format: Format, data: bytes):
        self.width = width
        self.height = height
        self.format = format
        self.data = data
        self.with_crop = False
        self.crop_info_offset_x = -1
        self.crop_info_offset_y = -1
        self.crop_info_data_image_width = -1
        self.crop_info_data_image_height = -1

    def make_c_api_data(self):
        assert self.data

        cdata = self.data

        return CapiData(
            c_char_p(cdata),
            c_int32(self.width),
            c_int32(self.height),
            c_int32(self.format.value),
            c_int32(self.with_crop),
            c_int32(self.crop_info_offset_x),
            c_int32(self.crop_info_offset_y),
            c_int32(self.crop_info_data_image_width),
            c_int32(self.crop_info_data_image_height)
        )
    ##
    # \~English
    #    \brief Create a RawImage object that represents the specified rectangle area of this image.
    #
    # \~Russian
    #    \brief Создать объект RawImage, представляющий указанную прямоугольную область этого изображения.
    def crop(self, rectangle: Rectangle):

        offset_x = self.crop_info_offset_x if self.with_crop else 0
        offset_y = self.crop_info_offset_y if self.with_crop else 0
        data_width = self.crop_info_data_image_width if self.with_crop else self.width
        data_height = self.crop_info_data_image_height if self.with_crop else self.height

        result = RawImage(rectangle.width, rectangle.height, self.format, self.data)

        result.with_crop = True
        result.crop_info_offset_x = rectangle.x + offset_x
        result.crop_info_offset_y = rectangle.y + offset_y
        result.crop_info_data_image_width = data_width
        result.crop_info_data_image_height = data_height

        return result

    ##
    # \~English
    #    \brief Virtual destructor.
    # \~Russian
    #    \brief Виртуальный деструктор.
    def __del__(self):
        pass
