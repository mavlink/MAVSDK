#!/usr/bin/env bash

set -e

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

proto_dir="${script_dir}/../proto/protos"
mavsdk_server_generated_dir="${script_dir}/../src/mavsdk_server/src/generated"
third_party_dir="${script_dir}/../build/default/third_party"
protoc_binary="${third_party_dir}/install/bin/protoc"
protoc_grpc_binary="${third_party_dir}/install/bin/grpc_cpp_plugin"

function snake_case_to_camel_case {
    echo $1 | awk -v FS="_" -v OFS="" '{for (i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) substr($i,2)} 1'
}

command -v ${protoc_binary} > /dev/null && command -v ${protoc_grpc_binary} > /dev/null || {
    echo "-------------------------------"
    echo " Error"
    echo "-------------------------------"
    echo >&2 "'protoc' or 'grpc_cpp_plugin' not found"
    echo >&2 ""
    echo >&2 "Those are expected to be built for the host system in '${third_party_dir}'!"
    echo >&2 ""
    echo >&2 "You may want to run the CMake configure step first:"
    echo >&2 ""
    echo >&2 "    cmake -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H."
    exit 1
}

command -v protoc-gen-mavsdk > /dev/null || {
    echo "-------------------------------"
    echo " Error"
    echo "-------------------------------"
    echo >&2 "'protoc-gen-mavsdk' not found in PATH"
    echo >&2 ""
    echo >&2 "Make sure 'protoc-gen-mavsdk' is installed and available"
    echo >&2 "You can install it using pip:"
    echo >&2 ""
    echo >&2 "    pip3 install --user protoc-gen-mavsdk"
    exit 1
}

echo "Found protoc ($(${protoc_binary} --version)): ${protoc_binary}"
echo "Found grpc_cpp_plugin: ${protoc_grpc_binary}"

plugin_list_and_core=$(cd ${script_dir}/../proto/protos && ls -d */ | sed 's:/*$::')

echo "Processing mavsdk_options.proto"
${protoc_binary} -I ${proto_dir} --cpp_out=${mavsdk_server_generated_dir} --grpc_out=${mavsdk_server_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/mavsdk_options.proto

tmp_output_dir="$(mktemp -d)"
protoc_gen_mavsdk=$(which protoc-gen-mavsdk)
template_path_plugin_h="${script_dir}/../templates/plugin_h"
template_path_plugin_cpp="${script_dir}/../templates/plugin_cpp"
template_path_plugin_impl_h="${script_dir}/../templates/plugin_impl_h"
template_path_plugin_impl_cpp="${script_dir}/../templates/plugin_impl_cpp"
template_path_mavsdk_server="${script_dir}/../templates/mavsdk_server"
template_path_cmake="${script_dir}/../templates/cmake"
template_path_plugin_py="${script_dir}/../templates/mavsdk-python2"
template_path_plugin_swift_h="${script_dir}/../templates/mavsdk-swift2-h"
template_path_plugin_swift_mm="${script_dir}/../templates/mavsdk-swift2-mm"

for plugin in ${plugin_list_and_core}; do

    echo "Processing ${plugin}/${plugin}.proto"

    mkdir -p ${mavsdk_server_generated_dir}
    ${protoc_binary} -I ${proto_dir} --cpp_out=${mavsdk_server_generated_dir} --grpc_out=${mavsdk_server_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/${plugin}/${plugin}.proto

    if [[ "${plugin}" == "core" ]]; then
        continue
    fi

    mkdir -p ${script_dir}/../src/plugins/${plugin}/include/plugins/${plugin}
    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_plugin_h}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${script_dir}/../src/plugins/${plugin}/include/plugins/${plugin}/${plugin}.h

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=cpp,template_path=${template_path_plugin_cpp}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).cpp ${script_dir}/../src/plugins/${plugin}/${plugin}.cpp

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_mavsdk_server}" ${proto_dir}/${plugin}/${plugin}.proto
    mkdir -p ${script_dir}/../src/mavsdk_server/src/plugins/${plugin}
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${script_dir}/../src/mavsdk_server/src/plugins/${plugin}/${plugin}_service_impl.h

    file_impl_h="${script_dir}/../src/plugins/${plugin}/${plugin}_impl.h"
    if [[ ! -f "${file_impl_h}" ]]; then
        ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_plugin_impl_h}" ${proto_dir}/${plugin}/${plugin}.proto
        mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${file_impl_h}
        echo "-> Creating ${file_impl_h}"
    else
        # Warn if file is not checked in yet.
        if [[ ! $(git ls-files --error-unmatch ${file_impl_h} 2> /dev/null) ]]; then
            echo "-> Not creating ${file_impl_h} because it already exists"
        fi
    fi

    file_impl_cpp="${script_dir}/../src/plugins/${plugin}/${plugin}_impl.cpp"
    if [[ ! -f $file_impl_cpp ]]; then
        ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=cpp,template_path=${template_path_plugin_impl_cpp}" ${proto_dir}/${plugin}/${plugin}.proto
        mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).cpp ${file_impl_cpp}
        echo "-> Creating ${file_impl_cpp}"
    else
        # Warn if file is not checked in yet.
        if [[ ! $(git ls-files --error-unmatch ${file_impl_cpp} 2> /dev/null) ]]; then
            echo "-> Not creating ${file_impl_cpp} because it already exists"
        fi
    fi

    file_cmake="${script_dir}/../src/plugins/${plugin}/CMakeLists.txt"
    if [[ ! -f $file_cmake ]]; then
        ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=txt,template_path=${template_path_cmake}" ${proto_dir}/${plugin}/${plugin}.proto
        mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).txt ${file_cmake}
        echo "-> Creating ${file_cmake}"
    else
        # Warn if file is not checked in yet.
        if [[ ! $(git ls-files --error-unmatch ${file_cmake} 2> /dev/null) ]]; then
            echo "-> Not creating ${file_cmake} because it already exists"
        fi
    fi

    plugins_cmake_file="${script_dir}/../src/plugins/CMakeLists.txt"
    if [[ ! $(grep ${plugin} ${plugins_cmake_file}) ]]; then
        echo "-> Adding entry for '${plugin}' to ${plugins_cmake_file}"

        # We want to append the plugin to the list but before the passthrough plugin.
        # Therefore, we grep for the line numbers of add_subdirectory, cut to numbers only, and use the first of the two last.
        last_line=$(grep -n 'add_subdirectory' 'src/plugins/CMakeLists.txt' | cut -f1 -d: | tail -2 | head -n 1)
        # We have to increment by one to write it below the last one.
        last_line=$(($last_line+1))
        sed -i "${last_line}iadd_subdirectory(${plugin})" ${plugins_cmake_file}
    fi

    #${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=cpp,template_path=${template_path_plugin_py}" ${proto_dir}/${plugin}/${plugin}.proto
    #mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).cpp ${script_dir}/../mavsdk-python2/plugin-${plugin}.cpp

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_plugin_swift_h}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${script_dir}/../mavsdk-swift2/plugin-${plugin}.h
    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=mm,template_path=${template_path_plugin_swift_mm}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).mm ${script_dir}/../mavsdk-swift2/plugin-${plugin}.mm
done
