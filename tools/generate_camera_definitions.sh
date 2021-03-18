#!/usr/bin/env bash

set -e

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

tmp_dir=$(mktemp -d)
echo "Temporary directory for this script: ${tmp_dir}"


###########
echo "* Building cpp_rsc binary..."

curl -L https://github.com/JonasVautherin/cpp_rsc/archive/master.tar.gz | tar -xvz -C ${tmp_dir}
clone_dir=${tmp_dir}/cpp_rsc-master

cmake -B${clone_dir}/build -H${clone_dir}
cmake --build ${clone_dir}/build

cpp_rsc=${clone_dir}/build/src/cpp_rsc


###########
echo "* Writing the config file for cpp_rsc..."

config_file="${tmp_dir}/camera_definition_config.rsc"
output_file="camera_definition_files"

echo "[general]" >> ${config_file}
echo "output-file-name=${output_file}" >> ${config_file}
echo "suffix-header=h" >> ${config_file}
echo "suffix-src=cpp" >> ${config_file}

for camera_xml in `ls ${script_dir}/../src/plugins/camera/camera_definition_files/ | grep -E "\.xml$"`; do
    echo "    -> adding file ${camera_xml}"
    echo "" >> ${config_file}
    echo "[file]" >> ${config_file}
    echo "file-path=${script_dir}/../src/plugins/camera/camera_definition_files/${camera_xml}" >> ${config_file}
    echo "var-name=${camera_xml%%.xml}xml" >> ${config_file}
    echo "text-file=TRUE" >> ${config_file}
done


###########
echo "* Generating camera_definition_files..."

generated_dir="${script_dir}/../src/plugins/camera/camera_definition_files/generated"
mkdir -p ${generated_dir}

cd ${generated_dir}
${cpp_rsc} ${config_file}
