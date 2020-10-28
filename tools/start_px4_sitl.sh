#!/usr/bin/env bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$#" -ne 1 ]; then
    echo "Model argument needed"
    exit 1
fi

sitl_model=$1
echo "SITL model: $sitl_model"

sitl_world="empty"

set -e

# This script spawns the Gazebo PX4 software in the loop (SITL) simulation.
# Options:
#     AUTOSTART_SITL=1 if SITL is started manually or real hardware is used.
#     PX4_FIRMWARE_DIR=<path> to specify where the source of the PX4 Firmware is.
#     HEADLESS=1 if you don't need the simulation viewer.

if [[ $AUTOSTART_SITL != 1 ]]; then
    echo "Not autostarting SITL (use AUTOSTART_SITL=1)"
    exit 0
fi

if [[ -n "$PX4_FIRMWARE_DIR" ]]; then
    px4_firmware_dir=$PX4_FIRMWARE_DIR
else
    # Try to use the default path on the same folder level.
    px4_firmware_dir=`realpath ../Firmware`
fi

# Make sure everything is stopped first.
${script_dir}/stop_px4_sitl.sh

# To prevent any races.
sleep 1

current_dir=$(pwd)
log_dir=$current_dir/logs

# Create log dir in case it doesn't exist yet
mkdir -p $log_dir

# No user input needed, run it in deamon mode.
export NO_PXH=1

# The logs are saved with a timestamp.
timestamp=`date +%Y-%m-%dT%H-%M-%S%z`

# Before changing dir, save where we start from.
pushd .

# Go to Firmware build dir.
cd $px4_firmware_dir/build/px4_sitl_default/tmp/rootfs

# And run
if [ "$PX4_VERSION" = "v1.9" ] || [ "$PX4_VERSION" = "v1.10" ]; then
    $px4_firmware_dir/Tools/sitl_run.sh \
        $px4_firmware_dir/build/px4_sitl_default/bin/px4 \
        none \
        gazebo \
        $sitl_model \
        $px4_firmware_dir \
        $px4_firmware_dir/build/px4_sitl_default &
else
    # 1.11 onwards requires the world argument
    $px4_firmware_dir/Tools/sitl_run.sh \
        $px4_firmware_dir/build/px4_sitl_default/bin/px4 \
        none \
        gazebo \
        $sitl_model \
        $sitl_world \
        $px4_firmware_dir \
        $px4_firmware_dir/build/px4_sitl_default &
fi

# Go back to dir where we started
popd

echo "waiting for SITL to be running"
sleep 10
echo "done"
