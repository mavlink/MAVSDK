if(MSVC)
    add_definitions(-DWINDOWS)
    set(warnings "-WX -W2")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # We need this so Windows links to e.g. dronecode_sdk_telemetry.dll.
    # Without this option it will look for dronecode_sdk_telemetry.lib and fail.
    option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Export all symbols on Windows" ON)
else()
    # We are not using exceptions to make it easier to write wrappers.
    add_definitions(-fno-exceptions)

    set(warnings "-Wall -Wextra -Werror -Wshadow -Wno-strict-aliasing -Wold-style-cast -Wdouble-promotion -Wformat=2 -Weffc++")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6)
            set(warnings "${warnings} -Wduplicated-cond -Wnull-dereference")
        endif()
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7)
            set(warnings "${warnings} -Wduplicated-branches")
        endif()

        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
            set(warnings "${warnings} -Wno-missing-field-initializers")
        endif()

        set(warnings "${warnings} -Wlogical-op")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(warnings "${warnings} -Wno-missing-braces -Wno-unused-lambda-capture")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(warnings "${warnings} -Wno-missing-braces -Wno-unused-parameter -Wno-unused-lambda-capture")
    endif()

    # Otherwise tinyxml2 complains.
    set(warnings "${warnings} -Wno-old-style-cast")
endif()

if(APPLE)
    add_definitions("-DAPPLE")
endif()

if(IOS)
    add_definitions("-DIOS")
endif()

if(UNIX AND NOT APPLE)
    add_definitions("-DLINUX")
endif()

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} --coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_COVERAGE} --coverage")
set(CMAKE_LINKER_FLAGS_COVERAGE "${CMAKE_LINKER_FLAGS_COVERAGE} --coverage")
