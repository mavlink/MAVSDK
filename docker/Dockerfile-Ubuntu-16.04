#
# Development environment for MAVSDK based on Ubuntu 16.04.
#
# Author: Julian Oes <julian@oes.ch>
#

FROM ubuntu:16.04
MAINTAINER Julian Oes <julian@oes.ch>

ENV DEBIAN_FRONTEND noninteractive


RUN apt-get update \
    && apt-get -y --quiet --no-install-recommends install \
        autoconf \
        automake \
        autotools-dev \
        build-essential \
        ca-certificates \
        ccache \
        cmake \
        colordiff \
        doxygen \
        git \
        golang-go \
        libcurl4-openssl-dev \
        libltdl-dev \
        libtinyxml2-dev \
        libtool \
        libz-dev \
        ninja-build \
        python \
        python-pip \
        ruby-dev \
        software-properties-common \
        sudo \
        wget \
    && apt-get -y autoremove \
    && apt-get clean autoclean \
    && rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/*

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-6.0 main" \
    && apt-get update \
    && apt-get install -y clang-format-6.0 \
    && rm -rf /var/lib/apt/lists/{apt,dpkg,cache,log} /tmp/* /var/tmp/* \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-6.0 1000

RUN gem install --no-document fpm;

RUN wget -qO- https://github.com/ncopa/su-exec/archive/dddd1567b7c76365e1e0aac561287975020a8fad.tar.gz | tar xvz && \
    cd su-exec-* && make && mv su-exec /usr/local/bin && cd .. && rm -rf su-exec-*

# Create user with id 1001 (Jenkins docker workflow default)
RUN useradd --shell /bin/bash -u 1001 -c "" -m user

ADD /sudoers.txt /etc/sudoers
RUN chmod 440 /etc/sudoers

COPY entrypoint.sh /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

WORKDIR "/home/user/MAVSDK"
