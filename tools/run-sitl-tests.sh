#!/usr/bin/env sh

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path/to/Firmware>"
    exit 1
fi

PX4_FIRMWARE_DIR=$1
NPROCS=$(nproc --all)
PX4_VERSIONS="master"

cmake -DCMAKE_BUILD_TYPE=Debug -DASAN=ON -DUBSAN=ON -DLSAN=ON -DBUILD_MAVSDK_SERVER=OFF -DBUILD_SHARED_LIBS=ON -j $NPROCS -Bbuild/debug -H.;
cmake --build build/debug -- -j $NPROCS;

# Filter the tests to run according to availability in upstream
if [ -n "`echo $PX4_VERSIONS | xargs -n1 echo | grep -e \"^$PX4_VERSION$\"`" ] ; then
  PX4_SIM_SPEED_FACTOR=10 AUTOSTART_SITL=1 PX4_FIRMWARE_DIR=$PX4_FIRMWARE_DIR HEADLESS=1 build/debug/src/integration_tests/integration_tests_runner --gtest_filter="SitlTest.*"
else
  PX4_SIM_SPEED_FACTOR=10 AUTOSTART_SITL=1 PX4_FIRMWARE_DIR=$PX4_FIRMWARE_DIR HEADLESS=1 build/debug/src/integration_tests/integration_tests_runner --gtest_filter="SitlTest.*:-SitlTest.CustomAction*"
fi
