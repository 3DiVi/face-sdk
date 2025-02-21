import argparse
import os.path
import sys
import csv
import time
import cv2
from face_sdk_3divi import FacerecService
from tqdm import tqdm

from utils import open_json_config, get_file_paths_pattern, get_face_sdk_3divi_library_path


def get_metric(data):
    return data[1] / (data[1] + data[0])


def test_on_dataset(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    mask_configuration = args.mask_configuration
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
    mask_estimator = service.create_processing_block(open_json_config(mask_configuration))

    datasets = ["with_mask", "no_mask"]
    header = ["file", "dataset", "confidence"]

    result_metrics = {}
    total_processing_time = 0.0
    process_image = 0

    with open(f"mask_result.csv", "w", newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)

        for dataset in datasets:
            current_dataset_path = f"{dataset_path}/{dataset}"
            if not os.path.isdir(current_dataset_path):
                continue
            files_path = get_file_paths_pattern(current_dataset_path, [".jpeg", ".jpg", ".png", ".bmp"])

            TP = 0
            FN = 0
            for file_path in tqdm(files_path, desc=dataset):
                img = cv2.imread(file_path)
                io_data = service.create_context_from_frame(img.tobytes(), img.shape[1], img.shape[0])
                face_detector(io_data)
                if len(io_data["objects"]) != 1:
                    continue
                face_fitter(io_data)

                start = time.time()
                mask_estimator(io_data)
                total_processing_time += time.time() - start

                process_image += 1

                obj = io_data["objects"][0]
                mask = obj["has_medical_mask"]
                confidence = mask["confidence"].get_value()
                verdict = mask["value"].get_value()

                if dataset == "with_mask":
                    TP += (verdict == True)
                    FN += (verdict == False)
                else:
                    TP += (verdict == False)
                    FN += (verdict == True)
                line = [file_path, dataset, confidence]
                writer.writerow(line)

            result_metrics[dataset] = (TP, FN)

    TP = 0
    FN = 0
    for key in result_metrics:
        print(f"{key} error: {get_metric(result_metrics[key])}")
        TP = + result_metrics[key][0]
        FN = + result_metrics[key][1]

    print(f"Accuracy: {1 - get_metric((TP, FN))}")
    print(f"Average processing time: {total_processing_time / process_image}")


def test_on_image(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    mask_configuration = args.mask_configuration
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
    mask_estimator = service.create_processing_block(open_json_config(mask_configuration))

    image = cv2.imread(input_image)

    io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

    face_detector(io_data)
    face_fitter(io_data)
    mask_estimator(io_data)
    for i, obj in enumerate(io_data["objects"]):
        mask = obj["has_medical_mask"]
        print(f"{i} verdict={mask['value'].get_value()}, confidence={mask['confidence'].get_value()}")


def main(args):  # launch parameters
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('--sdk_path', type=str, default="../")
    parent_parser.add_argument('--detector_configuration', type=str, default="detector_configurations/ssyv_light.json")
    parent_parser.add_argument('--mask_configuration', type=str, default="mask_configurations/light.json")

    parser = argparse.ArgumentParser(description='Mask Test')

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
