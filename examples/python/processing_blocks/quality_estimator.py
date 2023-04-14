import argparse  # for an ArgumentParser
import os.path
import cv2  # for reading images, visualising window and rectangle creation
import numpy as np

from sys import platform  # for a platform identification
from face_sdk_3divi import FacerecService, Config  # FacerecService creates service, Config creates capturer config


def help_message():
    message = f"\n This program is an example of the Quality Assessment Estimator Processing Block integration \n Usage: " \
              " [--input_image <image_path>] " \
              " [--sdk_path <sdk_root_dir>] \n"
    print(message)


def parse_args():  # launch parameters
    parser = argparse.ArgumentParser(description='Processing Block Example')
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    return parser.parse_args()


def draw_bbox(rect, img, color=(0, 255, 0)):  # an example of a bbox drawing with opencv
    return cv2.rectangle(img, (int(rect[0] * img.shape[1]), int(rect[1] * img.shape[0])),
                         (int(rect[2] * img.shape[1]), int(rect[3] * img.shape[0])), color, 2)


# the function gets the values which are required by the Quality Assessment Estimator Processing Block
# it uses the RawSample methods
def convert_raw_sample_to_context(sample, ctx, img_shape):
    frame = sample.get_rectangle()  # get a face bounding box
    ctx["bbox"] = [
        float(frame.x / img_shape[1]),
        float(frame.y / img_shape[0]),
        float((frame.x + frame.width) / img_shape[1]),
        float((frame.y + frame.height) / img_shape[0]),
    ]

    ctx["confidence"] = sample.get_score()  # get the detection confidence score
    ctx["class"] = "face"
    fitter_data = {}
    fitter_data["keypoints"] = []
    fitter_data["fitter_type"] = "fda"

    landmarks = sample.get_landmarks()  # get a face anthropometric points
    for pt in landmarks:
        fitter_data["keypoints"].append(pt.x)
        fitter_data["keypoints"].append(pt.y)
        fitter_data["keypoints"].append(pt.z)

        fitter_data["left_eye"] = [sample.get_left_eye().x, sample.get_left_eye().y]  # get a left eye coordinates
        fitter_data["right_eye"] = [sample.get_right_eye().x, sample.get_right_eye().y]  # get a right eye coordinates

    ctx["fitter"] = fitter_data
    ctx["angles"] = {}
    ctx["angles"]["yaw"] = sample.get_angles().yaw  # get a yaw angle
    ctx["angles"]["pitch"] = sample.get_angles().pitch  # get a pitch angle
    ctx["angles"]["roll"] = sample.get_angles().roll  # get a roll angle


def quality_estimator(input_image, sdk_path):
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
        "model_path": "",  # required
        "config_name": "quality_assessment.xml",  # optional
        "sdk_path": sdk_path,  # optional
        "facerec_conf_dir": sdk_conf_dir,  # optional
        "ONNXRuntime": {
            "library_path": sdk_onnx_path  # optional
        }
    }
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

    ioData = {"objects": []}  # creating container for output data

    for sample in samples:  # iteration over detected faces in ioData container
        estimatorCtx = {"image_ptr": image_ctx}  # put imgCtx in a container which will be passed to an estimator block
        convert_raw_sample_to_context(sample, estimatorCtx, img.shape)
        quality_block(estimatorCtx)  # call an estimator and pass a container with a cropped image
        ioData["objects"].append(estimatorCtx)  # add results to ioData container

    for obj in ioData["objects"]:  # iteration over objects in ioData container
        print("Quality score: ", obj["quality"]["qaa"]["totalScore"])  # print results in console
        print("Bbox coordinates: ", obj["bbox"])
        picture = draw_bbox(obj['bbox'], img)  # visualize bboxes on an image

    cv2.imshow("result", picture)  # an example of a result image visualizing with opencv
    cv2.waitKey(0)  # wait for a key to be pressed to close the window
    cv2.destroyAllWindows()  # close the window


if __name__ == "__main__":
    help_message()
    args = parse_args()

    quality_estimator(args.input_image, args.sdk_path)
