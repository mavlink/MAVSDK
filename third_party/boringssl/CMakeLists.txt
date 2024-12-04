cmake_minimum_required(VERSION 3.10.2)

project(external-boringssl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DANDROID_ABI:STRING=${ANDROID_ABI}"
        "-DANDROID_STL:STRING=${ANDROID_STL}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DIOS_PLATFORM=${IOS_PLATFORM}"
        )
endif()

# We disable ASM optimizations for MSVC and iOS (see above for iOS), which
# might have a performance impact on secure communications.
# This should be benchmarked/evaluated before we decide if it is worth
# enabling them or not in our case.
#
# More information here:  https://github.com/grpc/grpc/issues/9440
if (IOS OR MSVC)
    list(APPEND CMAKE_ARGS
        "-DOPENSSL_NO_ASM=ON"
        )
else()
    list(APPEND CMAKE_ARGS
        "-DOPENSSL_NO_ASM=OFF"
        )
endif()

message(STATUS "Preparing external project \"boringssl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    boringssl
    URL https://github.com/google/boringssl/releases/download/0.20241203.0/boringssl-0.20241203.0.tar.gz
    URL_MD5 3320699951028ba64150b5296bc9d0e6
    PREFIX boringssl
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
