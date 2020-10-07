#!/usr/bin/env sh

docker run -it --rm -v $(pwd):/home/user/MAVSDK:z -e LOCAL_USER_ID=`id -u` mavsdk/mavsdk-ubuntu-20.04-px4-sitl-v1.11 "$@"
