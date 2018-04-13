if(MSVC)
    add_definitions(-DWINDOWS)
    set(warnings "-WX -W2")

    # We need this so Windows links to e.g. dronecore_telemetry.dll.
    # Without this option it will look for dronecore_telemetry.lib and fail.
    option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Export all symbols on Windows" ON)
else()
    # We are not using exceptions to make it easier to write wrappers.
    add_definitions(-fno-exceptions)

    set(warnings "-Wall -Wextra -Werror -Wshadow -Wno-strict-aliasing -Wold-style-cast -Wdouble-promotion -Wformat=2")

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
        set(warnings "${warnings} -Wno-missing-braces")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(warnings "${warnings} -Wno-missing-braces -Wno-unused-parameter")
    endif()

    # Otherwise tinyxml2 complains.
    set(warnings "${warnings} -Wno-old-style-cast")
endif()

if(APPLE)
    # We need a define if on APPLE
    add_definitions("-DAPPLE")
endif()

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} --coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_COVERAGE} --coverage")
set(CMAKE_LINKER_FLAGS_COVERAGE "${CMAKE_LINKER_FLAGS_COVERAGE} --coverage")
