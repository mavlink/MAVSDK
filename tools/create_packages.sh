#!/usr/bin/env bash

set -e

echo "Packaging"

# This creates a version such as "v1.2.3-5-g123abc".
version=`git describe --always --tags`
# We want to extract 1.2.3 from it.
version=`echo ${version} | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
working_dir=${script_dir}/install
install_dir=${working_dir}/usr

rm -rf ${install_dir}

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${install_dir} -Bbuild/release-package -H.;
make -Cbuild/release-package install -j4;

common_args="--input-type dir  \
    --version ${version} \
    --maintainer julian@oes.ch \
    --url https://sdk.dronecode.org \
    --license BSD-3-Clause \
    --force"

pushd ${working_dir}

# We need the relative path of all files.
library_files=`find . -type f | cut -sd / -f 2-`
echo ${library_files}

echo "#!/bin/sh" > run_ldconfig

echo "/sbin/ldconfig" >> run_ldconfig
if cat /etc/os-release | grep 'Ubuntu'
then
    echo "Building DEB package"
    fpm ${common_args} \
        --output-type deb \
        --depends libcurl4-openssl-dev \
        --deb-no-default-config-files \
        --name libdronecode_sdk-dev \
        --provides libdronecode_sdk-dev \
        --after-install run_ldconfig \
        --after-remove run_ldconfig \
        ${library_files}

    dist_version=$(cat /etc/os-release | grep version_id | sed 's/[^0-9.]*//g')

    for file in *_amd64.deb
    do
        mv -v "${file}" "${script_dir}/${file%_amd64.deb}_ubuntu${dist_version}_amd64.deb"
    done

elif cat /etc/os-release | grep 'Fedora'
then
    echo "Building RPM package"
    fpm ${common_args} \
        --output-type rpm \
        --depends libcurl-devel \
        --name dronecode_sdk-devel \
        --provides dronecode_sdk-devel \
        --after-install run_ldconfig \
        --after-remove run_ldconfig \
        --rpm-rpmbuild-define "_build_id_links none" \
        ${library_files}

    dist_version=$(cat /etc/os-release | grep version_id | sed 's/[^0-9]*//g')

    for file in *.x86_64.rpm
    do
        mv -v "${file}" "${script_dir}/${file%.x86_64.rpm}.fc${dist_version}-x86_64.rpm"
    done
fi

rm run_ldconfig

popd

rm -rf ${install_dir}
