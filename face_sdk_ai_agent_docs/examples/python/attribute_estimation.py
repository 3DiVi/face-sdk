import sys
from face_sdk_3divi import FacerecService, Error

# sdk_root is provided by the user
service = FacerecService.create_service(
    f"{sdk_root}/lib/libfacerec.so", f"{sdk_root}/conf/facerec", f"{sdk_root}/license")

try:
    detector = service.create_processing_block(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"})
    fitter = service.create_processing_block(
        {"unit_type": "FACE_FITTER", "modification": "fda"})
    age_est = service.create_processing_block(
        {"unit_type": "AGE_ESTIMATOR"})
    gender_est = service.create_processing_block(
        {"unit_type": "GENDER_ESTIMATOR"})
    emotion_est = service.create_processing_block(
        {"unit_type": "EMOTION_ESTIMATOR"})

    with open("input.jpg", "rb") as f:
        ioData = service.create_context_from_encoded_image(f.read())

    detector(ioData)
    fitter(ioData)
    age_est(ioData)
    gender_est(ioData)
    emotion_est(ioData)

    if len(ioData["objects"]) == 0:
        print("No faces detected")
        sys.exit(1)

    for i, obj in enumerate(ioData["objects"]):
        print(f"Face {i + 1}:")
        if obj.contains("age"):
            print(f"  Age: {obj['age'].get_long()}")
        if obj.contains("gender"):
            print(f"  Gender: {obj['gender'].get_string()}")
        if obj.contains("emotions"):
            for em in obj["emotions"]:
                print(f"  {em['emotion'].get_string()}: {em['confidence'].get_double():.3f}")

except Error as e:
    print(f"SDK error: {e}")
