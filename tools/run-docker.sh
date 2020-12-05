#!/usr/bin/env sh

podman run -it --rm -v $(pwd):/home/user/MAVSDK:z mavsdk/mavsdk-ubuntu-20.04-px4-sitl-v1.11 "$@"
sudo chown -R $USER:$USER .
