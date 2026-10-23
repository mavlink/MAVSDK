#!/usr/bin/env bash

# Run with argument `--skip-checks` to skip checks for clean build and removing install dir.

# exit on any error
set -e

# This script will
# 1. copy the public headers into the layout they are installed with,
# 2. run doxygen and create html and xml docs,
# 3. run script to generate markdown from xml

echo "Generating docs"

show_docker_message() {
    echo ""
    echo "You can use doxygen from docker:"
    echo ""
    echo "    'tools/run-docker.sh tools/generate_docs.sh'"
}

if ! command -v doxygen >/dev/null 2>&1; then
    echo "Error: 'doxygen' is required and was not found!"
    show_docker_message
    exit 1
fi

DOXYGEN_VERSION=$(doxygen --version)
if [ "$DOXYGEN_VERSION" != "1.15.0" ]; then
    echo "Error: Required Doxygen version 1.15.0 not found. Found version: $DOXYGEN_VERSION"
    show_docker_message
    exit 1
fi

# Get current directory of script.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

overwrite=false
BUILD_DIR="${SCRIPT_DIR}/../build-docs"

while [ "$#" -gt 0 ]; do
    case "$1" in
        --overwrite)
            overwrite=true
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift
            ;;
    esac
    shift
done

INSTALL_DIR="${BUILD_DIR}/install"

# Doxygen only needs the public headers, so copy them instead of building and
# installing the library. This matches the install(FILES ...) rules in CMake.
HEADERS_DIR="${INSTALL_DIR}/include/mavsdk"
rm -rf "${HEADERS_DIR}" "${INSTALL_DIR}/docs"
mkdir -p "${HEADERS_DIR}/plugins"
cp "${SCRIPT_DIR}"/../src/mavsdk/core/include/mavsdk/*.{h,hpp} "${HEADERS_DIR}/"
cp -r "${SCRIPT_DIR}"/../src/mavsdk/plugins/*/include/plugins/* "${HEADERS_DIR}/plugins/"

return_result=0
# Doxygen likes to run where the source is (because INPUT in .doxygen is empty),
# so we cd there.
pushd "${HEADERS_DIR}"

# If any warnings are thrown, we should not flag this as a success.
doxygen_output_file=".doxygen_output.tmp"
doxygen "${SCRIPT_DIR}/.doxygen" &> $doxygen_output_file
cat $doxygen_output_file

if grep "warning" "${doxygen_output_file}" | grep -v "ignoring unsupported tag" | grep -v "operator" > /dev/null
then
    return_result=1
    echo "Please check doxygen warnings."
else
    "${SCRIPT_DIR}/generate_markdown_from_doxygen_xml.py" "${INSTALL_DIR}/docs" "${INSTALL_DIR}/docs"
fi

popd

if [ "$overwrite" = true ]; then
    # Clear folder first except index.md
    find "${SCRIPT_DIR}/../../docs/en/cpp/api_reference/" -type f -not -name 'index.md' -delete
    cp "${INSTALL_DIR}/docs/markdown/"* "${SCRIPT_DIR}/../../docs/en/cpp/api_reference/"
fi

exit $return_result
