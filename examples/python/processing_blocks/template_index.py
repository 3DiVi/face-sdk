import argparse  # for an ArgumentParser
import json
import os.path
import imghdr
import shutil
from io import BytesIO
from typing import List

import sys
try:
    from face_sdk_3divi import FacerecService  # FacerecService creates service
    from face_sdk_3divi.modules.context_template import ContextTemplate
    from face_sdk_3divi.modules.processing_block import ProcessingBlock
    from face_sdk_3divi.modules.dynamic_template_index import DynamicTemplateIndex
except:
    from face_sdk_3divi import FacerecService, ContextTemplate, ProcessingBlock, DynamicTemplateIndex # FacerecService creates service
    

def help_message():
    message = f"\n This program is an example of the TemplateIndex integration \n Usage: " \
              " [--sdk_path <sdk_root_dir>] " \
              " [--use_cuda <--use_cuda>]" \
              " [--modification <modification>]" \
              " [--input_image <input_image>]" \
              " [--capacity <capacity>]" \
              " [--templates_description <templates_description>]" \
              " [--images_paths <images_path images_path...>]" \
              " [--templates_paths <templates_path templates_path...>]"

    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description="Template Index Example")
    parser.add_argument("--sdk_path", default="../../../", type=str)
    parser.add_argument("--use_cuda",
                        action="store_true")  # pass the '--use_cuda' parameter before launch to use cuda acceleration
    parser.add_argument("--input_image", type=str, required=True)
    parser.add_argument("--capacity", type=int, default=1000)
    parser.add_argument("--modification", type=str, default="1000")
    parser.add_argument("--images_paths", nargs="+", type=str, default=["../../../bin/set1"])
    parser.add_argument("--templates_paths", nargs="+", type=str)
    parser.add_argument("--templates_description", type=str)

    return parser.parse_args()


def read_image(input_image: str) -> bytes:
    with open(input_image, "rb") as file:
        return file.read()


def create_service(sdk_path: str) -> FacerecService:
    sdk_conf_dir = os.path.join(sdk_path, "conf", "facerec")
    if sys.platform == "win32":  # for Windows
        sdk_dll_path = os.path.join(sdk_path, "bin", "facerec.dll")
    else:  # for Linux
        sdk_dll_path = os.path.join(sdk_path, "lib", "libfacerec.so")

    return FacerecService.create_service(  # create FacerecService
        sdk_dll_path,
        sdk_conf_dir,
        f'{sdk_path}/license')


def create_detector(use_cuda: bool) -> ProcessingBlock:
    return service.create_processing_block({
        "unit_type": "FACE_DETECTOR",
        "modification": "ssyv_light",
        "use_cuda": use_cuda,
    })


def create_fitter(use_cuda: bool) -> ProcessingBlock:
    return service.create_processing_block({
        "unit_type": "FACE_FITTER",
        "modification": "fda",
        "use_cuda": use_cuda,
    })


def create_template_extractor(modification: str, use_cuda: bool):
    return service.create_processing_block({
        "unit_type": "FACE_TEMPLATE_EXTRACTOR",
        "modification": modification,
        "use_cuda": use_cuda,
    })


def is_image(file_path):
    try:
        return imghdr.what(file_path) is not None
    except FileNotFoundError:
        return False


def create_template_index(images_path: str, modification: str, pipeline: List[ProcessingBlock]):

    config = service.create_context({"modification": modification, "version": 1})
    template_index = service.create_dynamic_template_index(config)

    for root, dirs, files in os.walk(images_path):
        for file in files:
            file_path = os.path.join(root, file)

            if not is_image(file_path):
                continue

            data = service.create_context_from_encoded_image(read_image(file_path))

            for processing_block in pipeline:
                processing_block(data)

            if not data.contains("objects"):
                continue

            template_index.add(
                data["objects"][0]["face_template"]["template"].get_value(),
                file_path
            )

    return template_index


def load_templates(templates_path: str, template_index: DynamicTemplateIndex, templates_description: str):
    file_size = os.path.getsize(templates_path)
    templates: List[ContextTemplate] = []
    uuids: List[str] = []
    description_file_name = os.path.basename(templates_path)

    with open(templates_description, "r") as file:
        description = json.loads(file.read())

    with open(templates_path, "rb") as file:
        templates_data = BytesIO()

        shutil.copyfileobj(file, templates_data)

        templates_data.seek(0)

        while True:
            offset = templates_data.tell()

            if offset == file_size:
                break

            templates.append(service.load_context_template(templates_data))

            if description_file_name in description:
                uuids.append(f"Image {description[description_file_name][str(offset)]} from templates file: {templates_path}")
            else:
                uuids.append(f"{templates_path}_{offset}")

            if len(templates) == 100:
                template_index.add(templates, uuids)

                templates.clear()
                uuids.clear()

    if len(templates) > 0:
        template_index.add(templates, uuids)

        templates.clear()
        uuids.clear()


if __name__ == "__main__":
    help_message()
    args = parse_args()

    if not (args.images_paths or args.templates_paths):
        args.error("At least one of --images_paths or --templates_paths is required.")

    service = create_service(args.sdk_path)

    if sys.platform == "win32":  # for Windows
        sdk_onnx_path = os.path.join(args.sdk_path, "bin")
    else:  # for Linux
        sdk_onnx_path = os.path.join(args.sdk_path, "lib")

    # Create processing pipeline: detect face -> construct face keypoints -> extract face template
    pipeline: List[ProcessingBlock] = [
        create_detector(args.use_cuda),
        create_fitter(args.use_cuda),
        create_template_extractor(args.modification, args.use_cuda)
    ]

    config = service.create_context({
        "capacity": args.capacity,
        "modification": args.modification,
        "version": 1,
    })
    template_index = service.create_dynamic_template_index(config)

    if args.images_paths is not None:
        print("Images processing...")
        for images_path in args.images_paths:
            template_index.concat(create_template_index(images_path, args.modification, pipeline))

    if args.templates_paths is not None:
        print("Templates loading....")
        for templates_path in args.templates_paths:
            load_templates(templates_path, template_index, args.templates_description)

    data = service.create_context_from_encoded_image(read_image(args.input_image))

    print(f"Process {args.input_image}")

    for processing_block in pipeline:
        processing_block(data)

    if not data.contains("objects"):
        print(f"No face in {args.input_image}")

        sys.exit(1)

    matcher_module = service.create_processing_block({
        "unit_type": "MATCHER_MODULE",
        "modification": args.modification
    })
    matcher_data = service.create_context({
        "template_index": template_index,
        "queries": data["objects"][0]["face_template"]["template"],
    })

    print("Matching")

    matcher_module(matcher_data)

    result = matcher_data["results"][0]

    print(f"UUID: {result['uuid'].get_value()}")
    print(f"Score: {result['score'].get_value()}")
