import argparse  # for an ArgumentParser
import os.path
import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
from face_sdk_3divi import FacerecService  # creates service


def help_message():
    message = f"\n This program is an example of the Human Body Detector Processing block integration \n Usage:" \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] " \
              " [--use_cuda <--use_cuda>] "
    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description='Processing Block Example')
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    parser.add_argument('--use_cuda', action='store_true')  # pass the '--use_cuda' parameter before launch to use cuda acceleration
    return parser.parse_args()


def draw_bbox(rect, img, color=(0, 255, 0)):  # an example of a bbox drawing with opencv
    return cv2.rectangle(img, (int(rect[0] * img.shape[1]), int(rect[1] * img.shape[0])),
                         (int(rect[2] * img.shape[1]), int(rect[3] * img.shape[0])), color, 2)


def body_detector(input_image, sdk_path, use_cuda):
    sdk_conf_dir = os.path.join(sdk_path, 'conf', 'facerec')
    if platform == "win32":  # for Windows
        sdk_dll_path = os.path.join(sdk_path, 'bin', 'facerec.dll')
        sdk_onnx_path = os.path.join(sdk_path, 'bin')
    else:  # for Linux
        sdk_dll_path = os.path.join(sdk_path, 'lib', 'libfacerec.so')
        sdk_onnx_path = os.path.join(sdk_path, 'lib')

    detector_config = {  # detector block configuration parameters
        "unit_type": "HUMAN_BODY_DETECTOR",  # required parameter
        "model_path": sdk_path + "share/bodydetectors/body.enc",  # required
        "confidence_threshold": 0.3,  # optional
        "iou_threshold": 0.5,  # optional
        "use_cuda": use_cuda,  # optional
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
    }

    service = FacerecService.create_service(  # create FacerecService
        sdk_dll_path,
        sdk_conf_dir,
        f'{sdk_path}/license')

    detector_block = service.create_processing_block(detector_config)  # create a human body detector processing block

    img: np.ndarray = cv2.imread(input_image, cv2.IMREAD_COLOR)  # read an image from a file
    image: np.ndarray = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # convert an image in RGB for a correct results

    imgCtx = {  # put an image in container
        "blob": image.tobytes(),
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [dim for dim in image.shape]
    }

    ioData = {"image": imgCtx}  # put imgCtx in container which will be passed to a detector block
    detector_block(ioData)  # call a human detector block and pass a container with an image

    for obj in ioData["objects"]:  # iteration over an objects in ioData container
        print("\n", obj)  # print a results in console
        picture = draw_bbox(obj['bbox'], img)  # visualize bboxes on an image

    cv2.imshow("result", picture)  # an example of a result image visualizing with opencv
    cv2.waitKey(0)  # wait for a key to be pressed to close a window
    cv2.destroyAllWindows()  # close a window


if __name__ == "__main__":
    help_message()
    args = parse_args()

    body_detector(args.input_image, args.sdk_path, args.use_cuda)
