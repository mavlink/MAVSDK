#!/usr/bin/env sh

set -e

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

proto_dir="${script_dir}/../proto/protos"
backend_generated_dir="${script_dir}/../src/backend/src/generated"
third_party_dir="${script_dir}/../build/default/third_party"
protoc_binary="${third_party_dir}/install/bin/protoc"
protoc_grpc_binary="${third_party_dir}/install/bin/grpc_cpp_plugin"

command -v ${protoc_binary} && command -v ${protoc_grpc_binary} || {
    echo "-------------------------------"
    echo " Error"
    echo "-------------------------------"
    echo >&2 "'protoc' or 'grpc_cpp_plugin' not found"
    echo >&2 ""
    echo >&2 "Those are expected to be built for the host system in '${third_party_dir}'!"
    echo >&2 ""
    echo >&2 "You may want to run the CMake configure step first:"
    echo >&2 ""
    echo >&2 "    $ cmake -DBUILD_BACKEND=ON -Bbuild/default -S."
    exit 1
}

plugin_list="action calibration geofence gimbal camera core info mission mocap offboard param shell telemetry"

echo ""
echo "-------------------------------"
echo " Generating pb and grpc.pb files"
echo "    * protoc --version: $(${protoc_binary} --version)"
echo "-------------------------------"
echo ""

mkdir -p ${backend_generated_dir}

for plugin in ${plugin_list}; do
    ${protoc_binary} -I ${proto_dir} --cpp_out=${backend_generated_dir} --grpc_out=${backend_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/${plugin}/${plugin}.proto
done

${protoc_binary} -I ${proto_dir} --cpp_out=${backend_generated_dir} --grpc_out=${backend_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/mavsdk_options.proto

echo ""
echo "-------------------------------"
echo " Generating C++ and mavsdk_server files"
echo "    * protoc --version: $(${protoc_binary} --version)"
echo "-------------------------------"
echo ""

tmp_output_dir="$(mktemp -d)"
protoc_gen_dcsdk=$(which protoc-gen-dcsdk)
template_path_plugin_h="${script_dir}/../templates/plugin_h"
template_path_plugin_cpp="${script_dir}/../templates/plugin_cpp"
template_path_mavsdk_server="${script_dir}/../templates/mavsdk_server"

for plugin in action calibration camera geofence gimbal mission offboard param telemetry; do
    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_dcsdk} --custom_opt="file_ext=h,template_path=${template_path_plugin_h}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/${plugin^}.h ${script_dir}/../src/plugins/${plugin}/include/plugins/${plugin}/${plugin}.h

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_dcsdk} --custom_opt="file_ext=cpp,template_path=${template_path_plugin_cpp}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/${plugin^}.cpp ${script_dir}/../src/plugins/${plugin}/${plugin}.cpp

    ${protoc_binary} -I ${proto_dir} --custom_out=${tmp_output_dir} --plugin=protoc-gen-custom=${protoc_gen_dcsdk} --custom_opt="file_ext=h,template_path=${template_path_mavsdk_server}" ${proto_dir}/${plugin}/${plugin}.proto
    mv ${tmp_output_dir}/${plugin}/${plugin^}.h ${script_dir}/../src/backend/src/plugins/${plugin}/${plugin}_service_impl.h
done
