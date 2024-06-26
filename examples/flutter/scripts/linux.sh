#!/bin/bash

# exit on error
set -e

if [ $# != 1 ]; then
	echo "Usage: ./linux.sh <rootdir>";
	echo "Example: ./linux.sh /home/andrew/dropbox/face_sdk_3divi/2.3.0";
	exit 1
fi

RESULT_DIR=$1;

echo  RESULT_DIR:$RESULT_DIR

CUR="$RESULT_DIR/examples/flutter/demo/assets"
mkdir -p "$CUR/share/facerec/recognizers";
mkdir -p "$CUR/share/facedetectors";

cp -r "$RESULT_DIR/conf" "$CUR";
cp -r "$RESULT_DIR/license" "$CUR";

cp -r "$RESULT_DIR/share/face_quality" "$CUR/share";
cp -r "$RESULT_DIR/share/faceanalysis" "$CUR/share";
cp -r "$RESULT_DIR/share/faceattributes" "$CUR/share";
cp -r "$RESULT_DIR/share/fda" "$CUR/share";

cp -r "$RESULT_DIR/share/facedetectors/blf" "$CUR/share/facedetectors";

cp -r "$RESULT_DIR/share/facerec/recognizers/method12v30" "$CUR/share/facerec/recognizers";
