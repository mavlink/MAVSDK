#!/usr/bin/env bash

set -e

DOCKER_CMD=docker

if ! command -v $DOCKER_CMD &> /dev/null
then
    echo "docker not found, trying podman instead"
    DOCKER_CMD=podman
fi

$DOCKER_CMD build -f Dockerfile-Ubuntu-22.04 -t mavsdk/mavsdk-ubuntu-22.04 .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-armv6-custom -t mavsdk/mavsdk-dockcross-linux-armv6-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-armv7-custom -t mavsdk/mavsdk-dockcross-linux-armv7-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-arm64-custom -t mavsdk/mavsdk-dockcross-linux-arm64-custom .

$DOCKER_CMD push mavsdk/mavsdk-ubuntu-22.04:latest
$DOCKER_CMD push mavsdk/mavsdk-dockcross-linux-armv6-custom:latest
$DOCKER_CMD push mavsdk/mavsdk-dockcross-linux-armv7-custom:latest
$DOCKER_CMD push mavsdk/mavsdk-dockcross-linux-arm64-custom:latest
