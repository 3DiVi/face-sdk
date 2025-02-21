import argparse
import os.path
import sys
import csv
import time
from typing import Tuple, List

import cv2
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from tqdm import tqdm

from face_sdk_3divi import FacerecService

from utils import open_json_config, get_file_paths_pattern, get_face_sdk_3divi_library_path


def get_apcer_bpcer(df, key="verdict", type_attack="all"):
    verdicts = df[key].to_numpy()
    real_verdict = df["real_verdict"].to_numpy()
    bpcer = np.sum(~verdicts & real_verdict) / np.sum(real_verdict)

    if type_attack != "all":
        df_attack = df[df["dataset"] == type_attack]
        real_verdict = df_attack["real_verdict"].to_numpy()
        verdicts = df_attack[key].to_numpy()

    apcer = np.sum(verdicts & (~real_verdict)) / np.sum(~real_verdict)

    return apcer, bpcer

def get_apcer_bpcer_with_threshold(df, threshold, type_attack="all"):
    df['verdict_on_threshold'] = df["confidence"] >= threshold
    return get_apcer_bpcer(df, "verdict_on_threshold", type_attack=type_attack)

def get_array_apcer_bpcer(df, n=101, type_attack="all"):
    array_apcer = np.zeros(n)
    array_bpcer = np.zeros(n)
    dip = np.linspace(0, 1, n)

    for i, th in enumerate(dip):
        array_apcer[i], array_bpcer[i] = get_apcer_bpcer_with_threshold(df, th, type_attack=type_attack)
    return array_apcer, array_bpcer


def test_on_dataset(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    liveness_configuration = args.liveness_configuration
    dataset_path = args.dataset_path
    draw_chart = args.draw_chart
    total_processing_time = 0.0
    total_images = 0
    skipped = 0

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    face_detector = service.create_processing_block(open_json_config(detector_configuration))

    face_fitter = service.create_processing_block({
        "unit_type": "FACE_FITTER",
        "modification": "fda"
    })
    liveness_estimator = service.create_processing_block(open_json_config(liveness_configuration))

    datasets = ["2d-mask", "3d-mask", "photo", "real", "regions", "replay"]
    header = ["file", "dataset", "confidence", "verdict", "real_verdict"]

    result_metrics = {}
    with open(f"liveness_result.csv", "w", newline='') as file:
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
                    skipped += 1
                    continue
                face_fitter(io_data)

                start = time.time()
                liveness_estimator(io_data)
                total_processing_time += time.time() - start

                total_images += 1

                obj = io_data["objects"][0]
                liveness = obj["liveness"]
                confidence = liveness["confidence"].get_value()
                verdict = liveness["value"].get_value()
                line = [file_path, dataset, confidence, (verdict == "REAL"), (dataset == "real")]
                writer.writerow(line)

            result_metrics[dataset] = (TP, FN)

    df = pd.read_csv(f"liveness_result.csv")

    type_attacks = df[df["dataset"] != "real"]["dataset"].unique()
    total_apcer, total_bpcer = get_apcer_bpcer(df, type_attack="all")

    print(f"bpcer: {round(total_bpcer, 3)}")
    print(f"apcer:\n\ttotal: {round(total_apcer, 3)}")
    for type_attack in type_attacks:
        apcer, _ = get_apcer_bpcer(df, type_attack=type_attack)
        print(f"\t{type_attack}: {round(apcer, 3)}")
    print(f"Average processing time: {total_processing_time / total_images} seconds")
    print(f"Skipped: {skipped}")

    if draw_chart:
        n = 101
        confidence_points = np.linspace(0, 1, n)
        array_apcer, array_bpcer = get_array_apcer_bpcer(df, n, type_attack="all")

        plt.plot(confidence_points, array_apcer, color="red", label="APCER")
        plt.plot(confidence_points, array_bpcer, color="purple", label="BPCER")

        plt.xticks(np.arange(0, 1.1, 0.1))
        plt.yticks(np.arange(0, 1.1, 0.1))

        plt.xlabel("Confidence")
        plt.ylabel("BPCER/APCER")
        plt.title("APCER/BPCER")
        plt.legend()
        plt.grid(True)
        plt.savefig("liveness_chart.png")

        if plt.isinteractive():
            plt.show()


def draw_charts(args):
    result_path = args.result_path

    df = pd.read_csv(result_path)

    type_attacks = df[df["dataset"] != "real"]["dataset"].unique()
    total_apcer, total_bpcer = get_apcer_bpcer(df, type_attack="all")

    print(f"bpcer: {round(total_bpcer, 3)}")
    print(f"apcer:\n\ttotal: {round(total_apcer, 3)}")
    for type_attack in type_attacks:
        apcer, _ = get_apcer_bpcer(df, type_attack=type_attack)
        print(f"\t{type_attack}: {round(apcer, 3)}")

    n = 101
    confidence_points = np.linspace(0, 1, n)
    array_apcer, array_bpcer = get_array_apcer_bpcer(df, n, type_attack="all")

    plt.plot(confidence_points, array_apcer, color="red", label="APCER")
    plt.plot(confidence_points, array_bpcer, color="purple", label="BPCER")

    plt.xticks(np.arange(0, 1.1, 0.1))
    plt.yticks(np.arange(0, 1.1, 0.1))

    plt.xlabel("Confidence")
    plt.ylabel("BPCER/APCER")
    plt.title("APCER/BPCER")
    plt.legend()
    plt.grid(True)
    plt.savefig("liveness_chart.png")

    if plt.isinteractive():
        plt.show()


def test_on_image(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    liveness_configuration = args.liveness_configuration
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
    liveness_estimator = service.create_processing_block(open_json_config(liveness_configuration))

    image = cv2.imread(input_image)

    io_data = service.create_context_from_frame(image.tobytes(), image.shape[1], image.shape[0])

    face_detector(io_data)
    if len(io_data["objects"]) != 1:
        print(f"not one face in image {input_image}")
    else:
        face_fitter(io_data)
        liveness_estimator(io_data)
        liveness = io_data["objects"][0]["liveness"]
        print(f"verdict = {liveness['value'].get_value()}")
        print(f"confidence = {liveness['confidence'].get_value()}")


def main(args):  # launch parameters
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('--sdk_path', type=str, default="../")
    parent_parser.add_argument('--detector_configuration', type=str, default="detector_configurations/ssyv_light.json")
    parent_parser.add_argument('--liveness_configuration', type=str, default="liveness_configurations/2d_light.json")

    parser = argparse.ArgumentParser(description='Liveness Test')

    subparsers = parser.add_subparsers()

    parser_test_on_dataset = subparsers.add_parser('test_on_dataset', parents=[parent_parser])
    parser_test_on_dataset_draw_charts = subparsers.add_parser('draw_charts')
    parser_test_on_image = subparsers.add_parser('test_on_image', parents=[parent_parser])

    parser_test_on_dataset.add_argument('--dataset_path', type=str, required=True)
    parser_test_on_dataset.add_argument('--draw_chart', action="store_true")
    parser_test_on_dataset.set_defaults(func=test_on_dataset)

    parser_test_on_dataset_draw_charts.add_argument('--result_path', type=str, required=True)
    parser_test_on_dataset_draw_charts.set_defaults(func=draw_charts)

    parser_test_on_image.add_argument('--input_image', type=str, required=True)
    parser_test_on_image.set_defaults(func=test_on_image)

    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args(args)
    args.func(args)


if __name__ == '__main__':
    main(sys.argv[1:])
