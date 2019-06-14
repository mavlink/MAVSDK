#!/usr/bin/env bash

set -e

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Build and run offline tests in Release mode
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=build/release/install -DBUILD_BACKEND=ON -Bbuild/release -H.;
make -Cbuild/release -j4 install;
./build/release/src/unit_tests_runner --gtest_filter="-CurlTest.*";
./build/release/src/backend/test/unit_tests_backend;

# Try to build the external plugin example
cmake -DCMAKE_BUILD_TYPE=Release -DEXTERNAL_DIR=external_example -Bbuild/external-plugin -H.;
make -Cbuild/external-plugin -j4;

# Check style
${script_dir}/fix_style.sh ${script_dir}/..

# Generate documentation
${script_dir}/generate_docs.sh

# Generate packages
${script_dir}/create_packages.sh ${script_dir}/../build/release/install ${script_dir}/../build/release
