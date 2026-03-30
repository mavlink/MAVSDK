#!/bin/bash

set -e  # Exit on any error

usage() {
    cat << EOF
    Usage: ./generate_from_protos.bash [options]

    -h, --help                   Show help
    -b, --build-dir BUILD_DIR    C++ build directory (passed to C++ generation script)
EOF
}

# Get the directory where this script is located
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd "$script_dir/.." && pwd)"
proto_dir="$project_root/../proto/protos"

build_dir_args=()
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
            build_dir_args=("--build-dir" "$(cd "$1" && pwd)")
            ;;
        --)
            shift
            break;;
    esac
shift
done

# Function to setup and activate virtual environment
setup_venv() {
    if [ -z "$VIRTUAL_ENV" ]; then
        echo "Not in a virtual environment. Setting up..."

        if [ ! -d "$project_root/venv" ]; then
            echo "Creating virtual environment..."
            python3 -m venv "$project_root/venv"
        fi

        echo "Activating virtual environment..."
        source "$project_root/venv/bin/activate"

        echo "Installing dependencies..."
        pip install protoc-gen-mavsdk

        echo "Virtual environment setup complete."
    else
        echo "Already in a virtual environment: $VIRTUAL_ENV"
    fi
}

# Main execution
main() {
    echo "Setting up venv..."
    setup_venv

    echo "Starting protoc code generation..."

    echo "------------------"
    echo "Generating C++"
    echo "------------------"
    pushd cpp
    ./tools/generate_from_protos.sh "${build_dir_args[@]}"
    popd

    echo "------------------"
    echo "Generating C"
    echo "------------------"
    pushd c
    ./tools/generate_from_protos.bash
    popd

    echo "------------------"
    echo "Generating Python (sync)"
    echo "------------------"
    pushd py/mavsdk
    ./tools/generate_from_protos.bash
    popd

    echo "------------------"
    echo "Generating Python (asyncio)"
    echo "------------------"
    pushd py/aiomavsdk
    ./tools/generate_from_protos.bash
    popd

    echo "All languages were generated successfully!"
}

# Run main function
main
