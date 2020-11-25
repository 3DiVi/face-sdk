##
#  \file depth_map_raw.py

import cv2
from ctypes import c_ushort
from . import get_repr


##
# \~English
#    \brief A depth map aligned with the original color image.
#      Correspondence between the coordinates of the original image and depth map is as follows:
#      let (x; y) be a point on the depth map, then a  corresponding point on the original image is
#      (depth_map_2_image_offset_x + depth_map_2_image_scale_x * x; depth_map_2_image_offset_y + depth_map_2_image_scale_y * y).
#
# \~Russian
#    \brief Карта глубины, отрегистрированная в соответствии с исходным цветным изображением.
#      Соответствие между координатами исходного изображения и картой глубины:
#      пусть (x; y) - точка на карте глубины, тогда соответствующая точка на исходном изображении - это:
#     (depth_map_2_image_offset_x + depth_map_2_image_scale_x * x; depth_map_2_image_offset_y + depth_map_2_image_scale_y * y).
class DepthMapRaw:

    ##
    #  \~English
    #     \brief Height of the depth map.
    #  \~Russian
    #     \brief Высота карты глубины.
    depth_map_rows: int

    ##
    #  \~English
    #     \brief Width of the depth map.
    #  \~Russian
    #     \brief Ширина карты глубины.
    depth_map_cols: int

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of the original image and depth map.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и карты глубины.
    depth_map_2_image_offset_x: float

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of the original image and depth map.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и карты глубины.
    depth_map_2_image_offset_y: float

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of the original image and depth map.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и карты глубины.
    depth_map_2_image_scale_x: float

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of the original image and depth map.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и карты глубины.
    depth_map_2_image_scale_y: float

    ##
    #  \~English
    #     \brief The horizontal viewing angle of the depth map in degrees.
    #  \~Russian
    #     \brief Горизонтальный угол обзора карты глубины в градусах.
    horizontal_fov: float

    ##
    #  \~English
    #     \brief The vertical viewing angle of the depth map in degrees.
    #  \~Russian
    #     \brief Вертикальный угол обзора карты глубины в градусах.
    vertical_fov: float

    ##
    #  \~English
    #     \brief Pointer to the depth raw data.
    #  \~Russian
    #     \brief Указатель на данные глубины.
    depth_data: bytes

    ##
    #  \~English
    #     \brief Scale of values stored in the depth_data.
    #  \~Russian
    #    \brief Масштаб значений, содержащихся в depth_data.
    depth_unit_in_millimeters: float

    ##
    #  \~English
    #     \brief Stride in bytes.
    #  \~Russian
    #     \brief Шаг в байтах.
    depth_data_stride_in_bytes: int

    def __init__(self):
        self.depth_map_rows = 0
        self.depth_map_cols = 0
        self.depth_map_2_image_offset_x = 0
        self.depth_map_2_image_offset_y = 0
        self.depth_map_2_image_scale_x = 0
        self.depth_map_2_image_scale_y = 0
        self.horizontal_fov = 0
        self.vertical_fov = 0
        self.depth_data = bytes()
        self.depth_unit_in_millimeters = 0
        self.depth_data_stride_in_bytes = 0

    ##
    # \~English
    #    \brief A function that loads a depth map from a file and converts it to a uint16 array.
    # \~Russian
    #    \brief Функция, выполняющая загрузку карты глубины из файла и преобразующая его в массив uint16.
    @staticmethod
    def read_depth_map(filepath: str):
        depth_img = cv2.imread(filepath, -1)
        assert len(depth_img.shape) == 2

        pyarr = depth_img.flatten().tolist()
        seq = c_ushort * len(pyarr)
        return seq(*pyarr)

    def __repr__(self):
        return get_repr(self)
