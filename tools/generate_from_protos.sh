#!/usr/bin/env bash

set -e

usage() {
    cat << EOF
    Usage: ./generate_from_protos.sh [options]

    -h, --help                   Show help
    -b, --build-dir BUILD_DIR    Chosen build directory (default: build/default)
EOF
}

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
proto_dir="${script_dir}/../proto/protos"
build_dir="${script_dir}/../build/default"
repo_dir="${script_dir}/../"
options=$(getopt -l "help,build-dir:" -o "hb:" -a -- "$@")

eval set -- "$options"

while true
do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        -b|--build-dir)
            shift
            build_dir=$1
            ;;
        --)
            shift
            break;;
    esac
shift
done

third_party_dir="${build_dir}/third_party"
mavsdk_server_generated_dir="${script_dir}/../src/mavsdk_server/src/generated"
protoc_binary="${third_party_dir}/install/bin/protoc"
protoc_grpc_binary="${third_party_dir}/install/bin/grpc_cpp_plugin"

echo "Looking for ${protoc_binary}"
if ! command -v ${protoc_binary} > /dev/null; then
    echo "Falling back to looking for protoc in PATH"
    if ! protoc_binary="$(command -v protoc)"; then
        echo >&2 "No protoc binary found"
        echo >&2 "'protoc' not found"
        echo >&2 ""
        echo >&2 "You may want to run the CMake configure step first:"
        echo >&2 ""
        echo >&2 "    cmake -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H."
        echo >&2 ""
        echo >&2 "or set the build directory used with -b"
        echo >&2 ""
        usage
        exit 1
    fi
fi
echo "Found protoc ($(${protoc_binary} --version)): ${protoc_binary}"

echo "Looking for ${protoc_grpc_binary}"
if ! command -v ${protoc_grpc_binary} > /dev/null; then
echo "Falling back to looking for grpc_cpp_plugin in PATH"
    if ! protoc_grpc_binary="$(command -v grpc_cpp_plugin)"; then
        echo >&2 "No grpc_cpp_plugin binary found"
        echo >&2 "'grpc_cpp_plugin' not found"
        echo >&2 ""
        echo >&2 "You may want to run the CMake configure step first:"
        echo >&2 ""
        echo >&2 "    cmake -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H."
        echo >&2 ""
        echo >&2 "or set the build directory used with -b"
        echo >&2 ""
        usage
        exit 1
    fi
fi
echo "Found grpc_cpp_plugin: ${protoc_grpc_binary}"

function snake_case_to_camel_case {
    echo $1 | awk -v FS="_" -v OFS="" '{for (i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) substr($i,2)} 1'
}

command -v ${protoc_binary} > /dev/null && command -v ${protoc_grpc_binary} > /dev/null || {
    echo "-------------------------------"
    echo " Error"
    echo "-------------------------------"
}

echo "Installing protoc-gen-mavsdk locally into build folder"
python3 -m pip install --upgrade --target=${build_dir}/pb_plugins ${script_dir}/../proto/pb_plugins

protoc_gen_mavsdk="${build_dir}/pb_plugins/bin/protoc-gen-mavsdk"
export PYTHONPATH="${build_dir}/pb_plugins:${PYTHONPATH}"
echo "Using protoc_gen_mavsdk: ${protoc_gen_mavsdk}"

plugin_list_and_core=$(cd ${script_dir}/../proto/protos && ls -d */ | sed 's:/*$::')
plugin_list=$(cd ${script_dir}/../proto/protos && ls -d */ | sed 's:/*$::' | grep -v core)

