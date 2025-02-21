import argparse
import sys
import xml.etree.ElementTree as ET
import json


def cvat_converter(args):
    cvat_annotation_path = args.cvat_annotation_path
    output_path = args.output_path

    tree = ET.parse(cvat_annotation_path)
    root = tree.getroot()

    result = {}
    for elem in root:
        if elem.tag != "image":
            continue
        attributes = elem.attrib
        width = float(attributes.get("width"))
        height = float(attributes.get("height"))
        image_name:str = attributes.get("name")
        objects = []
        for id, bbox in enumerate(elem):
            bbox_attributes = bbox.attrib
            label = bbox_attributes.get("label")
            xtl = float(bbox_attributes.get("xtl"))
            ytl = float(bbox_attributes.get("ytl"))
            xbr = float(bbox_attributes.get("xbr"))
            ybr = float(bbox_attributes.get("ybr"))
            obj = {
                "id": id,
                "class": label,
                "bbox": [xtl / width, ytl / height, xbr / width, ybr / height]
            }
            objects.append(obj)
        result[image_name] = {"objects": objects}
    with open(output_path, 'w') as f:
        json.dump(result, f)


def main(args):  # launch parameters
    parser = argparse.ArgumentParser(description='CVAT Converter')
    parser.add_argument('--cvat_annotation_path', type=str, required=True)
    parser.add_argument('--output_json_path', type=str, required=True)
    parser.set_defaults(func=cvat_converter)

    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args(args)
    args.func(args)

if __name__ == '__main__':
    main(sys.argv[1:])