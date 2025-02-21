import argparse
import os.path
import csv
import sys
import time
import cv2
from tqdm import tqdm
from face_sdk_3divi import FacerecService

from utils import open_json_config, get_face_sdk_3divi_library_path


def get_metric(data):
    return data[1] / (data[1] + data[0])


def test_on_dataset(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    age_configuration = args.age_configuration
    dataset_path = args.dataset_path

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    face_detector = service.create_processing_block(open_json_config(detector_configuration))
    face_fitter = service.create_processing_block({
        "unit_type": "FACE_FITTER",
        "modification": "fda"
    })
    age_estimator = service.create_processing_block(open_json_config(age_configuration))

    header = ["file", "real_age", "age"]
    age_sum = 0.0
    process_image = 0
    result_metrics = {
        "13-16": [0, 0, 0],
        "17-22": [0, 0],
        "8-12": [0, 0],
    }
    result8_12 = result_metrics["8-12"]
    result13_16 = result_metrics["13-16"]
    result17_22 = result_metrics["17-22"]
    total_processing_time = 0.0

    with open(f"age_result.csv", "w", newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)

        file_names = os.listdir(dataset_path)
        for file_name in tqdm(file_names):
            real_age = int(file_name.split('_')[0])
            file_path = f"{dataset_path}/{file_name}"

            img = cv2.imread(file_path)
            io_data = service.create_context_from_frame(img.tobytes(), img.shape[1], img.shape[0])
            face_detector(io_data)
            if len(io_data["objects"]) != 1:
                continue
            face_fitter(io_data)

            start = time.time()
            age_estimator(io_data)
            total_processing_time += time.time() - start

            obj = io_data["objects"][0]
            age = obj["age"].get_value()
            process_image += 1
            absolute_error_age = abs(real_age - age)

            if 13 <= real_age < 17:
                result13_16[2] += absolute_error_age
                if 13 <= age < 17:
                    result13_16[0] += 1
                else:
                    result13_16[1] += 1
            else:
                if 13 <= age < 17:
                    result8_12[0] += 1
                    result17_22[0] += 1
                else:
                    if 8 <= real_age < 13:
                        result8_12[1] += 1
                    elif 17 <= age < 23:
                        result17_22[1] += 1

            age_sum += absolute_error_age
            line = [file_path, real_age, age]
            writer.writerow(line)

    print(f"absolute_error_age: {age_sum / process_image}")
    print(f"13-16: MAE={result13_16[2] / (result13_16[0] + result13_16[1])}, TPR={get_metric(result13_16)}")
    print(f"8-12: FPR={get_metric(result8_12)}")
    print(f"17-22: FPR={get_metric(result17_22)}")
    print(f"Average processing time: {total_processing_time / process_image}")


def test_on_image(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    age_configuration = args.age_configuration
    input_image = args.input_image

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    face_detector = service.create_processing_block(open_json_config(detector_configuration))
    face_fitter = service.create_processing_block({
        "unit_type": "FACE_FITTER",
        "modification": "fda"
    })
    age_estimator = service.create_processing_block(open_json_config(age_configuration))

    image = cv2.imread(input_image)

    io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

    face_detector(io_data)
    face_fitter(io_data)
    age_estimator(io_data)
    for i, obj in enumerate(io_data["objects"]):
        age = obj["age"].get_value()
        print(f"{i} age = {age}")


def main(args):  # launch parameters
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('--sdk_path', type=str, default="../")
    parent_parser.add_argument('--detector_configuration', type=str, default="detector_configurations/ssyv_light.json")
    parent_parser.add_argument('--age_configuration', type=str, default="age_configurations/heavy.json")

    parser = argparse.ArgumentParser(description='Age Test')

    subparsers = parser.add_subparsers()

    parser_test_on_dataset = subparsers.add_parser('test_on_dataset', parents=[parent_parser])
    parser_test_on_image = subparsers.add_parser('test_on_image', parents=[parent_parser])

    parser_test_on_dataset.add_argument('--dataset_path', type=str, required=True)
    parser_test_on_dataset.set_defaults(func=test_on_dataset)

    parser_test_on_image.add_argument('--input_image', type=str, required=True)
    parser_test_on_image.set_defaults(func=test_on_image)

    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args(args)
    args.func(args)


if __name__ == '__main__':
    main(sys.argv[1:])
