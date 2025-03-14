#!/usr/bin/env sh

set -e

DOCKER_CMD=$(which docker podman 2>/dev/null | head -n 1)

if [ -z "$DOCKER_CMD" ]; then
  echo "docker or podman commands not found"
  exit 1
fi

$DOCKER_CMD build -f Dockerfile-dev -t docker.io/mavsdk/mavsdk-dev .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-armv6-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-armv6-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-armv7-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-armv7-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-arm64-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-arm64-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-arm64-lts-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-arm64-lts-custom .

$DOCKER_CMD push docker.io/mavsdk/mavsdk-dev:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-armv6-custom:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-armv7-custom:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-arm64-custom:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-arm64-lts-custom:latest
