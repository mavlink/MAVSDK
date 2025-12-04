#!/usr/bin/env bash

# Build MAVSDK with externally-provided dependencies (SUPERBUILD=OFF)
#
# This script demonstrates how to build MAVSDK when you want to provide
# dependencies yourself instead of using the superbuild.
#
# Prerequisites (install via apt on Ubuntu/Debian):
#   sudo apt install build-essential cmake git python3 python3-pip \
#                    liblzma-dev libtinyxml2-dev libjsoncpp-dev \
#                    libcurl4-openssl-dev libssl-dev
#
# Usage:
#   ./tools/build-with-system-deps.sh
#
# This will create:
#   - deps/          - source and build files for dependencies
#   - deps-install/  - installed dependencies
#   - build-release/ - MAVSDK build directory

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

DEPS_DIR="${ROOT_DIR}/deps"
DEPS_INSTALL_DIR="${ROOT_DIR}/deps-install"
BUILD_DIR="${ROOT_DIR}/build-release"

echo "=== Building MAVSDK with system dependencies ==="
echo "Dependencies will be built in: ${DEPS_DIR}"
echo "Dependencies will be installed to: ${DEPS_INSTALL_DIR}"
echo "MAVSDK will be built in: ${BUILD_DIR}"
echo ""

mkdir -p "${DEPS_DIR}"
mkdir -p "${DEPS_INSTALL_DIR}"

# Build MAVLink
# Use the same version as MAVSDK's superbuild (see third_party/CMakeLists.txt)
MAVLINK_VERSION="d6a7eeaf43319ce6da19a1973ca40180a4210643"
echo "=== Building MAVLink (${MAVLINK_VERSION}) ==="
if [ ! -d "${DEPS_DIR}/mavlink" ]; then
    git clone https://github.com/mavlink/mavlink.git "${DEPS_DIR}/mavlink"
    (cd "${DEPS_DIR}/mavlink" && git checkout "${MAVLINK_VERSION}" && git submodule update --init --recursive)
fi
cmake -B "${DEPS_DIR}/mavlink/build" -S "${DEPS_DIR}/mavlink" \
    -DMAVLINK_DIALECT=ardupilotmega
cmake --build "${DEPS_DIR}/mavlink/build"
cmake --install "${DEPS_DIR}/mavlink/build" --prefix "${DEPS_INSTALL_DIR}"
# MAVLink's cmake install doesn't include XML files, copy them manually
mkdir -p "${DEPS_INSTALL_DIR}/include/mavlink/message_definitions/v1.0"
cp "${DEPS_DIR}/mavlink/message_definitions/v1.0/minimal.xml" \
   "${DEPS_DIR}/mavlink/message_definitions/v1.0/standard.xml" \
   "${DEPS_DIR}/mavlink/message_definitions/v1.0/common.xml" \
   "${DEPS_DIR}/mavlink/message_definitions/v1.0/ardupilotmega.xml" \
   "${DEPS_INSTALL_DIR}/include/mavlink/message_definitions/v1.0/"

# Build libevents
# Use the same version as MAVSDK's superbuild (see third_party/libevents/CMakeLists.txt)
LIBEVENTS_VERSION="7c1720749dfe555ec2e71d5f9f753e6ac1244e1c"
echo "=== Building libevents (${LIBEVENTS_VERSION}) ==="
if [ ! -d "${DEPS_DIR}/libevents" ]; then
    git clone https://github.com/mavlink/libevents.git "${DEPS_DIR}/libevents"
    (cd "${DEPS_DIR}/libevents" && git checkout "${LIBEVENTS_VERSION}")
    python3 -m pip install -r "${DEPS_DIR}/libevents/requirements.txt" --break-system-packages || \
    python3 -m pip install -r "${DEPS_DIR}/libevents/requirements.txt"
fi
cmake -B "${DEPS_DIR}/libevents/build" -S "${DEPS_DIR}/libevents/libs/cpp" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
cmake --build "${DEPS_DIR}/libevents/build"
cmake --install "${DEPS_DIR}/libevents/build" --prefix "${DEPS_INSTALL_DIR}"

# Build PicoSHA2
echo "=== Building PicoSHA2 ==="
if [ ! -d "${DEPS_DIR}/PicoSHA2" ]; then
    git clone --branch cmake-install-support https://github.com/julianoes/PicoSHA2 "${DEPS_DIR}/PicoSHA2"
fi
cmake -B "${DEPS_DIR}/PicoSHA2/build" -S "${DEPS_DIR}/PicoSHA2"
cmake --build "${DEPS_DIR}/PicoSHA2/build"
cmake --install "${DEPS_DIR}/PicoSHA2/build" --prefix "${DEPS_INSTALL_DIR}"

# Build libmav
echo "=== Building libmav ==="
if [ ! -d "${DEPS_DIR}/libmav" ]; then
    git clone https://github.com/julianoes/libmavlike "${DEPS_DIR}/libmav"
    (cd "${DEPS_DIR}/libmav" && git reset --hard 80dbd91a0c5d6f0a79f1e8597b820ba075d1cf15)
fi
cmake -B "${DEPS_DIR}/libmav/build" -S "${DEPS_DIR}/libmav" \
    -DCMAKE_PREFIX_PATH="${DEPS_INSTALL_DIR}" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
cmake --build "${DEPS_DIR}/libmav/build"
cmake --install "${DEPS_DIR}/libmav/build" --prefix "${DEPS_INSTALL_DIR}"

# Build MAVSDK
echo "=== Building MAVSDK ==="
cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="${DEPS_INSTALL_DIR}" \
    -DSUPERBUILD=OFF
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo ""
echo "=== Build complete ==="
echo "MAVSDK library: ${BUILD_DIR}/src/mavsdk/libmavsdk.so"
echo ""
echo "To run tests:"
echo "  ${BUILD_DIR}/src/unit_tests/unit_tests_runner"
echo "  ${BUILD_DIR}/src/system_tests/system_tests_runner"
