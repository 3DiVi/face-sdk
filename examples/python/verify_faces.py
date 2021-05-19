from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.example import CVRawImage

import os
import cv2


def process_one_face(img_path):
    img = cv2.imread(img_path)
    assert img is not None
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    raw_img = CVRawImage(img)
    detected = capturer.capture(raw_img)
    print(f'On image detected {len(detected)} faces')
    assert len(detected) > 0
    return recognizer.processing(detected[0])


face_sdk_dir = "../.."
service = FacerecService.create_service(
    os.path.join(face_sdk_dir, "lib/libfacerec.so"),
    os.path.join(face_sdk_dir, "conf/facerec"))
print('Service created')

recognizer = service.create_recognizer("method9v30_recognizer.xml", True, True, False)
print('Recognizer created')

capturer = service.create_capturer(Config("common_capturer_blf_fda_front.xml"))
print('Capturer created')

template1 = process_one_face(os.path.join(face_sdk_dir, "bin/set1", "01100.jpg"))
template2 = process_one_face(os.path.join(face_sdk_dir, "bin/set2", "01100.jpg"))

print(recognizer.verify_match(template1, template2))
