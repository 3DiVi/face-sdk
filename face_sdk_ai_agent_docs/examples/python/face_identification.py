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
    matcher = service.create_processing_block(
        {"unit_type": "MATCHER_MODULE", "modification": "1000"})

    # Create DynamicTemplateIndex and enroll faces
    index = service.create_dynamic_template_index(
        service.create_context({"modification": "1000", "version": 1}))

    for img_path in ["face1.jpg", "face2.jpg", "face3.jpg"]:
        with open(img_path, "rb") as f:
            ioData = service.create_context_from_encoded_image(f.read())
        detector(ioData)
        fitter(ioData)
        extractor(ioData)
        for obj in ioData["objects"]:
            if obj["class"].get_string() != "face":
                continue
            templ = obj["face_template"]["template"].get_value()
            uuid = obj["face_template"]["uuid"].get_value()
            index.add(templ, uuid)
            print(f"Enrolled: {uuid}")

    # Search with query face
    with open("query.jpg", "rb") as f:
        queryData = service.create_context_from_encoded_image(f.read())
    detector(queryData)
    fitter(queryData)
    extractor(queryData)

    if len(queryData["objects"]) == 0:
        print("No faces detected")
        sys.exit(1)

    matcher_data = service.create_context({
        "template_index": index,
        "queries": queryData["objects"][0]["face_template"]["template"],
    })
    matcher_data["knn"] = 5
    matcher(matcher_data)

    for result in matcher_data["results"]:
        uuid = result["uuid"].get_value()
        score = result["score"].get_value()
        print(f"Match: {uuid} Score: {score:.4f}")

except Error as e:
    print(f"SDK error: {e}")
