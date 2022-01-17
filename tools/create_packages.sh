#!/usr/bin/env bash

set -e

echo "Packaging"
echo "---------"

if [ "$#" -ne 2 ]; then
  echo "Error: missing path to the 'install' directory (i.e. what was specified as -DCMAKE_INSTALL_PREFIX)" >&2
  echo "       or output path (where the package will be generated)!" >&2
  echo "Usage: $0 <path/to/install> <output/path>" >&2
  exit 1
fi

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
install_dir=$1
output_dir=$2

working_dir=$(mktemp -d)
mkdir -p ${working_dir}/install
cp -r ${install_dir} ${working_dir}/install/usr

# We need the relative path of all files.
library_files=`find ${working_dir}/install -type f | cut -sd / -f 5-`
echo "Files to be packaged:"
echo ${library_files}

# This creates a version such as "v1.2.3-5-g123abc".
version=`git describe --always --tags`
# We want to extract 1.2.3 from it.
version=`echo ${version} | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

echo "#!/bin/sh" > ${working_dir}/run_ldconfig
echo "/sbin/ldconfig" >> ${working_dir}/run_ldconfig

common_args="--chdir ${working_dir}/install \
             --input-type dir  \
             --name mavsdk \
             --version ${version} \
             --maintainer julian@oes.ch \
             --url https://mavsdk.mavlink.io \
             --license BSD-3-Clause \
             --after-install ${working_dir}/run_ldconfig \
             --after-remove ${working_dir}/run_ldconfig \
             --force"

if cat /etc/os-release | grep 'Ubuntu'
then
    echo "Building Ubuntu DEB package"
    fpm ${common_args} \
        --output-type deb \
        --deb-no-default-config-files \
        ${library_files}

    dist_version=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9.]*//g')

    for file in *_amd64.deb
    do
        mv -v "${file}" "${output_dir}/${file%_amd64.deb}_ubuntu${dist_version}_amd64.deb"
    done

elif cat /etc/os-release | grep 'Debian'
then
    echo "Building Debian DEB package"
    fpm ${common_args} \
        --output-type deb \
        --deb-no-default-config-files \
        ${library_files}

    dist_version=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9.]*//g')

    for file in *_amd64.deb
    do
        mv -v "${file}" "${output_dir}/${file%_amd64.deb}_debian${dist_version}_amd64.deb"
    done

elif cat /etc/os-release | grep 'Fedora'
then
    echo "Building RPM package"
    fpm ${common_args} \
        --output-type rpm \
        --rpm-rpmbuild-define "_build_id_links none" \
        ${library_files}

    dist_version=$(cat /etc/os-release | grep VERSION_ID | sed 's/[^0-9]*//g')

    for file in *.x86_64.rpm
    do
        mv -v "${file}" "${output_dir}/${file%.x86_64.rpm}.fc${dist_version}-x86_64.rpm"
    done
fi
