#!/usr/bin/env bash

set -e

docker pull mavlink/mavsdk-fedora-28
docker pull mavlink/mavsdk-fedora-29
docker pull mavlink/mavsdk-ubuntu-16.04
docker pull mavlink/mavsdk-ubuntu-18.04
docker pull mavlink/mavsdk-ubuntu-18.04-px4-sitl

docker build -f Dockerfile-Fedora-28 -t mavlink/mavsdk-fedora-28 .
docker build -f Dockerfile-Fedora-29 -t mavlink/mavsdk-fedora-29 .
docker build -f Dockerfile-Ubuntu-16.04 -t mavlink/mavsdk-ubuntu-16.04 .
docker build -f Dockerfile-Ubuntu-18.04 -t mavlink/mavsdk-ubuntu-18.04 .
docker build -f Dockerfile-Ubuntu-18.04-PX4-SITL -t mavlink/mavsdk-ubuntu-18.04-px4-sitl .

DATE=`date +%Y-%m-%d`

docker tag mavlink/mavsdk-fedora-28:latest mavlink/mavsdk-fedora-28:$DATE
docker tag mavlink/mavsdk-fedora-29:latest mavlink/mavsdk-fedora-29:$DATE
docker tag mavlink/mavsdk-ubuntu-16.04:latest mavlink/mavsdk-ubuntu-16.04:$DATE
docker tag mavlink/mavsdk-ubuntu-18.04:latest mavlink/mavsdk-ubuntu-18.04:$DATE
docker tag mavlink/mavsdk-ubuntu-18.04-px4-sitl:latest mavlink/mavsdk-ubuntu-18.04-px4-sitl:$DATE

docker push mavlink/mavsdk-fedora-28:latest
docker push mavlink/mavsdk-fedora-28:$DATE
docker push mavlink/mavsdk-fedora-29:latest
docker push mavlink/mavsdk-fedora-29:$DATE
docker push mavlink/mavsdk-ubuntu-16.04:latest
docker push mavlink/mavsdk-ubuntu-16.04:$DATE
docker push mavlink/mavsdk-ubuntu-18.04:latest
docker push mavlink/mavsdk-ubuntu-18.04:$DATE
docker push mavlink/mavsdk-ubuntu-18.04-px4-sitl:latest
docker push mavlink/mavsdk-ubuntu-18.04-px4-sitl:$DATE

echo "Please update tag in Jenkinsfile to '$DATE'"
