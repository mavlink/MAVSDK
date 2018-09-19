#!/bin/sh

docker run -it --rm --hostname dronecode-docker -v $(pwd):/home/dronecode/DronecodeSDK dronecode
