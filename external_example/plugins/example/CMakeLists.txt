add_library(dronecode_sdk_example SHARED
    example.cpp
    example_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

target_link_libraries(dronecode_sdk_example
    dronecode_sdk
)

install(FILES
    example.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_example
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
