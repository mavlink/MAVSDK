# Install script for directory: /home/amd/MAVSDK/src/plugins

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/action/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/calibration/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/camera/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/follow_me/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/geofence/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/gimbal/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/info/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/log_files/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/ftp/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/manual_control/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/mavlink_passthrough/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/mission/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/mission_raw/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/mocap/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/offboard/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/param/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/shell/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/telemetry/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/tune/cmake_install.cmake")
  include("/home/amd/MAVSDK/cmake-build-debug/src/plugins/failure/cmake_install.cmake")

endif()

