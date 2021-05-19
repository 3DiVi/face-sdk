import cv2
import sys

from face_sdk_3divi.modules import raw_image
from .image_and_depth_source import ImageAndDepthSource, ImageAndDepth


class OpenCVSource(ImageAndDepthSource):

    def __init__(self, cam_or_url: str):
        super().__init__()
        assert cam_or_url

        try:
            int(cam_or_url)
            is_webcam_stream = True
        except ValueError:
            is_webcam_stream = False

        if not is_webcam_stream:
            # open stream
            print(f"opening stream '{cam_or_url}'")
            self.capturer = cv2.VideoCapture(cam_or_url)
            assert self.capturer.isOpened(), "webcam not opened"
        else:
            # convert to integer
            cam_id = int(cam_or_url)
            assert cam_id >= 0, "wrong webcam id"

            # open webcam
            print(f"opening stream '{cam_id}'")
            if sys.platform == "linux":
                self.capturer = cv2.VideoCapture(cam_id)
            else:
                self.capturer = cv2.VideoCapture(cam_id, cv2.CAP_DSHOW)

            assert self.capturer.isOpened(), "webcam not opened"

            # set resolution
            self.capturer.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
            self.capturer.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

            assert self.capturer.isOpened(), "webcam not opened"

        # sometimes first few frames can be empty even if camera is good
        # so skip few frames
        for i in range(10):
            _, __ = self.capturer.read()

        # check first two frames
        _, image1 = self.capturer.read()
        _, image2 = self.capturer.read()
        print("image1 size:", image1.shape)
        print("image2 size:", image2.shape)

        assert (image1.dtype == 'uint8') and \
               (len(image1.shape) == 3) and (image1.shape[2] == 3) and \
               (image1.shape == image2.shape), "error opening webcam or stream"

    def get(self, data: ImageAndDepth):
        _, image = self.capturer.read()

        if image is not None:
            data.color_image = image.flatten()
            data.color_width = image.shape[1]
            data.color_height = image.shape[0]
            data.color_format = raw_image.Format.FORMAT_BGR
