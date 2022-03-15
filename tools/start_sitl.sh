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

# PX4 specific SITL Run Code
if [[ "${PX4_VERSION}" ]]; then
    if [[ -n "$PX4_FIRMWARE_DIR" ]]; then
        px4_firmware_dir=`realpath $PX4_FIRMWARE_DIR`
    else
        # Try to use the default path on the same folder level.
        px4_firmware_dir=`realpath ../PX4-Autopilot`
    fi

    # Make sure everything is stopped first.
    ${script_dir}/stop_sitl.sh

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

elif [[ "${APM_VERSION}" ]]; then
    if [[ -n "$APM_FIRMWARE_DIR" ]]; then
        apm_firmware_dir=`realpath $APM_FIRMWARE_DIR`
    else
        # Try to use the default path on the same folder level.
        apm_firmware_dir=`realpath ../ArduPilot`
    fi
    echo "DEBUG: APM_FIRMWARE_DIR: " ${APM_FIRMWARE_DIR}
    # Make sure everything is stopped first.
    ${script_dir}/stop_sitl.sh
    echo "DEBUG: script_dir: " ${script_dir}
    # To prevent any races.
    sleep 1

    current_dir=$(pwd)
    log_dir=$current_dir/logs
    echo "DEBUG: log_dir: " ${log_dir}

    # Create log dir in case it doesn't exist yet
    mkdir -p $log_dir

    # No user input needed, run it in deamon mode.
    # export NO_PXH=1

    # The logs are saved with a timestamp.
    timestamp=`date +%Y-%m-%dT%H-%M-%S%z`

    # Before changing dir, save where we start from.
    pushd .

    APM_HOME_LAT=${APM_HOME_LAT:-47.397742}
    APM_HOME_LONG=${APM_HOME_LONG:-8.545594}
    APM_HOME_ALT=${APM_HOME_ALT:-488}
    APM_HOME_DIR=${APM_HOME_DIR:-180}

    # Go to Firmware build dir.
    cd $apm_firmware_dir
    echo "DEBUG: current dir: " $(pwd)
    echo "DEBUG: Launching Screen"
    screen -dmS ardupilot python3 ./Tools/autotest/sim_vehicle.py \
            --vehicle ${APM_VEHICLE} \
            -I0 \
            --custom-location=${APM_HOME_LAT},${APM_HOME_LONG},${APM_HOME_ALT},${APM_HOME_DIR} \
            -w \
            --frame ${APM_FRAME} \
            --no-rebuild \
            --speedup 10 \
            -m "--out=udp:127.0.0.1:14540"
    
    # Go back to dir where we started
    popd

    echo "waiting for SITL to be running"
    sleep 10
    echo "done"
else
    echo "No Autopilot Version specified. Exiting."
    exit 1
fi
