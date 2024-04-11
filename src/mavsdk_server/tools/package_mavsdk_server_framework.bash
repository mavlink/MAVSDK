#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR=${SCRIPT_DIR}/../../../build

IOS_BACKEND_DIR=${BUILD_DIR}/ios/src/mavsdk_server/src
IOS_SIM_X64_BACKEND_DIR=${BUILD_DIR}/ios_simulator_x64/src/mavsdk_server/src
IOS_SIM_ARM64_BACKEND_DIR=${BUILD_DIR}/ios_simulator_arm64/src/mavsdk_server/src
IOS_SIM_FAT_BACKEND_DIR=${BUILD_DIR}/ios_simulator/src/mavsdk_server/src

MACOS_X64_BACKEND_DIR=${BUILD_DIR}/macos_x64/src/mavsdk_server/src
MACOS_ARM64_BACKEND_DIR=${BUILD_DIR}/macos_arm64/src/mavsdk_server/src
MACOS_FAT_BACKEND_DIR=${BUILD_DIR}/macos/src/mavsdk_server/src

if [ -d ${BUILD_DIR}/mavsdk_server.xcframework ]; then
    echo "${BUILD_DIR}/mavsdk_server.xcframework already exists! Aborting..."
    exit 1
fi


echo "Running lipo for simulator"
mkdir -p ${IOS_SIM_FAT_BACKEND_DIR}
cp -a ${IOS_SIM_X64_BACKEND_DIR}/mavsdk_server.framework ${IOS_SIM_FAT_BACKEND_DIR}
xcrun lipo -create -output ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${IOS_SIM_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${IOS_SIM_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server

tree ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework

echo "Running codesign for simulator"
xcrun codesign --verbose --sign - ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework



echo "Running lipo for macos"
mkdir -p ${MACOS_FAT_BACKEND_DIR}
cp -a ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework ${MACOS_FAT_BACKEND_DIR}
xcrun lipo -create -output ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${MACOS_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server

tree ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework

echo "Running codesign for macos"
xcrun codesign --verbose --sign - ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework/Versions/Current

echo "Creating xcframework..."
xcodebuild -create-xcframework \
    -framework ${IOS_BACKEND_DIR}/mavsdk_server.framework \
    -framework ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework \
    -framework ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework \
    -output ${BUILD_DIR}/mavsdk_server.xcframework

echo "Setting executable bit"
chmod +x ${BUILD_DIR}/mavsdk_server.xcframework/ios-arm64/mavsdk_server.framework/mavsdk_server
chmod +x ${BUILD_DIR}/mavsdk_server.xcframework/ios-arm64_x86_64-simulator/mavsdk_server.framework/mavsdk_server
chmod +x ${BUILD_DIR}/mavsdk_server.xcframework/macos-arm64_x86_64/mavsdk_server.framework/mavsdk_server

echo "Creating zip archive"
cd ${BUILD_DIR}
zip -9 -y -r mavsdk_server.xcframework.zip mavsdk_server.xcframework

shasum -a 256 mavsdk_server.xcframework.zip | awk '{ print $1 }' > mavsdk_server.xcframework.zip.sha256

echo "Success! You will find the xcframework in ${BUILD_DIR}!"
