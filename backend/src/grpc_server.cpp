#include "grpc_server.h"

#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>

#include "log.h"

namespace dronecore {
namespace backend {

void GRPCServer::run()
{
    grpc::ServerBuilder builder;
    setup_port(builder);

    builder.RegisterService(&_core);
    builder.RegisterService(&_action_service);
    builder.RegisterService(&_mission_service);
    builder.RegisterService(&_telemetry_service);

    _server = builder.BuildAndStart();
    LogInfo() << "Server started";
}

void GRPCServer::wait()
{
    if (_server != nullptr) {
        _server->Wait();
    } else {
        LogWarn() << "Calling 'wait()' on a non-existing server. Did you call 'run()' before?";
    }
}

void GRPCServer::setup_port(grpc::ServerBuilder &builder)
{
    std::string server_address("0.0.0.0:50051");
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    LogInfo() << "Server set to listen on " << server_address;
}

} // namespace backend
} // namespace dronecore
