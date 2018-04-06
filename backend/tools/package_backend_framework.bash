#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IOS_BACKEND_DIR=${SCRIPT_DIR}/../../build/ios/backend/src
IOS_SIM_BACKEND_DIR=${SCRIPT_DIR}/../../build/ios_simulator/backend/src
FAT_BIN_DIR=${SCRIPT_DIR}/../../build/fat_bin

mkdir -p ${FAT_BIN_DIR}

if [ -d ${FAT_BIN_DIR}/backend.framework ]; then
    echo "${FAT_BIN_DIR}/backend.framework already exists! Aborting..."
    exit 1
fi

if [ -f ${FAT_BIN_DIR}/dronecode-backend.zip ]; then
    echo "${FAT_BIN_DIR}/dronecode-backend.zip already exists! Aborting..."
    exit 1
fi

cp -r ${IOS_BACKEND_DIR}/backend.framework ${FAT_BIN_DIR}
lipo ${IOS_BACKEND_DIR}/backend.framework/backend ${IOS_SIM_BACKEND_DIR}/backend.framework/backend -create -output ${FAT_BIN_DIR}/backend.framework/backend

cd ${FAT_BIN_DIR}
zip -9 -r dronecode-backend.zip backend.framework
