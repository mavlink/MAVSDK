#!/usr/bin/env bash

set -eo pipefail

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


merge_dsyms() {
    local output_dir="$1"
    local x64_dsym="$2/mavsdk_server.framework.dSYM"
    local arm64_dsym="$3/mavsdk_server.framework.dSYM"
    local dwarf_path="Contents/Resources/DWARF/mavsdk_server"

    cp -a "${x64_dsym}" "${output_dir}"
    xcrun lipo -create \
        "${x64_dsym}/${dwarf_path}" \
        "${arm64_dsym}/${dwarf_path}" \
        -output "${output_dir}/mavsdk_server.framework.dSYM/${dwarf_path}"

    if [ -d "${arm64_dsym}/Contents/Resources/Relocations" ]; then
        mkdir -p "${output_dir}/mavsdk_server.framework.dSYM/Contents/Resources/Relocations"
        cp -a "${arm64_dsym}/Contents/Resources/Relocations/." \
            "${output_dir}/mavsdk_server.framework.dSYM/Contents/Resources/Relocations/"
    fi
}

verify_dsym() {
    local backend_dir="$1"
    local binary_uuids
    local dsym_uuids

    binary_uuids=$(xcrun dwarfdump --uuid "${backend_dir}/mavsdk_server.framework/mavsdk_server" | awk '{print $2, $3}' | LC_ALL=C sort)
    dsym_uuids=$(xcrun dwarfdump --uuid "${backend_dir}/mavsdk_server.framework.dSYM/Contents/Resources/DWARF/mavsdk_server" | awk '{print $2, $3}' | LC_ALL=C sort)
    if [ -z "${binary_uuids}" ] || [ "${binary_uuids}" != "${dsym_uuids}" ]; then
        echo "Framework and dSYM UUIDs do not match in ${backend_dir}" >&2
        exit 1
    fi
}

echo "Running lipo for simulator"
mkdir -p ${IOS_SIM_FAT_BACKEND_DIR}
cp -a ${IOS_SIM_X64_BACKEND_DIR}/mavsdk_server.framework ${IOS_SIM_FAT_BACKEND_DIR}
file ${IOS_SIM_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server
file ${IOS_SIM_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server

xcrun lipo -create -output ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${IOS_SIM_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${IOS_SIM_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server

merge_dsyms "${IOS_SIM_FAT_BACKEND_DIR}" "${IOS_SIM_X64_BACKEND_DIR}" "${IOS_SIM_ARM64_BACKEND_DIR}"

tree ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework

echo "Running codesign for simulator"
xcrun codesign --verbose --sign - ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework



echo "Running lipo for macos"
mkdir -p ${MACOS_FAT_BACKEND_DIR}
cp -a ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework ${MACOS_FAT_BACKEND_DIR}
# Fix up symlinks that seem to break when github artifacts unzips the archives
ln -s -f ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework/Versions/Current/mavsdk_server ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server
ln -s -f ${MACOS_ARM64_BACKEND_DIR}/mavsdk_server.framework/Versions/Current/mavsdk_server ${MACOS_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server
file ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server
file ${MACOS_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server

xcrun lipo -create -output ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${MACOS_X64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server \
    ${MACOS_ARM64_BACKEND_DIR}/mavsdk_server.framework/mavsdk_server

merge_dsyms "${MACOS_FAT_BACKEND_DIR}" "${MACOS_X64_BACKEND_DIR}" "${MACOS_ARM64_BACKEND_DIR}"

tree ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework

echo "Running codesign for macos"
xcrun codesign --verbose --sign - ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework/Versions/Current

echo "Verifying framework dSYMs..."
verify_dsym "${IOS_BACKEND_DIR}"
verify_dsym "${IOS_SIM_FAT_BACKEND_DIR}"
verify_dsym "${MACOS_FAT_BACKEND_DIR}"

echo "Creating xcframework..."
xcodebuild -create-xcframework \
    -framework ${IOS_BACKEND_DIR}/mavsdk_server.framework \
    -debug-symbols "${IOS_BACKEND_DIR}/mavsdk_server.framework.dSYM" \
    -framework ${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework \
    -debug-symbols "${IOS_SIM_FAT_BACKEND_DIR}/mavsdk_server.framework.dSYM" \
    -framework ${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework \
    -debug-symbols "${MACOS_FAT_BACKEND_DIR}/mavsdk_server.framework.dSYM" \
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
