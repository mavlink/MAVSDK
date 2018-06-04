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
    # Clean-up build & install directory
    if [ -d $source_dir/build ] || [ -d $install_prefix ] ; then
	printf "Clean-up your build & install directory using below command.\nmake distclean && rm -rf $install_prefix\n"
	exit 1
    fi
fi

# Build and install locally.
make INSTALL_PREFIX=$install_prefix default install

return_result=0
# Doxygen likes to run where the source is (because INPUT in .doxygen is empty),
# so we cd there.
pushd $install_prefix/include/dronecore
# If any warnings are thrown, we should not flag this as a success.
doxygen_output_file=".doxygen_output.tmp"
doxygen $source_dir/.doxygen &> $doxygen_output_file
cat $doxygen_output_file
if cat $doxygen_output_file | grep "warning" | grep -v "ignoring unsupported tag"
then
    return_result=1
    echo "Please check doxygen warnings."
fi
$source_dir/generate_markdown_from_doxygen_xml.py $install_prefix/docs $install_prefix/docs
popd

exit $return_result
