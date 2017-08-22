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
        python-dev \
		libcurl4-openssl-dev \
	&& apt-get -y autoremove \
	&& apt-get clean autoclean \
	&& rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*

CMD ["/bin/bash"]

WORKDIR "/home/docker1000/src/DroneCore"
