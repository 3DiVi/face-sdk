##
#  \file cv_raw_image.py
#  \~English
#     \brief CVRawImage - example of implementation of the RawImage interface via OpenCV.
#  \~Russian
#     \brief CVRawImage - пример реализации интерфейса RawImage через OpenCV.

import cv2
from face_sdk_3divi.modules.error import Error
from face_sdk_3divi.modules.raw_image import RawImage, Format


##
# \~English
#    \brief Example of implementation of the RawImage interface via OpenCV.
# \~Russian
#    \brief Пример реализации интерфейса RawImage через OpenCV.
class CVRawImage(RawImage):

    def __init__(self, image):
        self._mat = image
        ret, im_bmp = cv2.imencode('.bmp', self._mat)
        assert ret, "A image was not encoded"
        data = im_bmp.tobytes()

        super().__init__(self._mat.shape[1],
                         self._mat.shape[0],
                         self.get_format(),
                         data)

    ##
    # \~English
    #    \brief Get an image format.
    #
    #    \return
    #      -1, if the stored cv::Mat object is invalid
    #      face_sdk_3divi.modules.raw_image.Format.FORMAT_GRAY, if the mat type is CV_8UC1,
    #      face_sdk_3divi.modules.raw_image.Format.FORMAT_BGR, if the mat type is CV_8UC3.
    #
    # \~Russian
    #    \brief Получить формат изображения.
    #
    #    \return -1, если хранимое изображение некорректно
    #      face_sdk_3divi.modules.raw_image.Format.FORMAT_GRAY, если mat имеет формат CV_8UC1,
    #      face_sdk_3divi.modules.raw_image.Format.FORMAT_BGR, если mat имеет формат CV_8UC3.
    def get_format(self):
        if len(self._mat.shape) == 2 or self._mat.shape[2] == 1:
            return Format.FORMAT_GRAY

        if len(self._mat.shape) == 3 and self._mat.dtype == 'uint8':
            return Format.FORMAT_BGR

        raise Error(0xceef0f6a, "CVRawImage format not recognized!")

    ##
    # \~English
    #    \brief Get a constant reference to the stored image.
    #    \return Constant reference to the stored image.
    #
    # \~Russian
    #    \brief Получить константную ссылку на хранимое изображение.
    #    \return Константная ссылка на хранимое изображение.
    def mat(self):
        return self._mat

