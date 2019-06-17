#!/usr/bin/env bash

set -e

docker pull mavsdk/mavsdk-fedora-29
docker pull mavsdk/mavsdk-fedora-30
docker pull mavsdk/mavsdk-ubuntu-16.04
docker pull mavsdk/mavsdk-ubuntu-18.04
docker pull mavsdk/mavsdk-ubuntu-18.04-px4-sitl

docker build -f Dockerfile-Fedora-29 -t mavsdk/mavsdk-fedora-29 .
docker build -f Dockerfile-Fedora-30 -t mavsdk/mavsdk-fedora-30 .
docker build -f Dockerfile-Ubuntu-16.04 -t mavsdk/mavsdk-ubuntu-16.04 .
docker build -f Dockerfile-Ubuntu-18.04 -t mavsdk/mavsdk-ubuntu-18.04 .
docker build -f Dockerfile-Ubuntu-18.04-PX4-SITL -t mavsdk/mavsdk-ubuntu-18.04-px4-sitl .

DATE=`date +%Y-%m-%d`

docker tag mavsdk/mavsdk-fedora-29:latest mavsdk/mavsdk-fedora-29:$DATE
docker tag mavsdk/mavsdk-fedora-30:latest mavsdk/mavsdk-fedora-30:$DATE
docker tag mavsdk/mavsdk-ubuntu-16.04:latest mavsdk/mavsdk-ubuntu-16.04:$DATE
docker tag mavsdk/mavsdk-ubuntu-18.04:latest mavsdk/mavsdk-ubuntu-18.04:$DATE
docker tag mavsdk/mavsdk-ubuntu-18.04-px4-sitl:latest mavsdk/mavsdk-ubuntu-18.04-px4-sitl:$DATE

docker push mavsdk/mavsdk-fedora-29:latest
docker push mavsdk/mavsdk-fedora-29:$DATE
docker push mavsdk/mavsdk-fedora-30:latest
docker push mavsdk/mavsdk-fedora-30:$DATE
docker push mavsdk/mavsdk-ubuntu-16.04:latest
docker push mavsdk/mavsdk-ubuntu-16.04:$DATE
docker push mavsdk/mavsdk-ubuntu-18.04:latest
docker push mavsdk/mavsdk-ubuntu-18.04:$DATE
docker push mavsdk/mavsdk-ubuntu-18.04-px4-sitl:latest
docker push mavsdk/mavsdk-ubuntu-18.04-px4-sitl:$DATE

echo "Please update tag in Jenkinsfile to '$DATE'"
