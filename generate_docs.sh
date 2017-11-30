#!/bin/bash

# Run with argument `--skip-checks` to skip checks for clean build and removing install dir.

# exit on any error
set -e

# This script will
# 1. build and install the source,
# 2. run doxygen and create html and xml docs,
# 3. run script to generate markdown from xml

# Get current directory of script.
source_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

skip_checks=false
if [ "$#" -eq 1 -a "$1" == "--skip-checks" ]; then
    skip_checks=true
fi

# We use a local install folder so we don't need sudo.
install_prefix="$source_dir/install"

if [ "$skip_checks" = false ]; then
    # We need to do a clean build, otherwise the INSTALL_PREFIX has no effect.
    # Check for leftover install artefacts.
    if [ -e $source_dir/build/ ] || [ -e $install_prefix ] ; then
		printf "Cleaup your build & install directory using below command.\nmake clean && rm -rf $install_prefix\n"
		exit 1
    fi
fi

# Build and install locally.
make INSTALL_PREFIX=$install_prefix default install

# Doxygen likes to run where the source is (because INPUT in .doxygen is empty),
# so we cd there.
pushd $install_prefix/include/dronecore
doxygen $source_dir/.doxygen
$source_dir/generate_markdown_from_doxygen_xml.py $install_prefix/docs $install_prefix/docs
popd
