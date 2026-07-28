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
    extractor = service.create_processing_block(
        {"unit_type": "FACE_TEMPLATE_EXTRACTOR", "modification": "1000"})
    verifier = service.create_processing_block(
        {"unit_type": "VERIFICATION_MODULE", "modification": "1000"})

    # Process first face
    with open("face1.jpg", "rb") as f:
        ioData1 = service.create_context_from_encoded_image(f.read())
    detector(ioData1)
    fitter(ioData1)
    extractor(ioData1)
    if len(ioData1["objects"]) == 0:
        print("No face in face1.jpg")
        sys.exit(1)

    # Process second face
    with open("face2.jpg", "rb") as f:
        ioData2 = service.create_context_from_encoded_image(f.read())
    detector(ioData2)
    fitter(ioData2)
    extractor(ioData2)
    if len(ioData2["objects"]) == 0:
        print("No face in face2.jpg")
        sys.exit(1)

    # Compare templates
    verData = service.create_context({
        "template1": ioData1["objects"][0]["face_template"],
        "template2": ioData2["objects"][0]["face_template"],
    })
    verifier(verData)

    score = verData["result"]["score"].get_double()
    distance = verData["result"]["distance"].get_double()
    print(f"Score: {score:.4f} Distance: {distance:.4f}")
    print(f"Verdict: {'match' if score >= 0.85 else 'no match'}")

except Error as e:
    print(f"SDK error: {e}")