echo "Processing mavsdk_options.proto"
${protoc_binary} -I ${proto_dir} --cpp_out=${mavsdk_server_generated_dir} --grpc_out=${mavsdk_server_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/mavsdk_options.proto

tmp_output_dir="$(mktemp -d)"
template_path_plugin_h="${script_dir}/../templates/plugin_h"
template_path_plugin_cpp="${script_dir}/../templates/plugin_cpp"
template_path_plugin_impl_h="${script_dir}/../templates/plugin_impl_h"
template_path_plugin_impl_cpp="${script_dir}/../templates/plugin_impl_cpp"
template_path_mavsdk_server="${script_dir}/../templates/mavsdk_server"
template_path_cmake="${script_dir}/../templates/cmake"

plugins_file="${script_dir}/../src/plugins.txt"
# Overwrite plugins file to be empty
> $plugins_file

for plugin in ${plugin_list_and_core}; do

    echo "Processing ${plugin}/${plugin}.proto"

    mkdir -p ${mavsdk_server_generated_dir}
    ${protoc_binary} -I ${proto_dir} --cpp_out=${mavsdk_server_generated_dir} --grpc_out=${mavsdk_server_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/${plugin}/${plugin}.proto

    if [[ "${plugin}" == "core" ]]; then
        continue
    fi

    mkdir -p ${script_dir}/../src/mavsdk/plugins/${plugin}/include/plugins/${plugin}
    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_plugin_h}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${script_dir}/../src/mavsdk/plugins/${plugin}/include/plugins/${plugin}/${plugin}.h

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=cpp,template_path=${template_path_plugin_cpp}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).cpp ${script_dir}/../src/mavsdk/plugins/${plugin}/${plugin}.cpp

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_mavsdk_server}" ${proto_dir}/${plugin}/${plugin}.proto
    mkdir -p ${script_dir}/../src/mavsdk_server/src/plugins/${plugin}
    mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${script_dir}/../src/mavsdk_server/src/plugins/${plugin}/${plugin}_service_impl.h

    file_impl_h="${script_dir}/../src/mavsdk/plugins/${plugin}/${plugin}_impl.h"
    if [[ ! -f "${file_impl_h}" ]]; then
        ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=h,template_path=${template_path_plugin_impl_h}" ${proto_dir}/${plugin}/${plugin}.proto
        mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).h ${file_impl_h}
        echo "-> Creating ${file_impl_h}"
    else
        # Warn if file is not checked in yet.
        if [[ ! $(git -C ${repo_dir} ls-files --error-unmatch ${file_impl_h} 2> /dev/null) ]]; then
            echo "-> Not creating ${file_impl_h} because it already exists"
        fi
    fi

    file_impl_cpp="${script_dir}/../src/mavsdk/plugins/${plugin}/${plugin}_impl.cpp"
    if [[ ! -f $file_impl_cpp ]]; then
        ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=cpp,template_path=${template_path_plugin_impl_cpp}" ${proto_dir}/${plugin}/${plugin}.proto
        mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).cpp ${file_impl_cpp}
        echo "-> Creating ${file_impl_cpp}"
    else
        # Warn if file is not checked in yet.
        if [[ ! $(git -C ${repo_dir} ls-files --error-unmatch ${file_impl_cpp} 2> /dev/null) ]]; then
            echo "-> Not creating ${file_impl_cpp} because it already exists"
        fi
    fi

    file_cmake="${script_dir}/../src/mavsdk/plugins/${plugin}/CMakeLists.txt"
    if [[ ! -f $file_cmake ]]; then
        ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_mavsdk} --custom_opt="file_ext=txt,template_path=${template_path_cmake}" ${proto_dir}/${plugin}/${plugin}.proto
        mv ${tmp_output_dir}/${plugin}/$(snake_case_to_camel_case ${plugin}).txt ${file_cmake}
        echo "-> Creating ${file_cmake}"
    else
        # Warn if file is not checked in yet.
        if [[ ! $(git -C ${repo_dir} ls-files --error-unmatch ${file_cmake} 2> /dev/null) ]]; then
            echo "-> Not creating ${file_cmake} because it already exists"
        fi
    fi

    echo "${plugin}" >> $plugins_file
done

# Generate grpc_server.h and grpc_server.cpp files according to plugin list
python3 ${script_dir}/grpc_server_jinja.py $plugin_list
