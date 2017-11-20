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
    if [ -e $source_dir/build/ ]; then
        echo "Build directory ($install_prefix) already exists, you should do 'make clean' first."
        exit 1
    fi

    # Check for leftover install artefacts.
    if [ -e $install_prefix ]; then
        echo "Install directory ($install_prefix) already exists, you should delete it up first."
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
