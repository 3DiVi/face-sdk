from sys import platform

from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.example import CVRawImage

import os
import cv2


def read_image(img_path):
    img = cv2.imread(img_path)
    assert img is not None
    return img


def detect(img):
    raw_img = CVRawImage(img)
    detected = capturer.capture(raw_img)
    print(f'On image detected {len(detected)} faces')
    return detected


if platform == "win32":
    default_dll_path = "bin/facerec.dll"
else:
    default_dll_path = "lib/libfacerec.so"

face_sdk_3divi_dir = "../.."
service = FacerecService.create_service(
    os.path.join(face_sdk_3divi_dir, default_dll_path),
    os.path.join(face_sdk_3divi_dir, "conf/facerec"))
print('Service created')

capturer = service.create_capturer(Config("common_capturer_blf_fda_back.xml"))
print('Capturer created')
image = read_image(os.path.join(face_sdk_3divi_dir, "bin/set1", "01100.jpg"))
samples = detect(image)

for sample in samples:
    rect = sample.get_rectangle()
    cv2.rectangle(image, (rect.x, rect.y), (rect.x + rect.width, rect.y + rect.height), (0, 255, 0))
    for pt in sample.get_landmarks():
        cv2.circle(image, (int(pt.x), int(pt.y)), 1, (255, 0, 0))

cv2.imshow('Image', image)
cv2.waitKey()
