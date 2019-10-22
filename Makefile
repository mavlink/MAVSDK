all: default

default:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    mkdir -p build/default"
	@echo "    cd build/default"
	@echo "    cmake ../.."
	@echo "    make"
	@echo ""
	@echo "or, similarly:"
	@echo ""
	@echo "    cmake -Bbuild/default -H."
	@echo "    cmake --build build/default"
	@echo ""
	@echo "The following variables can be set:"
	@echo ""
	@echo "* -DCMAKE_BUILD_TYPE=Debug -> build in debug mode (or 'Release' for release mode)"
	@echo "* -DBUILD_BACKEND=ON -> build with the gRPC backend"
	@echo ""
	@echo "Find more information on https://www.dronecode.org/sdk/"

ios_simulator:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    mkdir -p build/ios_simulator"
	@echo "    cd build/ios_simulator"
	@echo "    cmake -DCMAKE_TOOLCHAIN_FILE:PATH=tools/ios.toolchain.cmake -DPLATFORM=SIMULATOR64 -DBUILD_BACKEND=ON ../.."
	@echo "    make"
	@echo ""
	@echo "or, similarly:"
	@echo ""
	@echo "    cmake -DCMAKE_TOOLCHAIN_FILE:PATH=tools/ios.toolchain.cmake -DPLATFORM=SIMULATOR64 -DBUILD_BACKEND=ON -Bbuild/ios_simulator -H."
	@echo "    cmake --build build/ios_simulator"
	@echo ""
	@echo "The following variables can be set:"
	@echo ""
	@echo "* -DCMAKE_BUILD_TYPE=Debug -> build in debug mode (or 'Release' for release mode)"
	@echo ""
	@echo "Find more information on https://www.dronecode.org/sdk/"

ios:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    mkdir -p build/ios"
	@echo "    cd build/ios"
	@echo "    cmake -DCMAKE_TOOLCHAIN_FILE:PATH=tools/ios.toolchain.cmake -DBUILD_BACKEND=ON ../.."
	@echo "    make"
	@echo ""
	@echo "or, similarly:"
	@echo ""
	@echo "    cmake -DCMAKE_TOOLCHAIN_FILE:PATH=tools/ios.toolchain.cmake -DBUILD_BACKEND=ON -Bbuild/ios -H."
	@echo "    cmake --build build/ios"
	@echo ""
	@echo "The following variables can be set:"
	@echo ""
	@echo "* -DCMAKE_BUILD_TYPE=Debug -> build in debug mode (or 'Release' for release mode)"
	@echo ""
	@echo "Find more information on https://www.dronecode.org/sdk/"

android_x86:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo "Instructions on how to build for Android will come soon."

android_x86_64:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo "Instructions on how to build for Android will come soon."

android_armeabi-v7a:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo "Instructions on how to build for Android will come soon."

android_arm64-v8a:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo "Instructions on how to build for Android will come soon."

android:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo "Instructions on how to build for Android will come soon."

fix_style:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    ./tools/fix_style.sh ."

run_unit_tests:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    ./build/default/unit_tests_runner"

run_unit_tests_offline:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    ./build/default/unit_tests_runner --gtest_filter=\"-CurlTest.*\""

run_integration_tests:
	@echo "This project no longer uses a Makefile, but relies solely on CMake."
	@echo ""
	@echo "You probably want to run something like:"
	@echo ""
	@echo "    ./build/default/integration_tests/integration_tests_runner"
