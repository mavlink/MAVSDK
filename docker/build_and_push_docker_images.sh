#!/usr/bin/env bash

set -e

docker build -f Dockerfile-Fedora-30 -t mavsdk/mavsdk-fedora-30 .
docker build -f Dockerfile-Fedora-31 -t mavsdk/mavsdk-fedora-31 .
docker build -f Dockerfile-Ubuntu-16.04 -t mavsdk/mavsdk-ubuntu-16.04 .
docker build -f Dockerfile-Ubuntu-18.04 -t mavsdk/mavsdk-ubuntu-18.04 .
docker build -f Dockerfile-Ubuntu-18.04-PX4-SITL-v1.9 -t mavsdk/mavsdk-ubuntu-18.04-px4-sitl-v1.9 .
docker build -f Dockerfile-Ubuntu-18.04-PX4-SITL-v1.10 -t mavsdk/mavsdk-ubuntu-18.04-px4-sitl-v1.10 .

docker push mavsdk/mavsdk-fedora-30:latest
docker push mavsdk/mavsdk-fedora-31:latest
docker push mavsdk/mavsdk-ubuntu-16.04:latest
docker push mavsdk/mavsdk-ubuntu-18.04:latest
docker push mavsdk/mavsdk-ubuntu-18.04-px4-sitl-v1.9:latest
docker push mavsdk/mavsdk-ubuntu-18.04-px4-sitl-v1.10:latest
