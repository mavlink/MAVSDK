file(STRINGS "${PC_FILE}" requires_private_line REGEX "^Requires\\.private:")

if(BUILD_SHARED_LIBS)
    if(requires_private_line)
        message(FATAL_ERROR "Shared builds must not emit Requires.private in ${PC_FILE}: ${requires_private_line}")
    endif()
    return()
endif()

set(expected_requires_private "nlohmann_json tinyxml2")
if(NOT BUILD_WITHOUT_CURL)
    set(expected_requires_private "libcurl ${expected_requires_private}")
endif()

if(NOT requires_private_line STREQUAL "Requires.private: ${expected_requires_private}")
    message(
        FATAL_ERROR
        "Static builds must emit 'Requires.private: ${expected_requires_private}' in ${PC_FILE}, got '${requires_private_line}'"
    )
endif()
