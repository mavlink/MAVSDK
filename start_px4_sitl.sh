#!/bin/bash
#set +e

echo "Start PX4 ..."

current_dir=$(pwd)
log_dir=$current_dir/logs

mkdir -p $log_dir

if [[ -n "$PX4_FIRMARE_DIR" ]]; then
    px4_firmware_dir=$PX4_FIRMARE_DIR
else
    px4_firmware_dir=$HOME/src/Firmware
fi

export NO_PXH=1
export HEADLESS=1

timestamp=`date --iso-8601=seconds`

pushd .
cd $px4_firmware_dir/build_posix_sitl_default/tmp
$px4_firmware_dir/Tools/sitl_run.sh $px4_firmware_dir/build_posix_sitl_default/src/firmware/posix/px4 posix-configs/SITL/init/lpe none gazebo iris $px4_firmware_dir $px4_firmware_dir/build_posix_sitl_default 2>1 > $log_dir/px4_sitl-$timestamp.log

popd

echo "done"
