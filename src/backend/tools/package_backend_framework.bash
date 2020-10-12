#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR=${SCRIPT_DIR}/../../../build
IOS_BACKEND_DIR=${BUILD_DIR}/ios/src/backend/src
IOS_SIM_BACKEND_DIR=${BUILD_DIR}/ios_simulator/src/backend/src

if [ -d ${BUILD_DIR}/mavsdk_server.xcframework ]; then
    echo "${BUILD_DIR}/mavsdk_server.xcframework already exists! Aborting..."
    exit 1
fi

echo "Creating xcframework..."
xcodebuild -create-xcframework -framework ${IOS_BACKEND_DIR}/mavsdk_server.framework -framework ${IOS_SIM_BACKEND_DIR}/mavsdk_server.framework -output ${BUILD_DIR}/mavsdk_server.xcframework

cd ${BUILD_DIR}
zip -9 -r mavsdk_server.zip mavsdk_server.xcframework

echo "Success! You will find the xcframework in ${BUILD_DIR}!"
