import argparse  # for an ArgumentParser
import os.path
import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
from face_sdk_3divi import FacerecService, Config  # FacerecService creates service, Config creates capturer config
from face_sdk_3divi.modules.context import Context


def help_message():
    message = f"\n This program is an example of the Quality Assessment Estimator Processing Block integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] \n"
    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description='Processing Block Example')
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--modification', default="assessment", type=str)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    return parser.parse_args()


def draw_bbox(rect, img, color=(0, 255, 0)):  # an example of a bbox drawing with opencv
    return cv2.rectangle(img, (int(rect[0].get_value() * img.shape[1]), int(rect[1].get_value() * img.shape[0])),
                         (int(rect[2].get_value() * img.shape[1]), int(rect[3].get_value() * img.shape[0])), color, 2)


def print_bbox_coordinates(bbox: Context, image):
    x1 = int(bbox[0].get_value() * image.shape[1])
    y1 = int(bbox[1].get_value() * image.shape[0])
    x2 = int(bbox[2].get_value() * image.shape[1])
    y2 = int(bbox[3].get_value() * image.shape[0])
    print(
        f"Bbox coordinates: ({x1}, {y1}) ({x2}, {y2})")


def quality_estimator(input_image, sdk_path, modification):
    sdk_conf_dir = os.path.join(sdk_path, 'conf', 'facerec')
    if platform == "win32":  # for Windows
        sdk_dll_path = os.path.join(sdk_path, 'bin', 'facerec.dll')
        sdk_onnx_path = os.path.join(sdk_path, 'bin')
    else:  # for Linux
        sdk_dll_path = os.path.join(sdk_path, 'lib', 'libfacerec.so')
        sdk_onnx_path = os.path.join(sdk_path, 'lib')

    service = FacerecService.create_service(  # create FacerecService
        sdk_dll_path,
        sdk_conf_dir,
        f'{sdk_path}/license')

    quality_config = {  # quality block configuration parameters
        "unit_type": "QUALITY_ASSESSMENT_ESTIMATOR",  # required parameter
        "modification": modification,
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
    }
    if (modification == "assessment"):
        quality_config["config_name"] = "quality_assessment.xml"

    quality_block = service.create_processing_block(
        quality_config)  # create quality assessment estimation processing block

    capturer_config = Config("common_capturer_uld_fda.xml")
    capturer = service.create_capturer(capturer_config)

    img: np.ndarray = cv2.imread(input_image, cv2.IMREAD_COLOR)  # read an image from a file
    image = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # convert an image in RGB for correct results
    _, im_png = cv2.imencode('.png', image)  # image encoding, required for convertation in raw sample
    img_bytes = im_png.tobytes()  # copy an image to a byte string

    samples = capturer.capture(img_bytes)  # capture faces in an image

    image_ctx = {  # put an image in container
        "blob": img.tobytes(),
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [dim for dim in img.shape]
    }

    ioData = service.create_context({"image": image_ctx})
    ioData["objects"] = []

    for sample in samples:  # iteration over detected faces in ioData container
        ctx = sample.to_context()
        ioData["objects"].push_back(ctx)  # add results to ioData container

    quality_block(ioData)  # call an estimator and pass a container with a cropped image

    for obj in ioData["objects"]:  # iteration over objects in ioData container
        print("Quality score: ", obj["quality"]["total_score"].get_value())  # print results in console
        print_bbox_coordinates(obj["bbox"], img)
        picture = draw_bbox(obj["bbox"], img)  # visualize bboxes on an image

    cv2.imshow("result", picture)  # an example of a result image visualizing with opencv
    cv2.waitKey(0)  # wait for a key to be pressed to close the window
    cv2.destroyAllWindows()  # close the window


if __name__ == "__main__":
    help_message()
    args = parse_args()

    quality_estimator(args.input_image, args.sdk_path, args.modification)
