#!/usr/bin/env bash

# Note: each build (Release, Debug, Coverage) is done in its own folder,
#       hence removing the need to clean between them.
#       However, the third_parties (which are always built in Release
#       mode with the superbuild) are reused between the builds.

set -e

# Build and run offline tests in Debug mode
cmake -DCMAKE_BUILD_TYPE=Debug -Bbuild/debug -H.;
make -Cbuild/debug -j4;
./build/debug/src/unit_tests_runner --gtest_filter="-CurlTest.*";

#git clean -dfx # Does that remove even a gitignored folder?

#make fix_style

# Build and run offline tests in Release mode
cmake -DCMAKE_BUILD_TYPE=Release -Bbuild/release -H.;
make -Cbuild/release -j4;
./build/release/src/unit_tests_runner --gtest_filter="-CurlTest.*";

#make fix_style

# Try to build the external plugin example
cmake -DCMAKE_BUILD_TYPE=Release -DEXTERNAL_DIR=external_example -Bbuild/external-plugin -H.;
make -Cbuild/external-plugin -j4;

# Generate documentation
./tools/generate_docs.sh
