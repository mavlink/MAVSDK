#!/bin/bash

set -e

make BUILD_TYPE=Debug run_unit_tests -j4
make default install
make fix_style
(cd example/takeoff_land && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make)
(cd example/fly_mission && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make)
(cd example/offboard_velocity && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make)
make clean
git clean -dfx

make BUILD_TYPE=Release run_unit_tests -j4
make default install
make fix_style
(cd example/takeoff_land && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make)
(cd example/fly_mission && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make)
(cd example/offboard_velocity && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make)
make clean
git clean -dfx

make EXTERNAL_DIR=external_example
make clean
git clean -dfx

./generate_docs.sh
