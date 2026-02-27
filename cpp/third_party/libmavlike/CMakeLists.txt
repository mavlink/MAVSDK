include(FetchContent)

set(BUILD_TESTING OFF CACHE BOOL "Disable libmavlike tests" FORCE)

FetchContent_Declare(
    libmavlike
    GIT_REPOSITORY https://github.com/julianoes/libmavlike
    GIT_TAG 80dbd91a0c5d6f0a79f1e8597b820ba075d1cf15
)

FetchContent_MakeAvailable(libmavlike)