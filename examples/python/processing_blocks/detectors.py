import argparse  # for an ArgumentParser
import os.path
import time

import cv2  # for reading images, visualising window and rectangle creation

from sys import platform  # for a platform identification

import numpy as np
from face_sdk_3divi import FacerecService  # FacerecService creates service
from face_sdk_3divi.modules.context import Context
from face_sdk_3divi.modules.processing_block import ProcessingBlock
from typing import Dict, Sequence

bbox_color = (0, 255, 0)
text_color = (0, 0, 255)
font_scale = 0.0  # can be passed as argument

unit_types: Dict[str, str] = {
    "face": "FACE_DETECTOR",
    "body": "HUMAN_BODY_DETECTOR",
    "objects": "OBJECT_DETECTOR"
}


def help_message():
    message = f"\n This program is an example of the Detectors Processing blocks integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] " \
              " [--use_cuda <--use_cuda>]" \
              " [--unit_type face|body|objects]" \
              " [--modification <modification>]" \
              " [--version <version>]" \
              " [--disable_show_output <--disable_show_output>]" \
              " [--disable_print_output <--disable_print_output>]" \
              " [--font_scale <font_scale>]"

    print(message)


def put_text(image: np.ndarray, org: Sequence[int], text: str):
    cv2.putText(image, text, org, cv2.FONT_HERSHEY_DUPLEX, font_scale, text_color, 1)


def get_draw_text(image: np.ndarray, obj: Context, y_offset=0) -> Sequence[int]:
    return int(obj["bbox"][0].get_double() * image.shape[1]), int(
        obj["bbox"][1].get_double() * image.shape[0]) - int(font_scale * y_offset) - int(font_scale * 10)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description="Detectors Processing Block Example")
    parser.add_argument("--input_image", type=str, required=True)
    parser.add_argument("--sdk_path", default="../../../", type=str)
    parser.add_argument("--use_cuda",
                        action="store_true")  # pass the '--use_cuda' parameter before launch to use cuda acceleration
    parser.add_argument("--unit_type", type=str, default="face")
    parser.add_argument("--modification", type=str, default="")
    parser.add_argument("--version", type=int, default=-1)
    parser.add_argument("--disable_show_output", action="store_true")
    parser.add_argument("--disable_print_output", action="store_true")
    parser.add_argument("--font_scale", type=float, default=0.45)

    return parser.parse_args()


def draw_bbox(bbox: Context, image: np.ndarray):  # an example of a bbox drawing with opencv
    return cv2.rectangle(image,
                         (int(bbox[0].get_double() * image.shape[1]), int(bbox[1].get_double() * image.shape[0])),
                         (int(bbox[2].get_double() * image.shape[1]), int(bbox[3].get_double() * image.shape[0])),
                         bbox_color, 2)


def print_bbox(bbox: Context, width: int, height: int):
    print(
        f"Bounding box: ({int(bbox[0].get_double() * width)}, {int(bbox[1].get_double() * height)}), ({int(bbox[2].get_double() * width)}, {int(bbox[3].get_double() * height)})")


def print_output(data: Context):
    width = data["image"]["shape"][1].get_long()
    height = data["image"]["shape"][0].get_long()

    for obj in data["objects"]:
        print_bbox(obj["bbox"], width, height)

        print(f"Confidence: {round(obj['confidence'].get_double(), 2)}")
        print(f"Class: {obj['class'].get_string()}", end="\n\n")


def show_output(input_image: str, data: Context):
    image = cv2.imread(input_image)

    for obj in data["objects"]:
        draw_bbox(obj["bbox"], image)

        put_text(image, get_draw_text(image, obj), f"Confidence: {round(obj['confidence'].get_double(), 2)}")
        put_text(image, get_draw_text(image, obj, 25), f"Class: {obj['class'].get_string()}")

    cv2.imshow("result", image)  # an example of a result image visualizing with opencv
    cv2.waitKey()  # wait for a key to be pressed to close the window
    cv2.destroyAllWindows()  # close the window


def read_image(input_image: str) -> bytes:
    with open(input_image, "rb") as file:
        return file.read()


def create_service(sdk_path: str) -> FacerecService:
    sdk_conf_dir = os.path.join(sdk_path, "conf", "facerec")
    if platform == "win32":  # for Windows
        sdk_dll_path = os.path.join(sdk_path, "bin", "facerec.dll")
    else:  # for Linux
        sdk_dll_path = os.path.join(sdk_path, "lib", "libfacerec.so")

    return FacerecService.create_service(  # create FacerecService
        sdk_dll_path,
        sdk_conf_dir,
        f'{sdk_path}/license')


def create_detector(unit_type: str, modification: str, version: int, use_cuda: bool) -> ProcessingBlock:
    config = service.create_context({
        "unit_type": unit_types[unit_type],
        "use_cuda": use_cuda,
    })

    if len(modification) != 0:
        config["modification"] = modification

    if version != -1:
        config["version"] = version

    return service.create_processing_block(config)


if __name__ == "__main__":
    help_message()
    args = parse_args()

    service = create_service(args.sdk_path)

    if platform == "win32":  # for Windows
        sdk_onnx_path = os.path.join(args.sdk_path, "bin")
    else:  # for Linux
        sdk_onnx_path = os.path.join(args.sdk_path, "lib")

    # Create detector
    detector = create_detector(args.unit_type, args.modification, args.version, args.use_cuda)
    data = service.create_context_from_encoded_image(read_image(args.input_image))

    start_time = time.time()
    detector(data)
    end_time = time.time()

    print(f"Inference time of detector is {round(end_time - start_time, 3)} seconds")

    if not args.disable_print_output:
        print_output(data)

    if not args.disable_show_output:
        font_scale = args.font_scale

        show_output(args.input_image, data)
