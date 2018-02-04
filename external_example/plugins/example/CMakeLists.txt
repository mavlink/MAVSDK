add_library(dronecore_example SHARED
    example.cpp
    example_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/mavlink/include
)

target_link_libraries(dronecore_example
    dronecore
)

install(FILES
    example.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_example
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
