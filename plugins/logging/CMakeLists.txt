add_library(dronecore_logging SHARED
    logging.cpp
    logging.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/libs/include
)

install(FILES
    loggin.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_logging
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
