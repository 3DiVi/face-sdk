##
#  \file ir_frame_raw.py

import cv2
from ctypes import c_ushort
from . import get_repr

## @defgroup PythonAPI
#  @{
## @defgroup IRLivenessEstimator
#  @{

##
# \~English
#    \brief An IR frame aligned with the original color image.
#      Correspondence between the coordinates of the original image and the IR frame is as follows:
#      let (x; y) be a point on the IR frame, then a  corresponding point on the original image is
#      (ir_frame_2_image_offset_x + ir_frame_image_2_scale_x * x; ir_frame_2_image_offset_y + ir_frame_2_image_scale_y * y).
#
# \~Russian
#    \brief Инфракрасное изображение, отрегистрированное в соответствии с исходным цветным изображением.
#      Соответствие между координатами исходного изображения и инфракрасного изображения:
#      пусть (x; y) - точка на инфракрасном изображении, тогда соответствующая точка на исходном изображении - это:
#      (ir_frame_2_image_offset_x + ir_frame_2_image_scale_x * x; ir_frame_2_image_offset_y + ir_frame_2_image_scale_y * y).
class IRFrameRaw:

    ##
    #  \~English
    #     \brief Height of an IR frame.
    #  \~Russian
    #     \brief Высота IR кадра.
    ir_frame_rows: int

    ##
    #  \~English
    #     \brief Width of an IR frame.
    #  \~Russian
    #     \brief Ширина IR кадра.
    ir_frame_cols: int

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of an original image and an IR frame.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и карты глубины.
    ir_frame_2_image_offset_x: float

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of the original image and depth map.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и инфракрасного.
    ir_frame_2_image_offset_y: float

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of an original image and an IR frame.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и инфракрасного.
    ir_frame_2_image_scale_x: float

    ##
    #  \~English
    #     \brief Parameter of correspondence between the coordinates of an original image and an IR frame.
    #  \~Russian
    #     \brief Параметр соответствия координат исходного изображения и инфракрасного.
    ir_frame_2_image_scale_y: float

    ##
    #  \~English
    #     \brief The horizontal viewing angle of an IR frame in degrees.
    #  \~Russian
    #     \brief Горизонтальный угол обзора инфракрасного изображения в градусах.
    ir_horizontal_fov: float

    ##
    #  \~English
    #     \brief The vertical viewing angle of an IR frame in degrees.
    #  \~Russian
    #     \brief Вертикальный угол обзора инфракрасного изображения в градусах.
    ir_vertical_fov: float

    ##
    #  \~English
    #     \brief Pointer to the IR data.
    #  \~Russian
    #     \brief Указатель на инфракрасные данные.
    ir_frame_data: bytes

    ##
    #  \~English
    #     \brief Stride in bytes.
    #  \~Russian
    #     \brief Шаг в байтах.
    ir_data_stride_in_bytes: int

    def __init__(self):
        self.ir_frame_rows = 0
        self.ir_frame_cols = 0
        self.ir_frame_2_image_offset_x = 0
        self.ir_frame_2_image_offset_y = 0
        self.ir_frame_2_image_scale_x = 0
        self.ir_frame_2_image_scale_y = 0
        self.ir_horizontal_fov = 0
        self.ir_vertical_fov = 0
        self.ir_frame_data = bytes()
        self.ir_data_stride_in_bytes = 0

    ##
    #  \~English
    #     \brief A function that loads a IR frame from a file and converts it to a uint16 array.
    #  \~Russian
    #     \brief Функция, выполняющая загрузку инфракрасного изображения из файла и преобразующая его в массив uint16.
    @staticmethod
    def read_ir_frame(filepath: str):
        ir_frame = cv2.imread(filepath, -1)
        assert len(ir_frame.shape) == 2

        pyarr = ir_frame.flatten().tolist()
        seq = c_ushort * len(pyarr)
        return seq(*pyarr)

    def __repr__(self):
        return get_repr(self)
