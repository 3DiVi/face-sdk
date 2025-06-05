#!/bin/bash

set -e

SCRIPT_DIR=$(dirname $(readlink -f $0))
ROOT_DIR=$(readlink -f ${SCRIPT_DIR}/../../../)

export CGO_LDFLAGS="-L${ROOT_DIR}/lib -lfacerec"

LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ROOT_DIR}/lib go run main.go -sdk_path=${ROOT_DIR} "$@"
