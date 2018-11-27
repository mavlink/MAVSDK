#!/usr/bin/env sh

docker run -it --rm -v $(pwd):/home/user/DronecodeSDK -e LOCAL_USER_ID=`id -u` dronecode/dronecode-sdk-ubuntu-18.04 "$@"
