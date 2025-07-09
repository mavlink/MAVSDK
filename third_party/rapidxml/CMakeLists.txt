cmake_minimum_required(VERSION 3.13)

project(external-rapidxml)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"rapidxml\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    rapidxml
    GIT_REPOSITORY https://github.com/timniederhausen/rapidxml
    GIT_TAG master
    PREFIX rapidxml
    CMAKE_ARGS ${CMAKE_ARGS}
        -DRAPIDXML_BUILD_TESTS=OFF
        -DRAPIDXML_INSTALL=ON
    # Custom install step to ensure headers are properly installed
    INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/rapidxml.hpp ${CMAKE_INSTALL_PREFIX}/include/rapidxml/rapidxml.hpp
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/rapidxml_iterators.hpp ${CMAKE_INSTALL_PREFIX}/include/rapidxml/rapidxml_iterators.hpp
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/rapidxml_print.hpp ${CMAKE_INSTALL_PREFIX}/include/rapidxml/rapidxml_print.hpp
        COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/rapidxml_utils.hpp ${CMAKE_INSTALL_PREFIX}/include/rapidxml/rapidxml_utils.hpp
    )
