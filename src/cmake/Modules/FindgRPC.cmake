# Downloaded from https://raw.githubusercontent.com/wastl/cmarmotta/master/cmake/FindgRPC.cmake
# Adapted from GRPC to gRPC, removed protobuf stuff.

find_program(gRPC_CPP_PLUGIN grpc_cpp_plugin) # Get full path to plugin

find_library(gRPC_LIBRARY NAMES grpc)
find_library(gRPCPP_LIBRARY NAMES grpc++)
find_library(GPR_LIBRARY NAMES gpr)

if (gRPC_LIBRARY)
    message(STATUS "Found gRPC: ${gRPC_LIBRARY}; plugin - ${gRPC_CPP_PLUGIN}")
    if(NOT TARGET gRPC::grpc)
        add_library(gRPC::grpc INTERFACE IMPORTED)
        set_target_properties(gRPC::grpc PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${gRPC_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${gRPC_LIBRARY}")
    endif()
endif()
if (gRPCPP_LIBRARY)
    if(NOT TARGET gRPC::grpc++)
        add_library(gRPC::grpc++ INTERFACE IMPORTED)
        set_target_properties(gRPC::grpc++ PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${gRPCPP_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${gRPCPP_LIBRARY}")
    endif()
endif()
if (gGPR)
    if(NOT TARGET gRPC::gpr)
        add_library(gRPC::gpr INTERFACE IMPORTED)
        set_target_properties(gRPC::gpr PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${gPR_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${gPR_LIBRARY}")
    endif()
endif()
