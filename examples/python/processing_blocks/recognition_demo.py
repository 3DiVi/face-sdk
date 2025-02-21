import sys
import argparse
import os.path

import cv2
import numpy as np

from face_sdk_3divi import Service


def help_message():
    message = f"usage: {sys.argv[0]} [--mode verify | identify] " \
              " [--input_image <path to image>]" \
              " [--input_image2 <path to image>]" \
              " [--sdk_path <path to sdk>]" \
              " [--window <yes/no>]" \
              " [--output <yes/no>]" \
              " [--modification <30, 50, 100, 1000>]"
    print(message)


def get_crop(obj, image):
    img_w = image.shape[1]
    img_h = image.shape[0]

    rect_ctx = obj["bbox"]
    x = int(rect_ctx[0].get_value() * img_w)
    y = int(rect_ctx[1].get_value() * img_h)
    width = int(rect_ctx[2].get_value() * img_w) - x
    height = int(rect_ctx[3].get_value() * img_h) - y

    crop_image = image[max(0, y - int(height * 0.25)): min(img_h, y + int(height * 1.25)),
                 max(0, x - int(width * 0.25)): min(img_w, x + int(width * 1.25))]

    return crop_image


def draw_bbox(obj, img, output, color=(0, 255, 0)):
    rect = obj["bbox"]
    if output == "yes":
        print(
            f"BBox coordinates: {int(rect[0].get_value() * img.shape[1])}, {int(rect[1].get_value() * img.shape[0])}, {(int(rect[2].get_value() * img.shape[1]))}, {int(rect[3].get_value() * img.shape[0])}")
    return cv2.rectangle(img, (int(rect[0].get_value() * img.shape[1]), int(rect[1].get_value() * img.shape[0])),
                         (int(rect[2].get_value() * img.shape[1]), int(rect[3].get_value() * img.shape[0])), color, 2)


def get_faces(service, face_detector, face_fitter, img):
    input_image: np.ndarray = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    height, width, _ = input_image.shape

    imgCtx = {"blob": input_image.tobytes(), "dtype": "uint8_t", "format": "NDARRAY",
              "shape": [dim for dim in input_image.shape]}
    ioData = service.create_context({"image": imgCtx})

    face_detector(ioData)
    face_fitter(ioData)

    return ioData


def recognition_demo(sdk_path, img_path_1, img_path_2, window, output, mode, modification):
    service = Service.create_service(sdk_path)
    if not os.path.exists(img_path_1):
        raise Exception(f"not exist file {img_path_1}")
    if not os.path.exists(img_path_2):
        raise Exception(f"not exist file {img_path_2}")

    face_detector = service.create_processing_block(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv"})
    face_template_extractor = service.create_processing_block({"unit_type": "FACE_TEMPLATE_EXTRACTOR", "modification": modification})
    face_fitter = service.create_processing_block({"unit_type": "FACE_FITTER", "modification": "fda"})

    img1: np.ndarray = cv2.imread(img_path_1, cv2.IMREAD_COLOR)
    img2: np.ndarray = cv2.imread(img_path_2, cv2.IMREAD_COLOR)
    ioData1 = get_faces(service, face_detector, face_fitter, img1)
    ioData2 = get_faces(service, face_detector, face_fitter, img2)

    if not len(ioData1["objects"]):
        raise Exception(f"no face detected on {img_path_1} image")

    if not len(ioData2["objects"]):
        raise Exception(f"no face detected on {img_path_2} image")

    if mode == "verify" and len(ioData1["objects"]) != 1:
        raise Exception(f"many faces in verify mode on {img_path_1} image")

    if len(ioData2["objects"]) != 1:
        raise Exception(f"many faces on {img_path_2} image")

    face_template_extractor(ioData1)
    face_template_extractor(ioData2)

    if mode == "verify":
        verification_module = service.create_processing_block(
            {"unit_type": "VERIFICATION_MODULE", "modification": modification})

        verification_data = service.create_context({
            "template1": ioData1["objects"][0]["face_template"],
            "template2": ioData2["objects"][0]["face_template"]
        })

        verification_module(verification_data)

        distance = verification_data["result"]["distance"].get_value()
        score = verification_data["result"]["score"].get_value()

        color = (0, 255, 0) if score >= 0.85 else (0, 0, 255)
        img1 = draw_bbox(ioData1["objects"][0], img1, output, color)
        img2 = draw_bbox(ioData2["objects"][0], img2, output, color)

        crop1 = get_crop(ioData1["objects"][0], img1)
        crop2 = get_crop(ioData2["objects"][0], img2)

        crop1 = cv2.resize(crop1, (320, 480))
        crop2 = cv2.resize(crop2, (320, 480))

        print(f"distance = {distance}")
        print(f"score = {score}")
        print(f"verdict = {score >= 0.85}")

        if window == "yes":
            cv2.imshow("result", np.concatenate((crop1, crop2), axis=1))
            cv2.waitKey(0)
            cv2.destroyAllWindows()
    else:
        matcher_module = service.create_processing_block({"unit_type": "MATCHER_MODULE", "modification": modification})
        template_index = service.create_processing_block({"unit_type": "TEMPLATE_INDEX", "modification": modification})
        templates = service.create_context([])

        for object in ioData1["objects"]:
            templates.push_back(object["face_template"])

        ioData1["templates"] = templates

        template_index(ioData1)

        matcher_data = service.create_context(
            {"knn": 1, "template_index": ioData1["template_index"]})

        matcher_data["queries"] = ioData2["objects"][0]["face_template"]["template"]

        matcher_module(matcher_data)

        find_index = matcher_data["results"][0]["index"].get_value()
        distance = matcher_data["results"][0]["distance"].get_value()
        score = matcher_data["results"][0]["score"].get_value()

        print(f"index = {find_index}")
        print(f"distance = {distance}")
        print(f"score = {score}")
        print(f"verdict = {score >= 0.85}")

        for i in range(len(ioData1["objects"])):
            obj = ioData1["objects"][i]
            color = (0, 255, 0) if i == find_index and score >= 0.85 else (0, 0, 255)
            draw_bbox(obj, img1, output, color)

        if window == "yes":
            cv2.imshow("result", img1)
            cv2.waitKey(0)
            cv2.destroyAllWindows()


def parse_args():
    parser = argparse.ArgumentParser(description='Recognition Demo')
    parser.add_argument('--mode', default="verify", type=str)
    parser.add_argument('--input_image', type=str, required=True)
    parser.add_argument('--input_image2', type=str)
    parser.add_argument('--sdk_path', default="../../../", type=str)
    parser.add_argument('--window', default="yes", type=str)
    parser.add_argument('--output', default="no", type=str)
    parser.add_argument("--modification", default="1000", type=str)

    return parser.parse_args()


if __name__ == "__main__":

    help_message()

    args = parse_args()

    try:
        if args.mode != "verify" and args.mode != "identify":
            raise Exception("there is no modifier " + args.mode)
        recognition_demo(args.sdk_path, args.input_image, args.input_image2, args.window, args.output, args.mode,
                         args.modification)
    except Exception as e:
        print(f"Error: {e}.")
