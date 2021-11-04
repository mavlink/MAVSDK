#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR=${SCRIPT_DIR}/../../../build
IOS_BACKEND_DIR=${BUILD_DIR}/ios/src/mavsdk_server/src
IOS_SIM_BACKEND_DIR=${BUILD_DIR}/ios_simulator/src/mavsdk_server/src
MACOS_BACKEND_DIR=${BUILD_DIR}/macos/src/mavsdk_server/src

if [ -d ${BUILD_DIR}/mavsdk_server.xcframework ]; then
    echo "${BUILD_DIR}/mavsdk_server.xcframework already exists! Aborting..."
    exit 1
fi

echo "Fixing Modules in macOS framework"
ln -sf Versions/Current/Modules ${MACOS_BACKEND_DIR}/mavsdk_server.framework

echo "Creating xcframework..."
xcodebuild -create-xcframework -framework ${IOS_BACKEND_DIR}/mavsdk_server.framework -framework ${IOS_SIM_BACKEND_DIR}/mavsdk_server.framework -framework ${MACOS_BACKEND_DIR}/mavsdk_server.framework -output ${BUILD_DIR}/mavsdk_server.xcframework

chmod +x ${BUILD_DIR}/mavsdk_server.xcframework/ios-arm64/mavsdk_server.framework/mavsdk_server
chmod +x ${BUILD_DIR}/mavsdk_server.xcframework/ios-x86_64-simulator/mavsdk_server.framework/mavsdk_server
chmod +x ${BUILD_DIR}/mavsdk_server.xcframework/macos-x86_64/mavsdk_server.framework/mavsdk_server

cd ${BUILD_DIR}
zip -9 -r mavsdk_server.xcframework.zip mavsdk_server.xcframework

shasum -a 256 mavsdk_server.xcframework.zip | awk '{ print $1 }' > mavsdk_server.xcframework.zip.sha256

echo "Success! You will find the xcframework in ${BUILD_DIR}!"
