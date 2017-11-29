#!/bin/bash

# Run with argument `--clean` for clean build and removal of install directory.

# exit on any error
set -e

# This script will
# 1. build and install the source,
# 2. run doxygen and create html and xml docs,
# 3. run script to generate markdown from xml

# Get current directory of script.
source_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

help_msg="
Usage: ./generate_docs.sh [OPTIONS]
1. Builds and installs the source
2. Runs doxygen and create HTML and XML docs
3. Runs script to generate markdown from XML

  --clean    for clean build and removal of install directory.
  --help     display this help and exit
"

clean_build=false
if [ "$#" -eq 1 -a "$1" == "--clean" ]; then
    clean_build=true
elif [ "$#" -eq 1 ]; then
	echo "$help_msg"
	if [ "$1" == "--help" ]; then
		exit 0
	else
		exit 1
	fi
fi

# We use a local install folder so we don't need sudo.
install_prefix="$source_dir/install"

if [ "$clean_build" = true ]; then
    # We need to do a clean build, otherwise the INSTALL_PREFIX has no effect.
    # Check for leftover install artefacts.
    if [ -e $source_dir/build/ ] || [ -e $install_prefix ] ; then
		make clean
		rm -rf $install_prefix
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
