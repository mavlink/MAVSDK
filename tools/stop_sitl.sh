#!/usr/bin/env bash

# This script shuts the Gazebo PX4 software in the loop (SITL) simulation down again.

# Prevent any potential races by waiting a bit.
sleep 1

if [[ $AUTOSTART_SITL != 1 ]]; then
    exit 0
fi

# Silently shutdown all leftover stuff.

UNAMESTR=$(uname)

if [[ "$UNAMESTR" == 'Linux' ]]; then
    QUIET="-q"
else
    QUIET=""
fi

killall $QUIET gzviewer
killall $QUIET gzserver
killall $QUIET px4


killall $QUIET sim_vehicle.py
killall $QUIET mavproxy.py
killall $QUIET ardurover
killall $QUIET arducopter

sleep 3

killall $QUIET -SIGKILL gzviewer
killall $QUIET -SIGKILL gzserver
killall $QUIET -SIGKILL px4

killall $QUIET -SIGKILL sim_vehicle.py
killall $QUIET -SIGKILL mavproxy.py
killall $QUIET -SIGKILL ardurover
killall $QUIET -SIGKILL arducopter

exit 0
