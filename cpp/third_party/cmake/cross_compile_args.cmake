macro(mavsdk_append_cross_compile_args output_var)
    if(CMAKE_CROSSCOMPILING)
        foreach(cross_var
                CMAKE_SYSTEM_NAME
                CMAKE_SYSTEM_PROCESSOR
                CMAKE_C_COMPILER
                CMAKE_CXX_COMPILER
                CMAKE_AR
                CMAKE_RANLIB
                CMAKE_STRIP
                PKG_CONFIG_EXECUTABLE)
            if(DEFINED ${cross_var} AND NOT "${${cross_var}}" STREQUAL "")
                list(APPEND ${output_var} "-D${cross_var}=${${cross_var}}")
            endif()
        endforeach()
    endif()
endmacro()
