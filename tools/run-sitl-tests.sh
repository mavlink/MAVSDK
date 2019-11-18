#!/usr/bin/env sh

set -e

PWD=$(pwd)

if [ -z ${PX4_FIRMWARE_DIR+x} ]; then
    PX4_FIRMWARE_DIR=$PWD/../Firmware
fi

NPROCS=$(nproc --all)

cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_BACKEND=OFF -DBUILD_SHARED_LIBS=ON -Bbuild/debug -H.;
cmake --build build/debug -- -j$NPROCS;
PX4_SIM_SPEED_FACTOR=10 AUTOSTART_SITL=1 PX4_FIRMWARE_DIR=$PX4_FIRMWARE_DIR HEADLESS=1 build/debug/src/integration_tests/integration_tests_runner --gtest_filter="SitlTest.*"
