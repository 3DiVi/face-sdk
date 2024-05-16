#!/bin/bash

# exit on error
set -e

if [ $# != 1 ]; then
	echo "Usage: ./isolates_demo <rootdir>";
	echo "Example: ./isolates_demo /home/andrew/dropbox/face_sdk_3divi_3divi/2.3.0";
	exit 1
fi

RESULT_DIR=$1;

echo RESULT_DIR:$RESULT_DIR

ASSETS="$RESULT_DIR/examples/flutter/isolates_demo/assets"
mkdir -p "$ASSETS/share/faceanalysis";
mkdir -p "$ASSETS/share/faceattributes";
mkdir -p "$ASSETS/share/facedetectors/blf";
mkdir -p "$ASSETS/share/face_quality";
mkdir -p "$ASSETS/share/fda";
mkdir -p "$ASSETS/share/processing_block/face_detector/blf_front";
mkdir -p "$ASSETS/share/processing_block/face_template_extractor/30/1";
mkdir -p "$ASSETS/share/processing_block/liveness_estimator/2d_light";
mkdir -p "$ASSETS/share/quality_iso";

cp -r "$RESULT_DIR/conf" "$ASSETS";
cp -r "$RESULT_DIR/license" "$ASSETS";

cp -r "$RESULT_DIR/share/processing_block/face_detector/blf_front" "$ASSETS/share/processing_block/face_detector";
cp -r "$RESULT_DIR/share/processing_block/face_template_extractor/30/1" "$ASSETS/share/processing_block/face_template_extractor/30";
cp "$RESULT_DIR/share/processing_block/liveness_estimator/2d_light/1.enc" "$ASSETS/share/processing_block/liveness_estimator/2d_light/";

cp "$RESULT_DIR/share/facedetectors/blf/front_model.enc" "$ASSETS/share/facedetectors/blf"
cp -r "$RESULT_DIR/share/faceanalysis" "$ASSETS/share";
cp -r "$RESULT_DIR/share/faceattributes" "$ASSETS/share";
cp -r "$RESULT_DIR/share/face_quality" "$ASSETS/share";
cp -r "$RESULT_DIR/share/fda" "$ASSETS/share";
cp -r "$RESULT_DIR/share/quality_iso" "$ASSETS/share";
