# Compilation step for static resources
# Taken from:
# https://beesbuzz.biz/code/4399-Embedding-binary-resources-with-CMake-and-C-11
function(ADD_RESOURCES out_var)
    set(result)
    foreach(in_f ${ARGN})
        set(out_f "${CMAKE_CURRENT_BINARY_DIR}/${in_f}.o")
        file(RELATIVE_PATH rel_path_with_file ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        get_filename_component(rel_path ${rel_path_with_file} DIRECTORY)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${rel_path})
        add_custom_command(OUTPUT ${out_f}
            COMMAND ld -r -b binary -o ${out_f} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f}
            DEPENDS ${in_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Adding resource ${in_f}"
            VERBATIM)
        list(APPEND result ${out_f})
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()
