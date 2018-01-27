add_library(dronecore_logging SHARED
    logging.cpp
    logging_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

if(ANDROID)
    target_link_libraries(dronecore_logging
        log
    )
endif()

target_link_libraries(dronecore_logging
    dronecore
)

install(FILES
    logging.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_logging
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
