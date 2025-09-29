import argparse  # for an ArgumentParser
import os.path
import time

import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
try:
    from face_sdk_3divi import FacerecService  # FacerecService creates service
    from face_sdk_3divi.modules.context import Context
    from face_sdk_3divi.modules.processing_block import ProcessingBlock
except:
    from face_sdk_3divi import FacerecService, Context, ProcessingBlock # FacerecService creates service

from typing import  Dict, Callable, Tuple

unit_types: Dict[str, str] = {
    "emotions": "EMOTION_ESTIMATOR",
    "age": "AGE_ESTIMATOR",
    "gender": "GENDER_ESTIMATOR",
    "mask": "MASK_ESTIMATOR",
    "glasses": "GLASSES_ESTIMATOR",
    "eye_openness": "EYE_OPENNESS_ESTIMATOR",
    "liveness": "LIVENESS_ESTIMATOR",
    "deepfake": "DEEPFAKE_ESTIMATOR",
    "quality": "QUALITY_ASSESSMENT_ESTIMATOR",
}
bbox_color = (0, 255, 0)
text_color = (0, 0, 255)
font_scale = 0.0  # can be passed as argument


def put_text(image: np.ndarray, org: Tuple[int, int], text: str):
    cv2.putText(image, text, org, cv2.FONT_HERSHEY_DUPLEX, font_scale, text_color, 1)


def help_message():
    message = f"\n This program is an example of the Estimators Processing blocks integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] " \
              " [--use_cuda <--use_cuda>]" \
              " [--unit_type emotions|age|gender|mask|glasses|eye_openness|liveness|quality]" \
              " [--modification <modification>]" \
              " [--version <version>]" \
              " [--disable_show_output <--disable_show_output>]" \
              " [--disable_print_output <--disable_print_output>]" \
              " [--font_scale <font_scale>]"

    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description="Estimators Processing Block Example")
    parser.add_argument("--input_image", type=str, required=True)
    parser.add_argument("--sdk_path", default="../../../", type=str)
    parser.add_argument("--use_cuda",
                        action="store_true")  # pass the '--use_cuda' parameter before launch to use cuda acceleration
    parser.add_argument("--unit_type", type=str, required=True)
    parser.add_argument("--modification", type=str, default="")
    parser.add_argument("--version", type=int, default=-1)
    parser.add_argument("--disable_show_output", action="store_true")
    parser.add_argument("--disable_print_output", action="store_true")
    parser.add_argument("--font_scale", type=float, default=0.45)

    return parser.parse_args()


def get_draw_text(image: np.ndarray, obj: Context, y_offset=0) -> Tuple[int, int]:
    return int(obj["bbox"][0].get_double() * image.shape[1]), int(
        obj["bbox"][3].get_double() * image.shape[0]) + y_offset + 20


def draw_bbox(bbox: Context, image: np.ndarray):  # an example of a bbox drawing with opencv
    return cv2.rectangle(image,
                         (int(bbox[0].get_double() * image.shape[1]), int(bbox[1].get_double() * image.shape[0])),
                         (int(bbox[2].get_double() * image.shape[1]), int(bbox[3].get_double() * image.shape[0])),
                         bbox_color, 2)


def print_bbox(bbox: Context, width: int, height: int):
    print(
        f"Bounding box: ({int(bbox[0].get_double() * width)}, {int(bbox[1].get_double() * height)}), ({int(bbox[2].get_double() * width)}, {int(bbox[3].get_double() * height)})")


def print_emotions(obj: Context):
    for emotion_object in obj["emotions"]:
        print(f"{emotion_object['emotion'].get_string()}: {round(emotion_object['confidence'].get_double(), 2)}")


def show_emotions(image: np.ndarray, obj: Context):
    y_offset = 0

    for emotion_object in obj["emotions"]:
        put_text(image, get_draw_text(image, obj, y_offset),
                 f"{emotion_object['emotion'].get_string()}: {round(emotion_object['confidence'].get_double(), 2)}")

        y_offset += 20


def print_age(obj: Context):
    print(f"Age: {obj['age'].get_long()}")


