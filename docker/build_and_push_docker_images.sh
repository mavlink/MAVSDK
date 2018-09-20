#!/usr/bin/env bash

set -e

docker pull dronecode/dronecode-sdk-fedora-27
docker pull dronecode/dronecode-sdk-fedora-28
docker pull dronecode/dronecode-sdk-ubuntu-16.04
docker pull dronecode/dronecode-sdk-ubuntu-18.04

docker build -f Dockerfile-Fedora-27 -t dronecode/dronecode-sdk-fedora-27 .
docker build -f Dockerfile-Fedora-28 -t dronecode/dronecode-sdk-fedora-28 .
docker build -f Dockerfile-Ubuntu-16.04 -t dronecode/dronecode-sdk-ubuntu-16.04 .
docker build -f Dockerfile-Ubuntu-18.04 -t dronecode/dronecode-sdk-ubuntu-18.04 .

DATE=`date +%Y-%m-%d`

docker tag dronecode/dronecode-sdk-fedora-27:latest dronecode/dronecode-sdk-fedora-27:$DATE
docker tag dronecode/dronecode-sdk-fedora-28:latest dronecode/dronecode-sdk-fedora-28:$DATE
docker tag dronecode/dronecode-sdk-ubuntu-16.04:latest dronecode/dronecode-sdk-ubuntu-16.04:$DATE
docker tag dronecode/dronecode-sdk-ubuntu-18.04:latest dronecode/dronecode-sdk-ubuntu-18.04:$DATE

docker push dronecode/dronecode-sdk-fedora-27:latest
docker push dronecode/dronecode-sdk-fedora-27:$DATE
docker push dronecode/dronecode-sdk-fedora-28:latest
docker push dronecode/dronecode-sdk-fedora-28:$DATE
docker push dronecode/dronecode-sdk-ubuntu-16.04:latest
docker push dronecode/dronecode-sdk-ubuntu-16.04:$DATE
docker push dronecode/dronecode-sdk-ubuntu-18.04:latest
docker push dronecode/dronecode-sdk-ubuntu-18.04:$DATE

echo "Please update tag in Jenkinsfile to '$DATE'"
