add_library(rounded_corners
    rounded_corners.cpp
)

add_executable(rounded_corners_test
    rounded_corners_test.cpp
)

target_link_libraries(rounded_corners_test
    PRIVATE
    rounded_corners
)

enable_testing()
add_test(NAME rounded_corners_test
    COMMAND rounded_corners_test
)