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
    liveness = service.create_processing_block({
        "unit_type": "LIVENESS_ESTIMATOR",
        "modification": "2d",
        "confidence_threshold": 0.94,
    })

    with open("input.jpg", "rb") as f:
        ioData = service.create_context_from_encoded_image(f.read())

    detector(ioData)
    fitter(ioData)

    # Liveness requires exactly one face per frame
    if len(ioData["objects"]) != 1:
        print(f"Exactly 1 face required for liveness, got {len(ioData['objects'])}")
        sys.exit(1)

    liveness(ioData)

    obj = ioData["objects"][0]
    value = obj["liveness"]["value"].get_string()
    conf = obj["liveness"]["confidence"].get_double()
    print(f"Liveness: {value} confidence: {conf:.4f}")

except Error as e:
    print(f"SDK error: {e}")
