#!/usr/bin/env bash

set -e

echo "Packaging"
echo "---------"

if [ "$#" -ne 4 ]; then
  echo "Error: missing path to the 'install' directory (i.e. what was specified as -DCMAKE_INSTALL_PREFIX)" >&2
  echo "       or output path (where the package will be generated)!" >&2
  echo "Usage: $0 <path/to/install> <output/path> <architecture> <name>" >&2
  exit 1
fi

install_dir=$1
output_dir=$2
arch="$3"
name="$4"

# The package architecture needs to be armhf, at least for RPi.
if [ "$arch" = "armv6" ] || [ "$arch" = "armv7" ]; then
    package_arch="armhf"
else
    package_arch=$arch
fi

working_dir=$(mktemp -d)
mkdir -p "${working_dir}/install"
cp -r "${install_dir}" "${working_dir}/install/usr"

# We need the relative path of all files.
mapfile -t library_files < <(find "${working_dir}/install" -type f | cut -sd / -f 5-)
echo "Files to be packaged:"
printf "%s\n" "${library_files[@]}"

# This creates a version such as "v1.2.3-5-g123abc".
version=$(git describe --always --tags)
# We want to extract 1.2.3 from it.
version=$(echo "${version}" | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/')

echo "#!/bin/sh" > "${working_dir}/run_ldconfig"
echo "/sbin/ldconfig" >> "${working_dir}/run_ldconfig"

common_args=(--chdir "${working_dir}/install"
             --input-type dir
             --name "${name}"
             --version "${version}"
             --maintainer julian@oes.ch
             --url https://mavsdk.mavlink.io
             --license BSD-3-Clause
             --after-install "${working_dir}/run_ldconfig"
             --after-remove "${working_dir}/run_ldconfig"
             --force
             -a "${package_arch}")

if grep -q "Ubuntu" /etc/os-release;
then
    echo "Building Ubuntu DEB package"
    fpm "${common_args[@]}" \
        --output-type deb \
        --deb-no-default-config-files \
        "${library_files[@]}"

    dist_version=$(grep VERSION_ID /etc/os-release | sed 's/[^0-9.]*//g')

    for file in *_"${package_arch}.deb"
    do
        mv -v "${file}" "${output_dir}/${file%_"${package_arch}".deb}_ubuntu${dist_version}_${arch}.deb"
    done

elif grep -q "Debian" /etc/os-release;
then
    echo "Building Debian DEB package"

    fpm "${common_args[@]}" \
        --output-type deb \
        --deb-no-default-config-files \
        "${library_files[@]}"

    dist_version=$(grep VERSION_ID /etc/os-release | sed 's/[^0-9.]*//g')

    for file in *_"${package_arch}".deb
    do
        mv -v "${file}" "${output_dir}/${file%_"${package_arch}".deb}_debian${dist_version}_${arch}.deb"
    done

elif grep -q "Fedora" /etc/os-release;
then
    echo "Building RPM package"
    fpm "${common_args[@]}" \
        --output-type rpm \
        --rpm-rpmbuild-define "_build_id_links none" \
        "${library_files[@]}"

    dist_version=$(grep VERSION_ID /etc/os-release | sed 's/[^0-9]*//g')

    for file in *."${package_arch}".rpm
    do
        mv -v "${file}" "${output_dir}/${file%."${package_arch}".rpm}.fc${dist_version}-${arch}.rpm"
    done
fi
