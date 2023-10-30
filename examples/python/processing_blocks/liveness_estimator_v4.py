import argparse  # for an ArgumentParser
import os.path
import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
from face_sdk_3divi import FacerecService  # creates service
from face_sdk_3divi.modules.raw_image import RawImage, Format


def help_message():
    message = f"\n This program is an example of the Liveness Estimator Pad Processing Block integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] "
    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description='Processing Block Example')
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    return parser.parse_args()


def draw_bbox(rect, img, color=(0, 255, 0)):  # an example of a bbox drawing with opencv
    return cv2.rectangle(img, (int(rect[0].get_value() * img.shape[1]), int(rect[1].get_value() * img.shape[0])),
                         (int(rect[2].get_value() * img.shape[1]), int(rect[3].get_value() * img.shape[0])), color, 2)


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
        "modification": "v4"
    }
    liveness_block = service.create_processing_block(liveness_config)  # create liveness estimation processing block
    capturer = service.create_capturer("common_capturer_refa_fda_a.xml")

    img: np.ndarray = cv2.imread(input_image, cv2.IMREAD_COLOR)  # read an image from a file
    image = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # convert an image in RGB for correct results
    imgCtx = {  # put an image in container
        "blob": image.tobytes(),
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [dim for dim in img.shape]
    }
    raw_image = RawImage(image.shape[1], image.shape[0], Format.FORMAT_RGB, image.tobytes())
    ioData = service.create_context(
        {"image": imgCtx, "objects": []})  # put imgCtx in container which will be passed to a liveness block
    samples = capturer.capture(raw_image)

    for sample in samples:
        ioData["objects"].push_back(sample.to_context())

    liveness_block(ioData)  # call a liveness block and pass a container with an image

    for obj in ioData["objects"]:  # iteration over objects in ioData container
        print(
            f"Confidence: {obj['liveness']['confidence'].get_value()}, result: {obj['liveness']['value'].get_value()}"
        )  # print results in console
        picture = draw_bbox(obj["bbox"], img)  # visualize bboxes on an image

    cv2.imshow("result", picture)  # an example of a result image visualizing with opencv
    cv2.waitKey()  # wait for a key to be pressed to close the window
    cv2.destroyAllWindows()  # close the window


if __name__ == "__main__":
    help_message()
    args = parse_args()

    liveness_estimator(args.input_image, args.sdk_path)
