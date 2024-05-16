#!/bin/bash

# exit on error
set -e

if [ $# != 1 ]; then
	echo "Usage: ./processing_block <rootdir>";
	echo "Example: ./processing_block /home/andrew/dropbox/face_sdk_3divi/2.3.0";
	exit 1
fi

RESULT_DIR=$1;

echo RESULT_DIR:$RESULT_DIR

ASSETS="$RESULT_DIR/examples/flutter/processing_block_demo/assets"
mkdir -p "$ASSETS/share/faceanalysis";
mkdir -p "$ASSETS/share/processing_block/face_detector/uld";
mkdir -p "$ASSETS/share/processing_block/face_fitter/tddfa_faster";
mkdir "$ASSETS/share/faceattributes";
mkdir "$ASSETS/share/quality_iso";

cp -r "$RESULT_DIR/conf" "$ASSETS";
cp -r "$RESULT_DIR/license" "$ASSETS";

cp -r "$RESULT_DIR/share/processing_block/face_detector/uld" "$ASSETS/share/processing_block/face_detector";
cp -r "$RESULT_DIR/share/processing_block/face_fitter/tddfa_faster" "$ASSETS/share/processing_block/face_fitter";

cp -r "$RESULT_DIR/share/faceanalysis" "$ASSETS/share";
cp -r "$RESULT_DIR/share/faceattributes" "$ASSETS/share";
cp -r "$RESULT_DIR/share/quality_iso" "$ASSETS/share";