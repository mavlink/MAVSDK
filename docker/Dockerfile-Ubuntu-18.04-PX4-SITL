#
# PX4 SITL testing environment for MAVSDK based on Ubuntu 18.04.
# Author: Julian Oes <julian@oes.ch>
#
FROM mavlink/mavsdk-ubuntu-18.04
MAINTAINER Julian Oes <julian@oes.ch>

ENV FIRMWARE_DIR ${WORKDIR}../Firmware

RUN apt-get update && \
    apt-get install -y cmake \
                       curl \
                       git \
                       libeigen3-dev \
                       libopencv-dev \
                       libroscpp-dev \
                       protobuf-compiler \
                       python-empy \
                       python-jinja2 \
                       python-numpy \
                       python-toml \
                       python-yaml \
                       unzip \
                       gazebo9 \
                       libgazebo9-dev \
                       ninja-build \
                       psmisc \
    && apt-get -y autoremove \
    && apt-get clean autoclean \
    && rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*

RUN git clone https://github.com/PX4/Firmware.git ${FIRMWARE_DIR}
RUN git -C ${FIRMWARE_DIR} submodule update --init --recursive
RUN cd ${FIRMWARE_DIR} && DONT_RUN=1 make px4_sitl gazebo && DONT_RUN=1 make px4_sitl gazebo
