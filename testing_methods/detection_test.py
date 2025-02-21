import argparse
import json
import os
import sys
import time
from copy import deepcopy
from typing import Union

import cv2
from face_sdk_3divi import FacerecService
from tqdm import tqdm

from utils import BoxUtils, check_file_exist, open_json_config, get_file_paths_pattern, get_face_sdk_3divi_library_path


class Dataset:
    def __init__(self, image_folder, annotation_file):
        self._image_folder = image_folder
        self._images_info = []
        self.load_annotation(annotation_file)

    def __len__(self) -> int:
        return len(self._images_info)

    def __getitem__(self, index: Union[int, dict]) -> dict:
        sample = deepcopy(self._images_info[index])

        image_path = os.path.abspath(os.path.join(self._image_folder, sample['image_annotation']['file_name']))
        assert os.path.exists(image_path), "No image file found for {}".format(image_path)

        sample['id'] = image_path
        sample['path'] = []
        sample["exist_file"] = os.path.exists(image_path)
        if sample["exist_file"]:
            sample['image'] = cv2.imread(image_path, cv2.IMREAD_COLOR)

        return sample

    def load_annotation(self, annotation_file):
        with open(annotation_file) as json_file:
            json_data = json.load(json_file)

        for key, data in json_data.items():
            dataset_annotation = {
                "image_annotation": {"file_name": key},
                "objects": data["objects"]
            }
            self._images_info.append(dataset_annotation)


def test_with_annotation(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    annotation_path = args.annotation_path
    dataset_path = args.dataset_path
    total_processing_time = 0.0

    check_file_exist(detector_configuration, "detector_configuration")
    check_file_exist(annotation_path, "annotation_path")

    ds = Dataset(dataset_path, annotation_path)

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')

    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)

    config_detector = open_json_config(detector_configuration)
    face_detector = service.create_processing_block(config_detector)

    accumulated_tp = 0
    accumulated_fp = 0
    accumulated_fn = 0
    processed_images = 0

    for sample in tqdm(ds):
        if not sample["exist_file"]:
            continue

        image = sample["image"]

        io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

        start = time.time()
        face_detector(io_data)
        total_processing_time += time.time() - start

        cap_predict_obj = []

        for obj in io_data["objects"]:
            bbox = obj["bbox"].to_dict()
            cap_predict_obj.append({'bbox': bbox, 'class': 'face'})

        match_result = BoxUtils.find_matching(cap_predict_obj, sample["objects"], 0.3)
        TP, FP, FN = match_result

        accumulated_tp += len(TP)
        accumulated_fp += len(FP)
        accumulated_fn += len(FN)
        processed_images += 1

    precision = accumulated_tp / (accumulated_tp + accumulated_fp)
    recall = accumulated_tp / (accumulated_tp + accumulated_fn)
    f1_score = (2 * recall * precision) / (recall + precision)

    print(f"\n"
          f"Count of TP: {accumulated_tp}\n"
          f"Count of FP: {accumulated_fp}\n"
          f"Count of FN: {accumulated_fn}\n"
          f"Precision: {round(precision, 3)}\n"
          f"Recall: {round(recall, 3)}\n"
          f"F1_score: {round(f1_score, 3)}\n"
          f"Processed images: {processed_images}\n"
          f"Average processing time: {total_processing_time / processed_images} seconds\n")

def test_without_annotation(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    dataset_path = args.dataset_path
    total_processing_time = 0.0

    check_file_exist(detector_configuration, "detector_configuration")

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')

    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    config_detector = open_json_config(detector_configuration)
    face_detector = service.create_processing_block(config_detector)

    files_path = get_file_paths_pattern(dataset_path, [".jpeg", ".jpg", ".png", ".bmp"])
    total_images = len(files_path)
    total_detection_face = 0
    image_without_face = 0
    for file_path in tqdm(files_path):
        image = cv2.imread(file_path)

        io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

        start = time.time()
        face_detector(io_data)
        total_processing_time += time.time() - start

        count_face = len(io_data["objects"])
        total_detection_face += count_face
        if not count_face:
            image_without_face += 1
    print(f"total_images:\t{total_images}")
    print(f"total_detection_face:\t{total_detection_face}")
    print(f"image_without_face:\t{image_without_face}")
    print(f"Average processing time: {total_processing_time / total_images} seconds")

def test_on_image(args):
    sdk_path = args.sdk_path
    image_path = args.input_image
    detector_configuration = args.detector_configuration

    check_file_exist(detector_configuration, "detector_configuration")
    check_file_exist(image_path, "image_path")

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')

    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    config_detector = open_json_config(detector_configuration)
    face_detector = service.create_processing_block(config_detector)

    image = cv2.imread(image_path)
    io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

    face_detector(io_data)

    draw_image = image.copy()
    height, width, _ = image.shape
    for obj in io_data["objects"]:
        bbox = obj["bbox"].to_dict()
        draw_image = cv2.rectangle(draw_image, (int(bbox[0] * width), int(bbox[1] * height)),
                                   (int(bbox[2] * width), int(bbox[3] * height)), (0, 255, 0), 2, 1)

    cv2.imshow("detection_face", draw_image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def main(args):  # launch parameters
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('--sdk_path', type=str, default="../")
    parent_parser.add_argument('--detector_configuration', type=str, default="detector_configurations/ssyv_light.json")

    parser = argparse.ArgumentParser(description='Detectors Test')

    subparsers = parser.add_subparsers()

    parser_test_with_annotation = subparsers.add_parser('test_with_annotation', parents=[parent_parser])
    parser_test_without_annotation = subparsers.add_parser('test_without_annotation', parents=[parent_parser])
    parser_test_on_image= subparsers.add_parser('test_on_image', parents=[parent_parser])

    parser_test_with_annotation.add_argument('--dataset_path', type=str, required=True)
    parser_test_with_annotation.add_argument('--annotation_path', type=str, required=True)
    parser_test_with_annotation.set_defaults(func=test_with_annotation)

    parser_test_without_annotation.add_argument('--dataset_path', type=str, required=True)
    parser_test_without_annotation.set_defaults(func=test_without_annotation)

    parser_test_on_image.add_argument('--input_image', type=str, required=True)
    parser_test_on_image.set_defaults(func=test_on_image)

    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args(args)
    args.func(args)


if __name__ == "__main__":
    main(sys.argv[1:])

