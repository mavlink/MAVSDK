# Check if fuzzing is enabled and compiler supports libfuzzer
if(NOT BUILD_FUZZ_TESTS)
    return()
endif()

# Check if we're using Clang (required for libfuzzer)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(WARNING "Fuzz tests require Clang compiler. Skipping fuzz tests.")
    return()
endif()

# Create the UDP fuzzer executable
add_executable(udp_fuzzer
    udp_fuzzer.cpp
)

# Set fuzzer-specific compiler and linker flags
target_compile_options(udp_fuzzer PRIVATE
    -fsanitize=fuzzer,address
    -g
    -O1
)

target_link_options(udp_fuzzer PRIVATE
    -fsanitize=fuzzer,address
)

# Link against MAVSDK
target_link_libraries(udp_fuzzer
    mavsdk
)

# Include MAVSDK headers
target_include_directories(udp_fuzzer PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core/include
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

# Add custom target for running the fuzzer
add_custom_target(run_udp_fuzzer
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/udp_fuzzer -max_total_time=60 -print_final_stats=1
    DEPENDS udp_fuzzer
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running UDP fuzzer for 60 seconds"
)

# Print instructions
message(STATUS "Fuzz tests enabled. Build target 'udp_fuzzer' to create fuzzer executable.")
message(STATUS "Run 'make run_udp_fuzzer' to execute fuzzing for 60 seconds.")
message(STATUS "Or run manually: ./udp_fuzzer -help for options")
