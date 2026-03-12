#!/bin/bash

set -e  # Exit on any error

# Get the directory where this script is located
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd "$script_dir/.." && pwd)"
proto_dir="$project_root/../proto/protos"

# Default plugins if none provided
default_plugins=("action" "action_server" "arm_authorizer_server" "calibration" "camera" "camera_server" "component_metadata" "component_metadata_server" "events" "failure" "follow_me" "ftp" "ftp_server" "geofence" "gimbal" "gripper" "info" "log_files" "log_streaming" "manual_control" "mavlink_direct" "mission" "mission_raw" "mission_raw_server" "mocap" "offboard" "param" "param_server" "rtk" "server_utility" "shell" "telemetry" "telemetry_server" "transponder" "tune" "winch")

# Use provided plugins or defaults
plugins=("${@:-${default_plugins[@]}}")

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

# Function to run protoc commands for a plugin
process_plugin() {
    local plugin=$1
    local proto_file="$proto_dir/${plugin}/${plugin}.proto"
    local output_dir="$project_root/src/cmavsdk/plugins"
    local template_path="$project_root/templates"
    
    # Check if proto file exists
    if [ ! -f "$proto_file" ]; then
        echo "Error: Proto file not found: $proto_file"
        return 1
    fi
    
    # Check if template directory exists
    if [ ! -d "$template_path" ]; then
        echo "Error: Template directory not found: $template_path"
        return 1
    fi
    
    # Create output directory if it doesn't exist
    mkdir -p "$output_dir"
    
    echo "Processing $plugin..."
    
    # Generate header file
    echo "  Generating ${plugin}.h..."
    protoc "$proto_file" \
        --plugin=protoc-gen-custom="$(which protoc-gen-mavsdk)" \
        -I"$proto_dir/${plugin}" \
        -I"$proto_dir" \
        --custom_out="$output_dir" \
        --custom_opt="output_file=${plugin}/${plugin}.h" \
        --custom_opt="template_path=$template_path" \
        --custom_opt="template_file=file.h.j2" \
        --custom_opt="lstrip_blocks=True" \
        --custom_opt="trim_blocks=True"
    
    # Generate implementation file
    echo "  Generating ${plugin}.cpp..."
    protoc "$proto_file" \
        --plugin=protoc-gen-custom="$(which protoc-gen-mavsdk)" \
        -I"$proto_dir/${plugin}" \
        -I"$proto_dir" \
        --custom_out="$output_dir" \
        --custom_opt="output_file=${plugin}/${plugin}.cpp" \
        --custom_opt="template_path=$template_path" \
        --custom_opt="template_file=file.cpp.j2" \
        --custom_opt="lstrip_blocks=True" \
        --custom_opt="trim_blocks=True"
    
    echo "  $plugin processing complete."
}

# Main execution
main() {
    echo "Setting up venv..."
    setup_venv

    echo "Generating plugins_generated.cmake..."
    python3 ${script_dir}/generate_cmake.py ${plugins[*]}

    echo "Starting protoc code generation..."
    echo "Plugins to process: ${plugins[*]}"
    
    # Process each plugin
    for plugin in "${plugins[@]}"; do
        process_plugin "$plugin"
    done
    
    echo "All plugins processed successfully!"
}

# Run main function
main
