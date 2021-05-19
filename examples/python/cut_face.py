from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.modules.raw_image import Format
from face_sdk_3divi.example import CVRawImage

import os
import cv2
import numpy as np


def detect(img_path):
    img = cv2.imread(img_path)
    assert img is not None
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    raw_img = CVRawImage(img)
    detected = capturer.capture(raw_img)
    print(f'On image detected {len(detected)} faces')
    return detected


face_sdk_dir = "../.."
service = FacerecService.create_service(
    os.path.join(face_sdk_dir, "lib/libfacerec.so"),
    os.path.join(face_sdk_dir, "conf/facerec"))
print('Service created')

capturer = service.create_capturer(Config("common_capturer_uld_fda.xml"))
print('Capturer created')

samples = detect(os.path.join(face_sdk_dir, "bin/set1", "01100.jpg"))
for i, sample in enumerate(samples):
    raw_img = sample.cut_face_raw_image(Format.FORMAT_GRAY)
    img_crop = np.frombuffer(raw_img.data, dtype=np.uint8).reshape([raw_img.height, raw_img.width])
    cv2.imshow('Image', img_crop)
    cv2.waitKey()

