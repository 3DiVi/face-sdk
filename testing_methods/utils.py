import json
import os
from pathlib import Path
from sys import platform

import numpy as np


class BoxUtils:
    @staticmethod
    def compute_iou(box, boxes, box_area, boxes_area):
        if len(boxes) == 0:
            return np.array([0], dtype=np.float64)

        # Calculate intersection areas
        x1 = np.maximum(box[0], boxes[:, 0])
        y1 = np.maximum(box[1], boxes[:, 1])
        x2 = np.minimum(box[2], boxes[:, 2])
        y2 = np.minimum(box[3], boxes[:, 3])
        intersection = np.maximum(x2 - x1, 0) * np.maximum(y2 - y1, 0)
        union = box_area + boxes_area[:] - intersection[:]
        iou = intersection / union
        return iou

    @staticmethod
    def get_area(box):
        return (box[2] - box[0]) * (box[3] - box[1])

    @staticmethod
    def get_areas(boxes):
        return (boxes[:, 2] - boxes[:, 0]) * (boxes[:, 3] - boxes[:, 1])

    @staticmethod
    def matching_search(iou: np.array, iou_threshold, predicted_data, gt_data):
        iou_t = np.transpose(iou)
        matching_pd_index = []
        matching_gt_index = []
        for i, row in enumerate(iou_t):
            max_iou = 0
            index_pd = 0
            for j in range(len(row)):
                if row[j] > max_iou and predicted_data[i]["class"] == gt_data[j]["class"]:
                    max_iou = row[j]
                    index_pd = j

            if (max_iou > iou_threshold) and (index_pd not in matching_pd_index):
                matching_gt_index.append(i)
                matching_pd_index.append(index_pd)

        return tuple(zip(matching_gt_index, matching_pd_index))

    @staticmethod
    def find_matching(predicts, gt_objects, iou_threshold):
        num_predicts = len(predicts)
        num_gt_objects = len(gt_objects)

        if num_predicts == 0:
            return [], [], gt_objects
        if num_gt_objects == 0:
            return [], predicts, []

        gt_boxes = np.array([o["bbox"] for o in gt_objects], dtype=np.float32)
        predict_boxes = np.array([o["bbox"] for o in predicts], dtype=np.float32)

        predicted_box_areas = BoxUtils.get_areas(predict_boxes)
        iou = np.ndarray([num_gt_objects, num_predicts], dtype=np.float32)
        # Compute iou for each box
        for gt_index, gt_box in enumerate(gt_boxes):
            gt_box_area = BoxUtils.get_area(gt_box)
            iou[gt_index] = BoxUtils.compute_iou(gt_box, predict_boxes, gt_box_area, predicted_box_areas)

        matched_indices = BoxUtils.matching_search(iou, iou_threshold, predicts, gt_objects)
        # matched_indices = matching_method(iou, iou_threshold, predicts, gt_objects)

        is_gt_unmatched = np.ones([num_gt_objects], dtype=np.bool_)
        is_predict_unmatched = np.ones([num_predicts], dtype=np.bool_)

        for pred_index, gt_index in matched_indices:
            is_gt_unmatched[gt_index] = False
            is_predict_unmatched[pred_index] = False

        unmatched_gt_indices = np.arange(num_gt_objects)[is_gt_unmatched]
        unmatched_predict_indices = np.arange(num_predicts)[is_predict_unmatched]

        matching = [(predicts[i], gt_objects[j]) for i, j in matched_indices]
        unmatched_predicts = [predicts[i] for i in unmatched_predict_indices]
        unmatched_gt_objects = [gt_objects[i] for i in unmatched_gt_indices]
        return matching, unmatched_predicts, unmatched_gt_objects

def check_file_exist(path, info):
    if not os.path.exists(path):
        raise Exception(f"{info}: not exist file {path}")

def open_json_config(path) -> dict:
    with open(path, 'r') as file:
        data = json.load(file)
    return data

def get_file_paths_pattern(directory, exts):
    return [str(path) for path in Path(directory).rglob("*") if path.suffix in exts]

def get_face_sdk_3divi_library_path() -> str:
    return "bin/facerec.dll" if platform == "win32" else "lib/libfacerec.so"