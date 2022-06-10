rm -rf build
cmake -DCMAKE_BUILD_TYPE=Debug -DMAVLINK_DIALECT=openhd -Bbuild/default -H.
cmake --build build/default -j8

sudo cmake --build build/default --target install