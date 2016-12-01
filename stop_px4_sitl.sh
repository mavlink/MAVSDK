#!/bin/bash

# This script shuts the Gazebo PX4 software in the loop (SITL) simulation down again.

if [[ $AUTOSTART_SITL != 1 ]]; then
    exit 0
fi

# Silently shutdown all leftover stuff.

if [[ "$unamestr" == 'Linux' ]]; then
    killall -q gzviewer
    killall -q gzserver
    killall -q px4
else
    killall gzviewer &
    killall gzserver &
    killall px4 &
fi
