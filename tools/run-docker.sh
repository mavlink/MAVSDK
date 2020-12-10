#!/usr/bin/env sh

if command -v podman &> /dev/null
then
    podman run -it --rm -v $(pwd):/home/user/MAVSDK:z mavsdk/mavsdk-ubuntu-20.04-px4-sitl-v1.11 "$@"
    echo "sudo needed to repair file ownership after podman ran: sudo chown -R $USER:$USER ."
    sudo chown -R $USER:$USER .

elif command -v docker &> /dev/null
then
    docker run -it --rm -v $(pwd):/home/user/MAVSDK:z -e LOCAL_USER_ID=`id -u` mavsdk/mavsdk-ubuntu-18.04-px4-sitl "$@"

else
    echo "docker or podman commands not found"
    exit 1
fi
