# Taken from https://github.com/lefticus/cpp_starter_project/blob/master/cmake/StaticAnalyzers.cmake
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK}
        --suppress=missingIncludeSystem:missingInclude
        --suppress=*:*/src/third_party/*
        -Isrc/mavsdk
        --inline-suppr
        --inconclusive
        --std=c++17
        .
    )
  else()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif()
endif()