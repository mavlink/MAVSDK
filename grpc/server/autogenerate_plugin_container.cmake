file(STRINGS ../proto/plugins.conf plugins_list)

foreach(plugin ${plugins_list})
    string(SUBSTRING ${plugin} 0 1 FIRST_LETTER)
    string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
    string(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" class_name "${plugin}")
    set(PLUGIN_MAP_APPEND_STRING "${PLUGIN_MAP_APPEND_STRING}    map[\"${plugin}\"] = &createInstances<${class_name}RPCImpl>;\n")
endforeach()

configure_file(dronecore_server.cpp.in dronecore_server.cpp)
