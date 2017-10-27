cmake_minimum_required(VERSION 2.8.12)

include(FindPythonInterp REQUIRED)

add_custom_target(pydronecore ALL)

if(${PYTHONINTERP_FOUND})
    set(proto_dir ${CMAKE_SOURCE_DIR}/grpc/proto)

    add_custom_command(TARGET pydronecore
        COMMAND ${PYTHON_EXECUTABLE}
            -m grpc_tools.protoc
            -I ${proto_dir}
            --python_out=.
            --grpc_python_out=.
            ${proto_dir}/dronecore.proto
        DEPENDS ${proto_dir}/dronecore.proto
    )
    add_custom_command(TARGET pydronecore
        COMMAND ${PYTHON_EXECUTABLE}
            -m grpc_tools.protoc
            -I ${proto_dir}
            --python_out=.
            --grpc_python_out=.
            ${proto_dir}/action.proto
        DEPENDS ${proto_dir}/action.proto
    )

    add_custom_command(TARGET pydronecore
        COMMAND ${PYTHON_EXECUTABLE}
            -m grpc_tools.protoc
            -I ${proto_dir}
            --python_out=.
            --grpc_python_out=.
            ${proto_dir}/mission.proto
        DEPENDS ${proto_dir}/mission.proto
    )

    add_custom_command(TARGET pydronecore
        COMMAND ${PYTHON_EXECUTABLE}
            -m grpc_tools.protoc
            -I ${proto_dir}
            --python_out=.
            --grpc_python_out=.
            ${proto_dir}/telemetry.proto
        DEPENDS ${proto_dir}/telemetry.proto
    )
endif()
