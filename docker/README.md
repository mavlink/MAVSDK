# MAVSDK Docker Images

This directory contains Dockerfiles for building Docker images used in the MAVSDK project.

## Available Images

### Ubuntu Images

- `Dockerfile-ubuntu-20.04`: Ubuntu 20.04 with build dependencies for MAVSDK
- `Dockerfile-ubuntu-22.04`: Ubuntu 22.04 with build dependencies for MAVSDK
- `Dockerfile-ubuntu-24.04`: Ubuntu 24.04 with build dependencies for MAVSDK

These images include all the dependencies needed for development, CI builds, and documentation generation.

### Debian Images

- `Dockerfile-debian-11`: Debian 11 (Bullseye)
- `Dockerfile-debian-12`: Debian 12 (Bookworm)
- `Dockerfile-debian-13`: Debian 13 (Trixie)

### Dockcross Images

- `Dockerfile.dockcross-linux-armv6-custom`: For cross-compiling to ARMv6
- `Dockerfile.dockcross-linux-armv7-custom`: For cross-compiling to ARMv7
- `Dockerfile.dockcross-linux-arm64-custom`: For cross-compiling to ARM64
- `Dockerfile.dockcross-linux-arm64-lts-custom`: For cross-compiling to ARM64 LTS

## Using the Docker Images

Docker images are built by the project and are hosted on Docker Hub. You can use them simply by:

```bash
cd /whereever/MAVSDK
./cpp/tools/run-docker.sh
```

By default, this will use the `mavsdk/mavsdk-dev` image, which is an alias for the Ubuntu 24.04 image.

To use a specific Ubuntu version:

```bash
cd /whereever/MAVSDK
DOCKER_IMAGE=mavsdk/mavsdk-ubuntu-24.04 ./cpp/tools/run-docker.sh
```

## CI Integration

The Docker images are used in the GitHub Actions workflow to provide consistent build environments for the CI pipeline. The workflow uses the images as containers for the jobs, which allows us to have a consistent build environment across all jobs.
