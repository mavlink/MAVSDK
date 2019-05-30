cmake_minimum_required(VERSION 3.1)

project(external-boringssl)
include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/google/boringssl.git)
set(ARG_GIT_TAG 98ad4d77e372919980ae6cbcd19e928b8f80ce47)

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
        "-DOPENSSL_NO_ASM=ON" # Note: https://github.com/grpc/grpc/issues/9440
        )
else()
    list(APPEND CMAKE_ARGS
        "-DOPENSSL_NO_ASM=OFF" # Note: https://github.com/grpc/grpc/issues/9440
        )
endif()

message(STATUS "Preparing external project \"boringssl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    boringssl
    GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
    GIT_TAG ${ARG_GIT_TAG}
    PREFIX boringssl
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
