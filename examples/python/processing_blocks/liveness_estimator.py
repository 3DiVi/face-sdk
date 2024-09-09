import argparse  # for an ArgumentParser
import os.path
import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
from face_sdk_3divi import FacerecService  # creates service


def help_message():
    message = f"\n This program is an example of the Liveness Estimator Processing Block integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] "
    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description='Processing Block Example')
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    return parser.parse_args()


def draw_bbox(rect, img, color=(0, 255, 0)):  # an example of a bbox drawing with opencv
    return cv2.rectangle(img, (int(rect[0] * img.shape[1]), int(rect[1] * img.shape[0])),
                         (int(rect[2] * img.shape[1]), int(rect[3] * img.shape[0])), color, 2)


def liveness_estimator(input_image, sdk_path):
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

    liveness_config = {  # liveness block configuration parameters
        "unit_type": "LIVENESS_ESTIMATOR",  # required parameter
        "modification": "2d_additional_check",
        "model_path": "",  # required
        "sdk_path": sdk_path,  # optional
        "capturer_config_name": "common_capturer_uld_fda.xml",  # optional
        "config_name": "liveness_2d_estimator_v3.xml",  # optional
        "facerec_conf_dir": sdk_conf_dir,  # optional
        "dll_path": sdk_dll_path,  # optional
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
    }
    liveness_block = service.create_processing_block(liveness_config)  # create liveness estimation processing block

    img: np.ndarray = cv2.imread(input_image, cv2.IMREAD_COLOR)  # read an image from a file
    imgCtx = {  # put an image in container
        "blob": img.tobytes(),
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [dim for dim in img.shape]
    }

    ioData = {"image": imgCtx}  # put imgCtx in container which will be passed to a liveness block
    liveness_block(ioData)  # call a liveness block and pass a container with an image

    for obj in ioData["objects"]:  # iteration over objects in ioData container
        print("\n", obj)  # print results in console
        picture = draw_bbox(obj["bbox"], img)  # visualize bboxes on an image

    cv2.imshow("result", picture)  # an example of a result image visualizing with opencv
    cv2.waitKey(0)  # wait for a key to be pressed to close the window
    cv2.destroyAllWindows()  # close the window


if __name__ == "__main__":
    help_message()
    args = parse_args()

    liveness_estimator(args.input_image, args.sdk_path)
