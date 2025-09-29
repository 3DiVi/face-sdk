import argparse  # for an ArgumentParser
import os.path
import time
from typing import Dict, Sequence

import numpy as np
import cv2  # for reading images, visualising window and rectangle creation

from sys import platform  # for a platform identification
try:
    from face_sdk_3divi import FacerecService  # FacerecService creates service
    from face_sdk_3divi.modules.context import Context
    from face_sdk_3divi.modules.processing_block import ProcessingBlock
except:
    from face_sdk_3divi import FacerecService, Context, ProcessingBlock # FacerecService creates service

bbox_color = (0, 255, 0)
text_color = (0, 0, 255)
font_scale = 0.0  # can be passed as argument


def help_message():
    message = f"\n This program is an example of the Fitters Processing blocks integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] " \
              " [--use_cuda <--use_cuda>]" \
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
        obj["bbox"][3].get_double() * image.shape[0]) + y_offset + 20


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description="Fitters Processing Block Example")
    parser.add_argument("--input_image", type=str, required=True)
    parser.add_argument("--sdk_path", default="../../../", type=str)
    parser.add_argument("--use_cuda",
                        action="store_true")  # pass the '--use_cuda' parameter before launch to use cuda acceleration
    parser.add_argument("--modification", type=str, default="fda")
    parser.add_argument("--disable_show_output", action="store_true")
    parser.add_argument("--disable_print_output", action="store_true")
    parser.add_argument("--font_scale", type=float, default=0.45)

    return parser.parse_args()


def draw_bbox(bbox: Context, image: np.ndarray):  # an example of a bbox drawing with opencv
    return cv2.rectangle(image,
                         (int(bbox[0].get_double() * image.shape[1]), int(bbox[1].get_double() * image.shape[0])),
                         (int(bbox[2].get_double() * image.shape[1]), int(bbox[3].get_double() * image.shape[0])),
                         bbox_color, 2)


def draw_angles(obj: Context, image: np.ndarray):
    y_offset = 0
    pose = obj["pose"]

    put_text(image, get_draw_text(image, obj, y_offset), f"Yaw: {round(pose['yaw'].get_double(), 3)}")
    y_offset += 20

    put_text(image, get_draw_text(image, obj, y_offset), f"Pitch: {round(pose['pitch'].get_double(), 3)}")
    y_offset += 20

    put_text(image, get_draw_text(image, obj, y_offset), f"Roll: {round(pose['roll'].get_double(), 3)}")


def draw_keypoints(obj: Context, image: np.ndarray):
    keypoints = obj["keypoints"]
    width = image.shape[1]
    height = image.shape[0]

    for point in keypoints["points"]:
        proj = point["proj"]

        cv2.circle(image, (int(proj[0].get_double() * width), int(proj[1].get_double() * height)), 2, (255, 0, 0), 1)


def print_bbox(bbox: Context, width: int, height: int):
    print(
        f"Bounding box: ({int(bbox[0].get_double() * width)}, {int(bbox[1].get_double() * height)}), ({int(bbox[2].get_double() * width)}, {int(bbox[3].get_double() * height)})")


def print_angles(pose: Context):
    print(f"Yaw: {round(pose['yaw'].get_double(), 3)}")
    print(f"Pitch: {round(pose['pitch'].get_double(), 3)}")
    print(f"Roll: {round(pose['roll'].get_double(), 3)}")


def print_keypoints(keypoints: Context, width: int, height: int):
    named_points = keypoints.keys()

    print("Named points:")

    for named_point in named_points:
        point = keypoints[named_point]
        if point.is_object():  # points array
            proj = point["proj"]

            print(f"{named_point}: ({int(proj[0].get_double() * width)}, {int(proj[1].get_double() * height)})")

def print_output(data: Context):
    width = data["image"]["shape"][1].get_long()
    height = data["image"]["shape"][0].get_long()

    for obj in data["objects"]:
        print_bbox(obj["bbox"], width, height)

        print_angles(obj["pose"])

        print_keypoints(obj["keypoints"], width, height)

        print()


def show_output(input_image: str, data: Context):
    image = cv2.imread(input_image)

    for obj in data["objects"]:
        draw_bbox(obj["bbox"], image)

        draw_angles(obj, image)

        draw_keypoints(obj, image)

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


def create_detector(use_cuda: bool) -> ProcessingBlock:
    return service.create_processing_block(service.create_context({
        "unit_type": "FACE_DETECTOR",
        "modification": "ssyv_light",
        "use_cuda": use_cuda,
    }))


def create_fitter(modification: str, use_cuda: bool) -> ProcessingBlock:
    return service.create_processing_block(service.create_context({
        "unit_type": "FACE_FITTER",
        "modification": modification,
        "use_cuda": use_cuda,
    }))


if __name__ == "__main__":
    help_message()
    args = parse_args()

    service = create_service(args.sdk_path)

    # Create processing pipeline: detect face -> construct face keypoints
    pipeline: Dict[str, ProcessingBlock] = {
        "detector": create_detector(args.use_cuda),
        "fitter": create_fitter(args.modification, args.use_cuda)
    }
    data = service.create_context_from_encoded_image(read_image(args.input_image))

    for name, processing_block in pipeline.items():
        start_time = time.time()
        processing_block(data)  # run each processing block and store all results in data Context
        end_time = time.time()

        print(f"Inference time of {name} is {round(end_time - start_time, 3)} seconds")

    if not args.disable_print_output:
        print_output(data)

    if not args.disable_show_output:
        font_scale = args.font_scale

        show_output(args.input_image, data)
