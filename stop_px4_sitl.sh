#!/bin/sh

echo "Stop PX4..."

# Running HEADLESS, so no viewer to clean up
#killall gzviewer

killall gzserver

killall px4

echo "done"
