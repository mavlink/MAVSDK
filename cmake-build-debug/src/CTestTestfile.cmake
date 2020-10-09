# CMake generated Testfile for 
# Source directory: /home/amd/MAVSDK/src
# Build directory: /home/amd/MAVSDK/cmake-build-debug/src
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(unit_tests "unit_tests_runner")
set_tests_properties(unit_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/amd/MAVSDK/src/cmake/unit_tests.cmake;29;add_test;/home/amd/MAVSDK/src/cmake/unit_tests.cmake;0;;/home/amd/MAVSDK/src/CMakeLists.txt;73;include;/home/amd/MAVSDK/src/CMakeLists.txt;0;")
subdirs("core")
subdirs("plugins")
subdirs("third_party/gtest")
subdirs("integration_tests")
