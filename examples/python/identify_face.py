from sys import platform

from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.example import CVRawImage

import os
import cv2


def process_one_face(img_path):
    img = cv2.imread(img_path)
    assert img is not None
    raw_img = CVRawImage(img)
    detected = capturer.capture(raw_img)
    print(f'On image detected {len(detected)} faces')
    assert len(detected) > 0
    return recognizer.processing(detected[0])


if platform == "win32":
    default_dll_path = "bin/facerec.dll"
else:
    default_dll_path = "lib/libfacerec.so"

face_sdk_3divi_dir = "../.."
service = FacerecService.create_service(
    os.path.join(face_sdk_3divi_dir, default_dll_path),
    os.path.join(face_sdk_3divi_dir, "conf/facerec"))
print('Service created')

recognizer = service.create_recognizer("recognizer_latest_v30.xml", True, True, False)
print('Recognizer created')

capturer = service.create_capturer(Config("common_capturer_blf_fda_front.xml"))
print('Capturer created')

base_dir = os.path.join(face_sdk_3divi_dir, "bin/set1")
templates = []
base_dir_content = os.listdir(base_dir)
for file in base_dir_content:
    if file.endswith('jpg'):
        path = os.path.join(base_dir, file)
        templates.append(process_one_face(path))

search_filename = "bin/set2/01100.jpg"
search_template = process_one_face(os.path.join(face_sdk_3divi_dir, search_filename))

index = recognizer.create_index(templates, 1)
nearest = recognizer.search([search_template], index, 1)[0][0]
print(f'For image `{search_filename}` closest - `{base_dir_content[nearest.i]}`.\n{nearest.match_result}' )
