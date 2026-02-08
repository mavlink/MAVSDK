# Check if fuzzing is enabled and compiler supports libfuzzer
if(NOT BUILD_FUZZ_TESTS)
    return()
endif()

# Check if we're using Clang (required for libfuzzer)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(WARNING "Fuzz tests require Clang compiler. Skipping fuzz tests.")
    return()
endif()

# Create the MAVLink fuzzer executable
add_executable(mavlink_fuzzer
    mavlink_fuzzer.cpp
)

# Set fuzzer-specific compiler and linker flags
target_compile_options(mavlink_fuzzer PRIVATE
    -fsanitize=fuzzer,address
    -g
    -O1
)

target_link_options(mavlink_fuzzer PRIVATE
    -fsanitize=fuzzer,address
)

# Link against MAVSDK
target_link_libraries(mavlink_fuzzer
    mavsdk
)

# Include MAVSDK headers
target_include_directories(mavlink_fuzzer PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core/include
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

# Add custom target for running the fuzzer
add_custom_target(run_mavlink_fuzzer
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mavlink_fuzzer -max_total_time=60 -print_final_stats=1
    DEPENDS mavlink_fuzzer
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running MAVLink fuzzer for 60 seconds"
)

# Print instructions
message(STATUS "Fuzz tests enabled. Build target 'mavlink_fuzzer' to create fuzzer executable.")
message(STATUS "Run 'make run_mavlink_fuzzer' to execute fuzzing for 60 seconds.")
message(STATUS "Or run manually: ./mavlink_fuzzer -help for options")
