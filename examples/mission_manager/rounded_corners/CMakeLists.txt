add_library(rounded_corners
    rounded_corners.cpp
    segment.cpp
    point.cpp
)

add_executable(rounded_corners_test
    rounded_corners_test.cpp
)

target_link_libraries(rounded_corners_test
    PRIVATE
    rounded_corners
)

if (VISUALIZE)
    FIND_PACKAGE(GLUT REQUIRED)
    find_package(OpenGL REQUIRED)

    target_link_libraries(rounded_corners_test
        PRIVATE
        GLUT::GLUT
        OpenGL::OpenGL
        GLU
    )

    target_compile_definitions(rounded_corners_test PRIVATE -DVISUALIZE=1)
endif()

enable_testing()
add_test(NAME rounded_corners_test
    COMMAND rounded_corners_test
)