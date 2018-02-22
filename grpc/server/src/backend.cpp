#include "backend.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "action/actionrpc_impl.h"
#include "core/corerpc_impl.h"
#include "log.h"
#include "mission/missionrpc_impl.h"
#include "telemetry/telemetryrpc_impl.h"

namespace dronecore {
namespace backend {

bool DroneCoreBackend::run(const int mavlink_listen_port)
{
    _connection_initiator.start(_dc, 14540);
    _connection_initiator.wait();

    grpc::ServerBuilder builder;
    setup_port(builder);

    CoreServiceImpl core(_dc);
    builder.RegisterService(&core);

    Action action(&_dc.device());
    ActionServiceImpl actionService(action);
    builder.RegisterService(&actionService);

    Mission mission(&_dc.device());
    MissionServiceImpl missionService(mission);
    builder.RegisterService(&missionService);

    Telemetry telemetry(&_dc.device());
    TelemetryServiceImpl telemetryService(telemetry);
    builder.RegisterService(&telemetryService);

    _server = builder.BuildAndStart();
    LogInfo() << "Server started";
    _server->Wait();

    return true;
}

void DroneCoreBackend::setup_port(grpc::ServerBuilder &builder)
{
    std::string server_address("0.0.0.0:50051");
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    LogInfo() << "Server set to listen on " << server_address;
}

} // namespace backend
} //namespace dronecore
