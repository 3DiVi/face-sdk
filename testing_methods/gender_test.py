import argparse
import os.path
import sys
import time
import cv2
from face_sdk_3divi import FacerecService
from tqdm import tqdm

from utils import open_json_config, get_face_sdk_3divi_library_path


def test_on_dataset(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    gender_configuration = args.gender_configuration
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
    gender_estimator = service.create_processing_block(open_json_config(gender_configuration))

    correct_gender = 0
    correct_gender_male = 0
    correct_gender_female = 0
    wrong_gender_male = 0
    wrong_gender_female = 0
    process_image = 0
    file_names = os.listdir(dataset_path)
    total_processing_time = 0.0

    for file_name in tqdm(file_names):
        label = int(file_name.split('_')[1])
        file_path = f"{dataset_path}/{file_name}"

        img = cv2.imread(file_path)
        io_data = service.create_context_from_frame(img.tobytes(), img.shape[1], img.shape[0])
        face_detector(io_data)
        if len(io_data["objects"]) != 1:
            continue
        face_fitter(io_data)

        start = time.time()
        gender_estimator(io_data)
        total_processing_time += time.time() - start

        obj = io_data["objects"][0]
        gender = (obj["gender"].get_value() == "FEMALE")
        if label == gender:
            correct_gender += 1
            correct_gender_male += (gender == 0)
            correct_gender_female += (gender == 1)
        else:
            wrong_gender_male += (gender == 0)
            wrong_gender_female += (gender == 1)

        process_image += 1
    print(f"accuracy gender = {correct_gender / process_image}")
    print(f"female error = {wrong_gender_female / (correct_gender_female + wrong_gender_female)}")
    print(f"male error = {wrong_gender_male / (correct_gender_male + wrong_gender_male)}")
    print(f"Average processing time: {total_processing_time / process_image}")


def test_on_image(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    gender_configuration = args.gender_configuration
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
    gender_estimator = service.create_processing_block(open_json_config(gender_configuration))

    image = cv2.imread(input_image)

    io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

    face_detector(io_data)
    face_fitter(io_data)
    gender_estimator(io_data)
    for i, obj in enumerate(io_data["objects"]):
        age = obj["gender"].get_value()
        print(f"{i} gender = {age}")


def main(args):  # launch parameters
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('--sdk_path', type=str, default="../")
    parent_parser.add_argument('--detector_configuration', type=str, default="detector_configurations/ssyv_light.json")
    parent_parser.add_argument('--gender_configuration', type=str, default="gender_configurations/heavy.json")

    parser = argparse.ArgumentParser(description='Gender Test')

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
