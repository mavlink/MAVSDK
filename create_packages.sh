#!/usr/bin/env bash

set -e

# This creates a version such as "v1.2.3-5-g123abc".
version=`git describe --always --tags`
# We want to extract 1.2.3 from it.
version=`echo $version | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

working_dir=`pwd`
root_dir=`pwd`/install
install_dir=$root_dir/usr

rm -rf $install_dir
make clean
make BUILD_TYPE=Release INSTALL_PREFIX=$install_dir default install

common_args="--input-type dir  \
    --version $version \
    --maintainer julian@oes.ch \
    --url https://sdk.dronecode.org \
    --license BSD-3-Clause \
    --force"


pushd $root_dir

# We need the relative path of all files.
library_files=`find . -type f | cut -sd / -f 2-`
echo $library_files

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
        $library_files

    dist_version=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9.]*//g')

    for file in *_amd64.deb
    do
        mv -v "$file" "$working_dir/${file%_amd64.deb}_ubuntu${dist_version}_amd64.deb"
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
        $library_files

    dist_version=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9]*//g')

    for file in *.x86_64.rpm
    do
        mv -v "$file" "$working_dir/${file%.x86_64.rpm}.fc${dist_version}-x86_64.rpm"
    done
fi

rm run_ldconfig

popd

rm -rf $install_dir
