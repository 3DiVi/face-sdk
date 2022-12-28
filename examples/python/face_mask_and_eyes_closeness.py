import os
import cv2

from face_sdk_3divi import FacerecService, Config
from face_sdk_3divi.modules.face_attributes_estimator import MaskAttribute
from face_sdk_3divi.example import CVRawImage


def detect(img_path):
    img = cv2.imread(img_path)
    assert img is not None
    raw_img = CVRawImage(img)
    detected = capturer.capture(raw_img)
    print(f'On image detected {len(detected)} faces')
    return detected


face_sdk_3divi_dir = "../.."
service = FacerecService.create_service(
    os.path.join(face_sdk_3divi_dir, "lib/libfacerec.so"),
    os.path.join(face_sdk_3divi_dir, "conf/facerec"))
print('Service created')

capturer = service.create_capturer(Config("common_capturer_blf_fda_back.xml"))
print('Capturer created')

masked_face_estimator = service.create_face_attributes_estimator("face_mask_estimator.xml")
print('Masked face estimator created')

eyes_openness_estimator = service.create_face_attributes_estimator("eyes_openness_estimator_v2.xml")

samples = detect(os.path.join(face_sdk_3divi_dir, "bin/set1", "01100.jpg"))
for sample in samples:
    masked_face_res = masked_face_estimator.estimate(sample)
    if masked_face_res.mask_attribute == MaskAttribute.HAS_MASK:
        print(f"Face with mask. Score {masked_face_res.score}")
    else:
        print(f"Face without mask. Score {masked_face_res.score}")
    masked_face_res = eyes_openness_estimator.estimate(sample)
    left_eye_state = masked_face_res.left_eye_state
    right_eye_state = masked_face_res.right_eye_state
    print(f'Left eye state: {left_eye_state.eye_state.name.lower()}. Score: {left_eye_state.score}')
    print(f'Right eye state: {right_eye_state.eye_state.name.lower()}. Score: {right_eye_state.score}')
