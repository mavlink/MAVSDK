#!/usr/bin/env sh

set -e

DOCKER_CMD=$(which docker podman 2>/dev/null | head -n 1)

if [ -z "$DOCKER_CMD" ]; then
  echo "docker or podman commands not found"
  exit 1
fi

# Set default Docker image if not specified
DOCKER_IMAGE=${DOCKER_IMAGE:-docker.io/mavsdk/mavsdk-dev}

# Mount the whole git repo so git works inside, and start in the current subdirectory
CURRENT_DIR=$(pwd -P)
MOUNT_DIR=$(git rev-parse --show-toplevel 2>/dev/null || echo "$CURRENT_DIR")
WORK_DIR="/home/user/MAVSDK${CURRENT_DIR#"$MOUNT_DIR"}"

# Use the found container runtime
$DOCKER_CMD run -it --rm -v "$MOUNT_DIR":/home/user/MAVSDK:z -w "$WORK_DIR" -u "$(stat -c "%u:%g" "$MOUNT_DIR")" "${DOCKER_IMAGE}" "$@"
