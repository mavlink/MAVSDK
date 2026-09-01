#!/usr/bin/env bash

# Build MAVSDK using distribution packages for common dependencies.
#
# MAVSDK-specific dependencies that are not generally packaged by Linux
# distributions are still built by the MAVSDK superbuild.
#
# Prerequisites (install via apt on Ubuntu/Debian):
#   sudo apt install build-essential cmake git python3 python3-lxml \
#                    liblzma-dev libtinyxml2-dev nlohmann-json3-dev \
#                    libcurl4-openssl-dev libasio-dev libfmt-dev
#
# Usage:
#   ./tools/build-with-system-deps.sh
#
# This creates build-release/ as the MAVSDK build directory.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

BUILD_DIR="${ROOT_DIR}/build-release"

echo "=== Building MAVSDK with system dependencies ==="
echo "MAVSDK will be built in: ${BUILD_DIR}"
echo ""

echo "=== Building MAVSDK ==="
cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DSUPERBUILD=ON \
    -DMAVSDK_USE_SYSTEM_COMMON_DEPENDENCIES=ON
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo ""
echo "=== Build complete ==="
echo "MAVSDK library: ${BUILD_DIR}/src/mavsdk/libmavsdk.so"
echo ""
echo "To run tests:"
echo "  ${BUILD_DIR}/src/unit_tests/unit_tests_runner"
echo "  ${BUILD_DIR}/src/system_tests/system_tests_runner"
