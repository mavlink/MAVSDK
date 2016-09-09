# To cross compile for iPhone:
# build$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../contrib/cmake/CrossCompile-iOS.cmake

if (NOT APPLE)
    message(FATAL_ERROR "\nNeed to build on a Mac for iOS\n")
endif()

SET(CMAKE_SYSTEM_NAME Generic)
if (NOT "$ENV{IOS_SDK_VERSION}" STREQUAL "")
	SET(CMAKE_SYSTEM_VERSION $ENV{IOS_SDK_VERSION})
endif()
SET(CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_CROSSCOMPILING_TARGET IOS)
SET(IOS ON)
SET(UNIX ON)
SET(APPLE ON)

execute_process(COMMAND xcode-select -print-path
    OUTPUT_VARIABLE XCODE_SELECT OUTPUT_STRIP_TRAILING_WHITESPACE)

if(EXISTS ${XCODE_SELECT})
	SET(DEVROOT "${XCODE_SELECT}/Platforms/iPhoneOS.platform/Developer")
  set(NEW_XCODE_LOCATION ON)
	if (NOT EXISTS "${DEVROOT}/SDKs/iPhoneOS${CMAKE_SYSTEM_VERSION}.sdk")
		# specified SDK version does not exist, use last one
		file(GLOB INSTALLED_SDKS ${DEVROOT}/SDKs/*)
		list(SORT INSTALLED_SDKS)
		list(REVERSE INSTALLED_SDKS)
		list(GET INSTALLED_SDKS 0 LATEST_SDK)
		string(REGEX MATCH "[0-9]\\.[0-9]" CMAKE_SYSTEM_VERSION ${LATEST_SDK})
	endif()
else()
  set(NEW_XCODE_LOCATION OFF)
	SET(DEVROOT "/Developer/Platforms/iPhoneOS.platform/Developer")
endif()

#message(FATAL_ERROR "${CMAKE_SYSTEM_VERSION}")

#if (${CMAKE_SYSTEM_VERSION} VERSION_EQUAL "5.0" OR ${CMAKE_SYSTEM_VERSION} VERSION_GREATER "5.0")
#  SET(CMAKE_OSX_ARCHITECTURES armv6 armv7)
#  SET(ARCHS "-arch armv6 -arch armv7")
#endif()
#
#if (${CMAKE_SYSTEM_VERSION} VERSION_EQUAL "6.0" OR ${CMAKE_SYSTEM_VERSION} VERSION_GREATER "6.0")
#  SET(CMAKE_OSX_ARCHITECTURES armv7 armv7s)
#  SET(ARCHS "-arch armv7 -arch armv7s")
#endif()

#if (${CMAKE_SYSTEM_VERSION} VERSION_EQUAL "7.0" OR ${CMAKE_SYSTEM_VERSION} VERSION_GREATER "7.0")
SET(CMAKE_OSX_ARCHITECTURES armv7 armv7s arm64)
SET(ARCHS "-arch armv7 -arch armv7s -arch arm64")
#endif()

# we have to use clang - llvm will choke on those __has_feature macros?
SET (CMAKE_C_COMPILER "${XCODE_SELECT}/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang")
SET (CMAKE_CXX_COMPILER "${XCODE_SELECT}/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++")

if ($ENV{MACOSX_DEPLOYMENT_TARGET})
	message(FATAL_ERROR "llvm will croak with MACOSX_DEPLOYMENT_TARGET environment variable set when building for ios - unset MACOSX_DEPLOYMENT_TARGET")
endif()

SET(SDKROOT "${DEVROOT}/SDKs/iPhoneOS${CMAKE_SYSTEM_VERSION}.sdk")
SET(CMAKE_OSX_SYSROOT "${SDKROOT}")

# force compiler and linker flags
SET(CMAKE_C_LINK_FLAGS ${ARCHS})
SET(CMAKE_CXX_LINK_FLAGS ${ARCHS})
# SET(CMAKE_C_FLAGS ${ARCHS}) # C_FLAGS wont stick, use ADD_DEFINITIONS instead
# SET(CMAKE_CXX_FLAGS ${ARCHS})

ADD_DEFINITIONS(${ARCHS})
ADD_DEFINITIONS("--sysroot=${SDKROOT} -stdlib=libc++ -fembed-bitcode -Wno-missing-braces")

# ios headers
INCLUDE_DIRECTORIES(SYSTEM "${SDKROOT}/usr/include")

# ios libraries
LINK_DIRECTORIES("${SDKROOT}/usr/lib/system")
LINK_DIRECTORIES("${SDKROOT}/usr/lib")

SET (CMAKE_FIND_ROOT_PATH "${SDKROOT}")
SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
