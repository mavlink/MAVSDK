add_library(dronecore_follow_me SHARED
    follow_me.cpp
    follow_me_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

if(ANDROID)
    target_link_libraries(dronecore_follow_me
        log
    )
endif()

target_link_libraries(dronecore_follow_me
    dronecore
)

install(FILES
    follow_me.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_follow_me
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
