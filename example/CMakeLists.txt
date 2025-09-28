find_package(cmavsdk REQUIRED)

add_executable(example)

target_sources(example
    PRIVATE
        main.cpp
)

target_link_libraries(example
    PRIVATE
        cmavsdk
)
