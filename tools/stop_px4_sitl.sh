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


pkill -9 -f screen
pkill -9 -f python
pkill -9 -f run_in_terminal
pkill -9 -f mavproxy.py
pkill -9 -f ardurover
pkill -9 -f arducopter

sleep 3

killall $QUIET -SIGKILL gzviewer
killall $QUIET -SIGKILL gzserver
killall $QUIET -SIGKILL px4

pkill -9 -f screen
pkill -9 -f python
pkill -9 -f run_in_terminal
pkill -9 -f mavproxy.py
pkill -9 -f ardurover
pkill -9 -f arducopter

exit 0