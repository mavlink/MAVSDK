#!/usr/bin/env bash

set -e

echo "Packaging"

# This creates a version such as "v1.2.3-5-g123abc".
VERSION=`git describe --always --tags`
# We want to extract 1.2.3 from it.
VERSION=`echo ${VERSION} | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKING_DIR=${SCRIPT_DIR}/install
INSTALL_DIR=${WORKING_DIR}/usr

rm -rf ${INSTALL_DIR}

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -Bbuild/release-package -H.;
make -Cbuild/release-package install -j4;

common_args="--input-type dir  \
    --version ${VERSION} \
    --maintainer julian@oes.ch \
    --url https://sdk.dronecode.org \
    --license BSD-3-Clause \
    --force"


pushd ${WORKING_DIR}

# We need the relative path of all files.
LIBRARY_FILES=`find . -type f | cut -sd / -f 2-`
echo ${LIBRARY_FILES}

echo "#!/bin/sh" > run_ldconfig

echo "/sbin/ldconfig" >> run_ldconfig
if cat /etc/os-release | grep 'Ubuntu'
then
    echo "Building DEB package"
    fpm $common_args \
        --output-type deb \
        --depends libcurl4-openssl-dev \
        --deb-no-default-config-files \
        --name libdronecode_sdk-dev \
        --provides libdronecode_sdk-dev \
        --after-install run_ldconfig \
        --after-remove run_ldconfig \
        ${LIBRARY_FILES}

    DIST_VERSION=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9.]*//g')

    for FILE in *_amd64.deb
    do
        mv -v "$FILE" "${SCRIPT_DIR}/${FILE%_amd64.deb}_ubuntu${DIST_VERSION}_amd64.deb"
    done

elif cat /etc/os-release | grep 'Fedora'
then
    echo "Building RPM package"
    fpm $common_args \
        --output-type rpm \
        --depends libcurl-devel \
        --name dronecode_sdk-devel \
        --provides dronecode_sdk-devel \
        --after-install run_ldconfig \
        --after-remove run_ldconfig \
        --rpm-rpmbuild-define "_build_id_links none" \
        ${LIBRARY_FILES}

    DIST_VERSION=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9]*//g')

    for FILE in *.x86_64.rpm
    do
        mv -v "$FILE" "${SCRIPT_DIR}/${FILE%.x86_64.rpm}.fc${DIST_VERSION}-x86_64.rpm"
    done
fi

rm run_ldconfig

popd

rm -rf ${INSTALL_DIR}
