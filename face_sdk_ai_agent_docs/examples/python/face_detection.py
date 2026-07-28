import sys
from face_sdk_3divi import FacerecService, Error

# sdk_root is provided by the user
service = FacerecService.create_service(
    f"{sdk_root}/lib/libfacerec.so", f"{sdk_root}/conf/facerec", f"{sdk_root}/license")

try:
    detector = service.create_processing_block(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"})

    with open("input.jpg", "rb") as f:
        ioData = service.create_context_from_encoded_image(f.read())

    detector(ioData)

    for obj in ioData["objects"]:
        bbox = obj["bbox"]
        w = ioData["image"]["shape"][1].get_long()
        h = ioData["image"]["shape"][0].get_long()
        conf = obj["confidence"].get_double()
        print(f"Face: ({int(bbox[0].get_double() * w)}, {int(bbox[1].get_double() * h)})"
              f"-({int(bbox[2].get_double() * w)}, {int(bbox[3].get_double() * h)})"
              f" confidence: {conf:.3f}")

    if len(ioData["objects"]) == 0:
        print("No faces detected")
        sys.exit(1)

except Error as e:
    print(f"SDK error: {e}")
