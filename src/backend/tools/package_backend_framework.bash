#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IOS_BACKEND_DIR=${SCRIPT_DIR}/../../../build/ios/src/backend/src
IOS_SIM_BACKEND_DIR=${SCRIPT_DIR}/../../../build/ios_simulator/src/backend/src
FAT_BIN_DIR=${SCRIPT_DIR}/../../../build/fat_bin

mkdir -p ${FAT_BIN_DIR}

if [ -d ${FAT_BIN_DIR}/mavsdk_server.framework ]; then
    echo "${FAT_BIN_DIR}/mavsdk_server.framework already exists! Aborting..."
    exit 1
fi

if [ -f ${FAT_BIN_DIR}/mavsdk_server.zip ]; then
    echo "${FAT_BIN_DIR}/mavsdk_server.zip already exists! Aborting..."
    exit 1
fi

echo "Creating fat bin..."

cp -r ${IOS_BACKEND_DIR}/mavsdk_server.framework ${FAT_BIN_DIR}
lipo ${IOS_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server ${IOS_SIM_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server -create -output ${FAT_BIN_DIR}/mavsdk_server.framework/mavsdk_server

cd ${FAT_BIN_DIR}
zip -9 -r mavsdk_server.zip mavsdk_server.framework

echo "Success! You'll find the fat bin in ${FAT_BIN_DIR}!"
