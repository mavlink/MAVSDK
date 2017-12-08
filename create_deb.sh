#!/bin/bash

set -e

# This creates a version such as "v1.2.3-5-g123abc".
version=`git describe --always --tags`
# We want to extract 1.2.3 from it.
version=`echo $version | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

make clean
make INSTALL_PREFIX=/usr BUILD_TYPE=Release

sudo checkinstall \
    --maintainer="Julian Oes \<julian@oes.ch\>" \
    --pkgname="dronecore-dev" \
    --pkgversion="$version" \
    --pkgarch="amd64" \
    --pkgrelease=1 \
    --pkglicense="BSD-3-clause" \
    --requires="libcurl4-openssl-dev,libtinyxml2-dev" \
    --provides="dronecore-dev" \
    --backup=no \
    --deldoc=yes \
    --deldesc=yes \
    --default make default install

