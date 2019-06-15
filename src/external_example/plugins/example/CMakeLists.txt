add_library(mavsdk_example SHARED
    example.cpp
    example_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

target_link_libraries(mavsdk_example
    mavsdk
)

install(FILES
    example.h
    DESTINATION ${mavsdk_install_include_dir}
)

install(TARGETS mavsdk_example
    DESTINATION ${mavsdk_install_lib_dir}
)
