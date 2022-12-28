from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.example import CVRawImage

import os
import cv2


def detect_and_save_templ(img_path, save_path):
    img = cv2.imread(img_path)
    assert img is not None
    raw_img = CVRawImage(img)
    detected = capturer.capture(raw_img)
    assert len(detected) > 0
    print('Detected', len(detected), 'faces')
    with open(save_path, 'wb') as f:
        recognizer.processing(detected[0]).save(f)


face_sdk_3divi_dir = "../.."
service = FacerecService.create_service(
    os.path.join(face_sdk_3divi_dir, "lib/libfacerec.so"),
    os.path.join(face_sdk_3divi_dir, "conf/facerec"))
print('Service created')

recognizer = service.create_recognizer("recognizer_latest_v30.xml", True, True, False)
print('Recognizer created')

capturer = service.create_capturer(Config("common_capturer_blf_fda_front.xml"))
print('Capturer created')

save_path = 'test_emb1.bin'
detect_and_save_templ(os.path.join(face_sdk_3divi_dir, "bin/set1", "01100.jpg"), save_path)

with open(save_path, 'rb') as f:
    template1 = recognizer.load_template(f)
