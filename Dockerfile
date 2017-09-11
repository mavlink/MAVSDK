#
# Development environment for DroneCore
#
# Author: Julian Oes <julian@oes.ch>
#

FROM ubuntu:xenial
MAINTAINER Julian Oes <julian@oes.ch>

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
	&& apt-get -y --quiet --no-install-recommends install \
		cmake \
		build-essential \
		colordiff \
		astyle \
		git \
		libcurl4-openssl-dev \
		doxygen \
        autoconf \
        libtool \
        ca-certificates \
        autoconf \
        automake \
        python \
        python-pip \
        software-properties-common \
	&& apt-get -y autoremove \
	&& apt-get clean autoclean \
	&& rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*

RUN add-apt-repository ppa:maarten-fonville/protobuf \
    && apt-get update \
    && apt-get -y --quiet --no-install-recommends install \
        protobuf-compiler \
        libprotobuf-dev \
    && apt-get clean autoclean \
	&& rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*

RUN pip install grpcio grpcio-tools

# Install grpc according to https://github.com/grpc/grpc/blob/v1.6.x/INSTALL.md
# And to fix build error https://github.com/grpc/grpc/issues/12316
RUN git clone -b v1.6.x https://github.com/grpc/grpc \
    && cd grpc \
    && git submodule update --init \
    && make -j8 HAS_SYSTEM_PROTOBUF=false \
    && make install \
    && cd .. \
    && rm -rf grpc

CMD ["/bin/bash"]

WORKDIR "/home/docker1000/src/DroneCore"
