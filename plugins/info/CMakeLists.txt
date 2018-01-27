add_library(dronecore_info SHARED
    info.cpp
    info_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

if(ANDROID)
    target_link_libraries(dronecore_info
        log
    )
endif()

target_link_libraries(dronecore_info
    dronecore
)

install(FILES
    info.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_info
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
