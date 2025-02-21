import argparse
import json
import os
import sys
import csv
import time
from typing import List, Tuple

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import cv2
from face_sdk_3divi.modules.context_template import ContextTemplate
from tqdm import tqdm
from face_sdk_3divi import FacerecService

from utils import get_face_sdk_3divi_library_path


def open_json_config(path):
    with open(path, 'r') as file:
        data = json.load(file)
    return data


def generate_uuids(template_labels: List[Tuple[int, int]]) -> List[str]:
    uuids = []

    for person_id, cur_person_id in template_labels:
        uuids.append(f"{person_id}_{cur_person_id}")

    return uuids

def get_far_frr(df, th=0.85):
    result = df["score"].to_numpy() > th
    real_verdict = df["gt"].to_numpy()
    frr = np.sum(~result & real_verdict) / np.sum(real_verdict)
    far = np.sum(result & (~real_verdict)) / np.sum(~real_verdict)
    return far, frr

def get_points_far_frr(df, n=101):
    points_far = np.zeros(n)
    points_frr = np.zeros(n)
    confidence_points = np.linspace(0, 1, n)

    for i, th in enumerate(confidence_points):
        points_far[i], points_frr[i] = get_far_frr(df, th)

    return points_far, points_frr

def template_extract(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    modification = args.modification
    dataset_path = args.dataset_path
    output_path = args.output_path
    dataset_configuration = args.dataset_configuration

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    face_detector = service.create_processing_block(open_json_config(detector_configuration))
    face_fitter = service.create_processing_block({
        "unit_type": "FACE_FITTER",
        "modification": "fda"
    })
    template_extractor = service.create_processing_block({
        "unit_type": "FACE_TEMPLATE_EXTRACTOR",
        "modification": modification
    })

    with open(dataset_configuration, "r") as file:
        file_names = [line.split("\n")[0] for line in file]

    person_id = -1
    cur_i = 0
    last_person = f"{dataset_path}"
    all_image_process = 0
    skipped = 0
    total_processing_time = 0.0

    with open(f"{output_path}", "wb") as file:
        for file_name in tqdm(file_names):
            path_to_image = f"{dataset_path}/{file_name}"
            cur_person, _ = file_name.split("/")
            if cur_person != last_person:
                cur_i = 0
                person_id += 1
                last_person = cur_person

            img = cv2.imread(path_to_image)

            io_data = service.create_context_from_frame(img.tobytes(), img.shape[1], img.shape[0])
            face_detector(io_data)
            if len(io_data["objects"]) != 1:
                skipped += 1
                continue
            face_fitter(io_data)

            start = time.time()
            template_extractor(io_data)
            total_processing_time += time.time() - start

            template_ctx: ContextTemplate = io_data["objects"][0]["face_template"]["template"].get_value()

            file.write(person_id.to_bytes(4, byteorder="big"))
            file.write(cur_i.to_bytes(4, byteorder="big"))
            template_ctx.save(file)

            cur_i += 1
            all_image_process += 1
        print(f"{all_image_process} process from {len(file_names)}")
        print(f"Skipped: {skipped}")
        print(f"Average processing time: {total_processing_time / all_image_process}")


def verification_test_on_dataset(args):
    sdk_path = args.sdk_path
    modification = args.modification
    templates_path = args.templates_path
    draw_chart = args.draw_chart

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    verification_module = service.create_processing_block({
        "unit_type": "VERIFICATION_MODULE",
        "modification": modification
    })

    templates = []
    template_labels = []
    TP = 0
    FP = 0
    TN = 0
    FN = 0
    score_threshold = 0.85
    total_processing_time = 0.0
    total_comparisons = 0

    with open(templates_path, "rb") as file:
        while True:
            temp_data = file.read(8)
            if not temp_data:
                break
            template_labels.append((int.from_bytes(temp_data[:4], "big"), int.from_bytes(temp_data[4:8], "big")))
            templates.append(service.load_context_template(file))

    templates_count = len(templates)
    verification_data = service.create_context({})

    header = ["class1", "class2", "label1", "label2", "score", "distance"]
    with open(f"verification_{modification}_result.csv", "w", newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)
        for i in tqdm(range(templates_count)):
            person_id1, cur_person_id1 = template_labels[i]
            verification_data["template1"]["template"] = templates[i]

            for j in range(i + 1, templates_count):
                verification_data["template2"]["template"] = templates[j]
                person_id2, cur_person_id2 = template_labels[j]

                start = time.time()
                verification_module(verification_data)
                total_processing_time += time.time() - start

                total_comparisons += 1

                result = verification_data["result"]
                score = result["score"].get_double()
                distance = result["distance"].get_double()

                TP += (person_id1 == person_id2) and (score >= score_threshold)
                FP += (person_id1 != person_id2) and (score >= score_threshold)
                TN += (person_id1 != person_id2) and (score < score_threshold)
                FN += (person_id1 == person_id2) and (score < score_threshold)

                line = [person_id1, person_id2, cur_person_id1, cur_person_id2, score, distance]
                writer.writerow(line)
    far = FP / (FP + TN)
    frr = FN / (TP + FN)
    print(f"far: {far}")
    print(f"frr: {frr}")
    print(f"Total comparisons: {total_comparisons}")
    print(f"Average processing time: {total_processing_time / total_comparisons}")

    if draw_chart:
        df = pd.read_csv(f"verification_{modification}_result.csv")
        df["gt"] = df["class1"] == df["class2"]

        n = 101
        confidence_points = np.linspace(0, 1, n)
        far_points, frr_points = get_points_far_frr(df, n)
        plt.plot(confidence_points, far_points, color="red", label="FAR")
        plt.plot(confidence_points, frr_points, color="purple", label="FRR")

        plt.xticks(np.arange(0, 1.1, 0.1))
        plt.yticks(np.arange(0, 1.1, 0.1))
        plt.yscale('log')
        plt.xlabel("Confidence")
        plt.ylabel("FAR/FRR")
        plt.title("FAR/FRR")
        plt.legend()
        plt.grid(True)
        plt.savefig("verification_chart.png")

        if plt.isinteractive():
            plt.show()



def draw_charts(args):
    result_path = args.result_path
    output_path  = args.output_path

    df = pd.read_csv(result_path)
    df["gt"] = df["class1"] == df["class2"]

    n = 101
    confidence_points = np.linspace(0, 1, n)
    far_points, frr_points = get_points_far_frr(df, n)
    plt.plot(confidence_points, far_points, color="red", label="FAR")
    plt.plot(confidence_points, frr_points, color="purple", label="FRR")

    plt.xticks(np.arange(0, 1.1, 0.1))
    plt.yticks(np.arange(0, 1.1, 0.1))
    plt.yscale('log')
    plt.xlabel("Confidence")
    plt.ylabel("FAR/FRR")
    plt.title("FAR/FRR")
    plt.legend()
    plt.grid(True)
    plt.savefig(output_path)

    if plt.isinteractive():
        plt.show()


def identification_test_on_dataset(args):
    sdk_path = args.sdk_path
    modification = args.modification
    templates_path = args.templates_path
    draw_chart = args.draw_chart
    knn = args.knn

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    matcher_module = service.create_processing_block({
        "unit_type": "MATCHER_MODULE",
        "modification": modification
    })

    templates = []
    template_labels = []
    query_templates = []
    query_template_labels = []
    TP = 0
    FP = 0
    TN = 0
    FN = 0
    score_threshold = 0.85
    total_processing_time = 0.0
    total_comparisons = 0

    with open(templates_path, "rb") as file:
        last_person_id = -1
        while True:
            temp_data = file.read(8)
            if not temp_data:
                break
            labels = (int.from_bytes(temp_data[:4], "big"), int.from_bytes(temp_data[4:8], "big"))
            if labels[0] != last_person_id:
                template_labels.append(labels)
                templates.append(service.load_context_template(file))
                last_person_id = labels[0]
            else:
                query_template_labels.append(labels)
                query_templates.append(service.load_context_template(file))

    validation_templates_count = len(query_templates)
    index = service.create_dynamic_template_index(templates, generate_uuids(template_labels), False)
    matcher_data = service.create_context({
        "template_index": index,
        "knn": knn
    })

    header = ["class1", "class2", "label1", "label2", "score", "distance"]
    with open(f"identification_{modification}_result.csv", "w", newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)
        for i in tqdm(range(validation_templates_count)):
            person_id1, cur_person_id1 = query_template_labels[i]

            matcher_data["queries"] = query_templates[i]

            start = time.time()
            matcher_module(matcher_data)
            total_processing_time += time.time() - start

            total_comparisons += 1

            results = matcher_data["results"]

            for result in results:
                score = result["score"].get_value()
                uuid: str = result["uuid"].get_value()
                distance = result["distance"].get_value()

                person_id2, cur_person_id2 = map(int, uuid.split('_'))

                TP += (person_id1 == person_id2) and (score >= score_threshold)
                FP += (person_id1 != person_id2) and (score >= score_threshold)
                TN += (person_id1 != person_id2) and (score < score_threshold)
                FN += (person_id1 == person_id2) and (score < score_threshold)

                line = [person_id1, person_id2, cur_person_id1, cur_person_id2, score, distance]
                writer.writerow(line)

            matcher_data["result"].clear()

    fpir = FP / (FP + TN)
    fnir = FN / (TP + FN)
    print(f"fpir: {fpir}")
    print(f"fnir: {fnir}")
    print(f"Total comparisons: {total_comparisons}")
    print(f"Average processing time: {total_processing_time / total_comparisons}")

    if draw_chart:
        df = pd.read_csv(f"identification_{modification}_result.csv")
        df["gt"] = df["class1"] == df["class2"]

        n = 101
        confidence_points = np.linspace(0, 1, n)
        far_points, frr_points = get_points_far_frr(df, n)
        plt.plot(confidence_points, far_points, color="red", label="FAR")
        plt.plot(confidence_points, frr_points, color="purple", label="FRR")

        plt.xticks(np.arange(0, 1.1, 0.1))
        plt.yticks(np.arange(0, 1.1, 0.1))
        plt.yscale('log')
        plt.xlabel("Confidence")
        plt.ylabel("FAR/FRR")
        plt.title("FAR/FRR")
        plt.legend()
        plt.grid(True)
        plt.savefig("identification_chart.png")

        if plt.isinteractive():
            plt.show()


def test_on_image(args):
    sdk_path = args.sdk_path
    detector_configuration = args.detector_configuration
    modification = args.modification
    input_image1 = args.input_image1
    input_image2 = args.input_image2

    fsdk_dll_path = os.path.join(sdk_path, get_face_sdk_3divi_library_path())
    fsdk_conf_dir_path = os.path.join(sdk_path, 'conf', 'facerec')
    fsdk_license_path = os.path.join(sdk_path, 'license')
    service = FacerecService.create_service(fsdk_dll_path, fsdk_conf_dir_path, fsdk_license_path)
    face_detector = service.create_processing_block(open_json_config(detector_configuration))
    face_fitter = service.create_processing_block({
        "unit_type": "FACE_FITTER",
        "modification": "fda"
    })
    template_extractor = service.create_processing_block({
        "unit_type": "FACE_TEMPLATE_EXTRACTOR",
        "modification": modification
    })
    verification_module = service.create_processing_block({
        "unit_type": "VERIFICATION_MODULE",
        "modification": modification
    })

    image1 = cv2.imread(input_image1)
    image2 = cv2.imread(input_image2)

    io_data1 = service.create_context_from_frame(image1.tobytes(), image1.shape[1], image1.shape[0])
    io_data2 = service.create_context_from_frame(image2.tobytes(), image2.shape[1], image2.shape[0])

    face_detector(io_data1)
    face_fitter(io_data1)
    template_extractor(io_data1)
    face_detector(io_data2)
    face_fitter(io_data2)
    template_extractor(io_data2)

    if len(io_data1["objects"]) != 1:
        raise Exception(f"not one face in image {input_image1}")

    if len(io_data2["objects"]) != 1:
        raise Exception(f"not one face in image {input_image2}")

    verification_data = service.create_context({})
    verification_data["template1"] = io_data1["objects"][0]["face_template"]
    verification_data["template2"] = io_data2["objects"][0]["face_template"]

    verification_module(verification_data)
    result = verification_data["result"]
    print(f"score: {result['score'].get_value()}")
    print(f"distance: {result['distance'].get_value()}")
    print(f"far: {result['far'].get_value()}")
    print(f"frr: {result['frr'].get_value()}")


def main(args):
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument('--sdk_path', type=str, default="../")
    parent_parser.add_argument('--modification', type=str, default="1000")

    parser = argparse.ArgumentParser(description='Recognition Test')

    subparsers = parser.add_subparsers()

    parser_template_extract = subparsers.add_parser('template_extract', parents=[parent_parser])
    parser_verification_test_on_dataset = subparsers.add_parser('verification_test_on_dataset', parents=[parent_parser])
    parser_draw_charts = subparsers.add_parser('draw_charts')
    parser_identification_test_on_dataset = subparsers.add_parser('identification_test_on_dataset',
                                                                  parents=[parent_parser])

    parser_test_on_image = subparsers.add_parser('test_on_image', parents=[parent_parser])

    parser_template_extract.add_argument('--dataset_path', type=str, required=True)
    parser_template_extract.add_argument('--detector_configuration', type=str,
                                         default="detector_configurations/ssyv_light.json")
    parser_template_extract.add_argument('--output_path', type=str, required=True)
    parser_template_extract.add_argument('--dataset_configuration', type=str, required=True)
    parser_template_extract.set_defaults(func=template_extract)

    parser_verification_test_on_dataset.add_argument('--templates_path', type=str, required=True)
    parser_verification_test_on_dataset.add_argument('--draw_chart', action="store_true")
    parser_verification_test_on_dataset.set_defaults(func=verification_test_on_dataset)

    parser_draw_charts.add_argument('--result_path', type=str, required=True)
    parser_draw_charts.add_argument('--output_path', type=str, required=True)
    parser_draw_charts.set_defaults(func=draw_charts)

    parser_identification_test_on_dataset.add_argument('--templates_path', type=str, required=True)
    parser_identification_test_on_dataset.add_argument('--knn', type=int, required=True)
    parser_identification_test_on_dataset.add_argument('--draw_chart', action="store_true")
    parser_identification_test_on_dataset.set_defaults(func=identification_test_on_dataset)

    parser_test_on_image.add_argument('--detector_configuration', type=str,
                                      default="detector_configurations/ssyv_light.json")
    parser_test_on_image.add_argument('--input_image1', type=str, required=True)
    parser_test_on_image.add_argument('--input_image2', type=str, required=True)
    parser_test_on_image.set_defaults(func=test_on_image)

    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args(args)
    args.func(args)


if __name__ == '__main__':
    main(sys.argv[1:])
