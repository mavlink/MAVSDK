#!/usr/bin/env sh

set -e

DOCKER_CMD=$(which docker podman 2>/dev/null | head -n 1)

if [ -z "$DOCKER_CMD" ]; then
  echo "docker or podman commands not found"
  exit 1
fi

# Use the found container runtime
$DOCKER_CMD run -it --rm -v $(pwd):/home/user/MAVSDK:z -u `stat -c "%u:%g" $(pwd)` docker.io/mavsdk/mavsdk-dev "$@"
