#!/bin/bash

set -e

cd processing_block
go mod init processing_block || true
go mod tidy
cd ..

cd microservice
go mod init microservice || true
go mod tidy
cd ..

if [ ! -d "facesdk" ]; then
    SCRIPT_DIR=$(dirname $(readlink -f $0))
    GO_API_PATH=$(readlink -f ${SCRIPT_DIR}/../../)/go_api/facesdk/

    cp -r ${GO_API_PATH} .
    cd facesdk
    go mod init facesdk || true
    go mod tidy
    cd ..
fi

go work init ./processing_block ./microservice ./facesdk
