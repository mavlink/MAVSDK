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

plugin_list="action calibration geofence gimbal camera core info mission mocap offboard param telemetry"

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