def show_age(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Age: {obj['age'].get_long()}")


def print_gender(obj: Context):
    print(f"Gender: {obj['gender'].get_string()}")


def show_gender(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Gender: {obj['gender'].get_string()}")


def print_mask(obj: Context):
    print(f"Has mask: {obj['has_medical_mask']['value'].get_bool()}")
    print(f"Confidence: {round(obj['has_medical_mask']['confidence'].get_double(), 2)}")


def show_mask(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Has mask: {obj['has_medical_mask']['value'].get_bool()}")
    put_text(image, get_draw_text(image, obj, 20),
             f"Confidence: {round(obj['has_medical_mask']['confidence'].get_double(), 2)}")


def print_glasses(obj: Context):
    print(f"Has glasses: {obj['glasses']['value'].get_bool()}")
    print(f"Confidence: {round(obj['glasses']['confidence'].get_double(), 2)}")


def show_glasses(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Has glasses: {obj['glasses']['value'].get_bool()}")
    put_text(image, get_draw_text(image, obj, 20),
             f"Confidence: {round(obj['glasses']['confidence'].get_double(), 2)}")


def print_eye_openness(obj: Context):
    print(f"Right eye open: {obj['is_right_eye_open']['value'].get_bool()}")
    print(f"Right eye confidence: {round(obj['is_right_eye_open']['confidence'].get_double(), 2)}")
    print(f"Left eye open: {obj['is_left_eye_open']['value'].get_bool()}")
    print(f"Left eye confidence: {round(obj['is_left_eye_open']['confidence'].get_double(), 2)}")


def show_eye_openness(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Right eye open: {obj['is_right_eye_open']['value'].get_bool()}")
    put_text(image, get_draw_text(image, obj, 20),
             f"Right eye confidence: {round(obj['is_right_eye_open']['confidence'].get_double(), 2)}")
    put_text(image, get_draw_text(image, obj, 40), f"Left eye open: {obj['is_left_eye_open']['value'].get_bool()}")
    put_text(image, get_draw_text(image, obj, 60),
             f"Left eye confidence: {round(obj['is_left_eye_open']['confidence'].get_double(), 2)}")


def print_liveness(obj: Context):
    print(f"Liveness {obj['liveness']['value'].get_string()}")
    print(f"Confidence {round(obj['liveness']['confidence'].get_double(), 2)}")


def show_liveness(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Liveness {obj['liveness']['value'].get_string()}")
    put_text(image, get_draw_text(image, obj, 20), f"Confidence {round(obj['liveness']['confidence'].get_double(), 2)}")

def print_deepfake(obj: Context):
    print(f"Deepfake {obj['deepfake']['value'].get_value()}")
    print(f"Confidence {round(obj['deepfake']['confidence'].get_double(), 2)}")

def show_deepfake(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Deepfake {obj['deepfake']['value'].get_value()}")
    put_text(image, get_draw_text(image, obj, 20), f"Confidence {round(obj['deepfake']['confidence'].get_double(), 2)}")


def print_quality(obj: Context):
    print(f"Quality score: {round(obj['quality']['total_score'].get_double(), 2)}")


def show_quality(image: np.ndarray, obj: Context):
    put_text(image, get_draw_text(image, obj), f"Quality score: {round(obj['quality']['total_score'].get_double(), 2)}")


print_functions: Dict[str, Callable[[Context], None]] = {
    "emotions": print_emotions,
    "age": print_age,
    "gender": print_gender,
    "mask": print_mask,
    "glasses": print_glasses,
    "eye_openness": print_eye_openness,
    "liveness": print_liveness,
    "deepfake": print_deepfake,
    "quality": print_quality,
}


def print_output(unit_type: str, data: Context):
    width = data["image"]["shape"][1].get_long()
    height = data["image"]["shape"][0].get_long()

    for obj in data["objects"]:
        print_bbox(obj["bbox"], width, height)

        print_functions[unit_type](obj)

        print()


show_functions: Dict[str, Callable[[np.ndarray, Context], None]] = {
    "emotions": show_emotions,
    "age": show_age,
    "gender": show_gender,
    "mask": show_mask,
    "glasses": show_glasses,
    "eye_openness": show_eye_openness,
    "liveness": show_liveness,
    "deepfake": show_deepfake,
    "quality": show_quality,
}


def show_output(unit_type: str, input_image: str, data: Context):
    image = cv2.imread(input_image)

    for obj in data["objects"]:
        draw_bbox(obj["bbox"], image)

        show_functions[unit_type](image, obj)

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


def create_estimator(unit_type: str, modification: str, version: int, use_cuda: bool) -> ProcessingBlock:
    config = service.create_context({
        "unit_type": unit_types[unit_type],
        "use_cuda": use_cuda,
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
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

    # Create processing pipeline: detect face -> construct face keypoints -> estimate
    pipeline: Dict[str, ProcessingBlock] = {
        "detector": create_detector(args.use_cuda),
        "fitter": create_fitter(args.use_cuda),
        "estimator": create_estimator(args.unit_type, args.modification, args.version, args.use_cuda)
    }
    data = service.create_context_from_encoded_image(read_image(args.input_image))

    for name, processing_block in pipeline.items():
        start_time = time.time()
        processing_block(data)  # run each processing block and store all results in data Context
        end_time = time.time()

        print(f"Inference time of {name} is {round(end_time - start_time, 3)} seconds")

    if not args.disable_print_output:
        print_output(args.unit_type, data)

    if not args.disable_show_output:
        font_scale = args.font_scale

        show_output(args.unit_type, args.input_image, data)
