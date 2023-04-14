import argparse  # for an ArgumentParser
import os.path
import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
from face_sdk_3divi import FacerecService  # creates service

bone_map = [  # the list of key points and their connection
    ("right_ankle", "right_knee"),
    ("right_knee", "right_hip"),
    ("left_hip", "right_hip"),
    ("left_shoulder", "left_hip"),
    ("right_shoulder", "right_hip"),
    ("left_shoulder", "right_shoulder"),
    ("left_shoulder", "left_elbow"),
    ("right_shoulder", "right_elbow"),
    ("left_elbow", "left_wrist"),
    ("right_elbow", "right_wrist"),
    ("left_eye", "right_eye"),
    ("nose", "left_eye"),
    ("left_knee", "left_hip"),
    ("right_ear", "right_shoulder"),
    ("left_ear", "left_shoulder"),
    ("right_eye", "right_ear"),
    ("left_eye", "left_ear"),
    ("nose", "right_eye"),
    ("left_ankle", "left_knee"),
]


def help_message():
    message = f"\n This program is an example of the Human Pose Estimator Processing block integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] " \
              " [--use_cuda <--use_cuda>]"
    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description='Processing Block Example')
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    parser.add_argument('--use_cuda', action='store_true')  # pass the '--use_cuda' parameter before launch to use cuda acceleration
    return parser.parse_args()


def pose_estimator(input_image, sdk_path, use_cuda):
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

    detector_config = {  # detector block configuration parameters
        "unit_type": "HUMAN_BODY_DETECTOR",  # required parameter
        "model_path": sdk_path + "/share/bodydetectors/body.enc",  # required
        "confidence_threshold": 0.5,  # optional
        "iou_threshold": 0.5,  # optional
        "use_cuda": use_cuda,  # optional
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
    }

    estimator_config = {  # estimator block configuration parameters
        "unit_type": "HUMAN_POSE_ESTIMATOR",  # required parameter
        "model_path": sdk_path + "/share/humanpose/hpe-td.enc",  # required
        "label_map": sdk_path + "/share/humanpose/label_map_keypoints.txt",  # required
        "use_cuda": use_cuda,  # optional
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
    }

    detector_block = service.create_processing_block(detector_config)  # create a detector processing block
    estimator_block = service.create_processing_block(estimator_config)  # create an estimator processing block

    img: np.ndarray = cv2.imread(input_image, cv2.IMREAD_COLOR)  # read an image from a file
    image: np.ndarray = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # convert an image before passing it to a detector

    imgCtx = {  # put an image in a container
        "blob": image.tobytes(),
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [dim for dim in image.shape]
    }

    ioData = service.create_context({"image": imgCtx})  # create context and put imgCtx in container which will be passed to a detector block
    detector_block(ioData)  # call a detector block and pass a container with an image
    estimator_block(ioData)  # call a detector block and pass a container with an image

    for obj in ioData["objects"]:  # iteration over objects in ioData container
        img_w = img.shape[1]
        img_h = img.shape[0]

        rect = [coord.get_value() for coord in obj["bbox"]]  # get bbox coordinates from the ioData container
        x = int(rect[0] * img_w)
        y = int(rect[1] * img_h)
        width = int(rect[2] * img_w) - x
        height = int(rect[3] * img_h) - y
        cv2.rectangle(img, (x, y), (x + width, y + height), (0, 255, 0), 1)  # draw a bounding box

        for key in obj["keypoints"].keys():  # get key points coordinates from the ioData container
            point_x = int(obj["keypoints"][key]["proj"][0].get_value() * img_w)
            point_y = int(obj["keypoints"][key]["proj"][1].get_value() * img_h)
            cv2.circle(img, (point_x, point_y), 2, (0, 0, 255), 2)  # draw a keypoints

        for bone in bone_map:  # get bones coordinates from the ioData container
            point_x1 = int(obj["keypoints"][bone[0]]["proj"][0].get_value() * img_w)
            point_y1 = int(obj["keypoints"][bone[0]]["proj"][1].get_value() * img_h)

            point_x2 = int(obj["keypoints"][bone[1]]["proj"][0].get_value() * img_w)
            point_y2 = int(obj["keypoints"][bone[1]]["proj"][1].get_value() * img_h)

            cv2.line(img, (point_x1, point_y1), (point_x2, point_y2), (255, 255, 0), 1,
                     cv2.LINE_4)  # draw a skeleton lines

    cv2.imshow("result", img)  # an example of a result image visualizing with opencv
    cv2.waitKey(0)  # wait for a key to be pressed to close the window
    cv2.destroyAllWindows()  # close the window


if __name__ == "__main__":
    help_message()
    args = parse_args()

    pose_estimator(args.input_image, args.sdk_path, args.use_cuda)
