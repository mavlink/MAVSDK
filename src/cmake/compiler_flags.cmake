if(MINGW)
    add_definitions(-DWINDOWS -DMINGW)
endif()

if(MSVC)
    add_definitions(-DWINDOWS -D_USE_MATH_DEFINES)
    set(warnings "-WX -W2")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Needed by gRPC headers
    add_definitions(-D_WIN32_WINNT=0x0600)

    # Needed by big auto-generated grpc/protobuf header files
    add_definitions(-bigobj)

    # We need this so Windows links to e.g. mavsdk_telemetry.dll.
    # Without this option it will look for mavsdk_telemetry.lib and fail.
    option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Export all symbols on Windows" ON)

    if(NOT BUILD_SHARED_LIBS)
        add_definitions(-DCURL_STATICLIB)
    endif()
else()
    # We are not using exceptions to make it easier to write wrappers.
    if (MINGW)
        set(warnings "-Wall -Wextra -Wshadow -Wno-strict-aliasing -Wold-style-cast -Wdouble-promotion")
    else()
        add_definitions(-fno-exceptions)
        set(warnings "-Wall -Wextra -Wshadow -Wno-strict-aliasing -Wold-style-cast -Wdouble-promotion -Wformat=2 -Wno-address-of-packed-member")
        if (WERROR)
            set(warnings "${warnings} -Werror")
        endif()
    endif()


    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5)
            set(warnings "${warnings} -Wno-shadow -Wno-effc++")
        endif()
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

        # MAVLink warnings
        set(warnings "${warnings} -Wno-address-of-packed-member")

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

if(ANDROID)
    add_definitions("-DANDROID")
endif()

if(UNIX AND NOT APPLE)
    add_definitions("-DLINUX")
endif()

if(ASAN)
    set(CMAKE_C_FLAGS "-fsanitize=address ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_C_FLAGS}")
endif()

if(UBSAN)
    set(CMAKE_C_FLAGS "-fsanitize=undefined ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fsanitize=undefined ${CMAKE_C_FLAGS}")
endif()

if(LSAN)
    set(CMAKE_C_FLAGS "-fsanitize=leak ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fsanitize=leak ${CMAKE_C_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} --coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_COVERAGE} --coverage")
set(CMAKE_LINKER_FLAGS_COVERAGE "${CMAKE_LINKER_FLAGS_COVERAGE} --coverage")
