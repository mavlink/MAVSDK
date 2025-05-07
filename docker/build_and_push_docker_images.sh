#!/usr/bin/env sh

set -e

DOCKER_CMD=$(which docker podman 2>/dev/null | head -n 1)

if [ -z "$DOCKER_CMD" ]; then
  echo "docker or podman commands not found"
  exit 1
fi

# Build Ubuntu images
$DOCKER_CMD build -f Dockerfile-ubuntu-20.04 -t docker.io/mavsdk/mavsdk-ubuntu-20.04 .
$DOCKER_CMD build -f Dockerfile-ubuntu-22.04 -t docker.io/mavsdk/mavsdk-ubuntu-22.04 .
$DOCKER_CMD build -f Dockerfile-ubuntu-24.04 -t docker.io/mavsdk/mavsdk-ubuntu-24.04 .

# For backward compatibility, also tag the 24.04 image as mavsdk-dev
$DOCKER_CMD tag docker.io/mavsdk/mavsdk-ubuntu-24.04 docker.io/mavsdk/mavsdk-dev:latest

# Build dockcross images
$DOCKER_CMD build -f Dockerfile.dockcross-linux-armv6-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-armv6-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-armv7-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-armv7-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-arm64-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-arm64-custom .
$DOCKER_CMD build -f Dockerfile.dockcross-linux-arm64-lts-custom -t docker.io/mavsdk/mavsdk-dockcross-linux-arm64-lts-custom .

# Push Ubuntu images
$DOCKER_CMD push docker.io/mavsdk/mavsdk-ubuntu-20.04:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-ubuntu-22.04:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-ubuntu-24.04:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dev:latest

# Push dockcross images
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-armv6-custom:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-armv7-custom:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-arm64-custom:latest
$DOCKER_CMD push docker.io/mavsdk/mavsdk-dockcross-linux-arm64-lts-custom:latest
