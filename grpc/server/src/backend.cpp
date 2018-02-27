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
    if (!connect_to_vehicle(mavlink_listen_port)) {
        return false;
    }

    grpc::ServerBuilder builder;
    setup_port(builder);

    CoreServiceImpl core(dc);
    builder.RegisterService(&core);

    Action action(&dc.device());
    ActionServiceImpl actionService(action);
    builder.RegisterService(&actionService);

    Mission mission(&dc.device());
    MissionServiceImpl missionService(mission);
    builder.RegisterService(&missionService);

    Telemetry telemetry(&dc.device());
    TelemetryServiceImpl telemetryService(telemetry);
    builder.RegisterService(&telemetryService);

    server = builder.BuildAndStart();
    LogInfo() << "Server started";
    server->Wait();

    return true;
}

bool DroneCoreBackend::connect_to_vehicle(const int port)
{
    if (!add_udp_connection(port)) {
        return false;
    }

    log_uuid_on_timeout();
    wait_for_discovery();

    return true;
}

bool DroneCoreBackend::add_udp_connection(const int port)
{
    dronecore::ConnectionResult connection_result = dc.add_udp_connection(port);

    if (connection_result != ConnectionResult::SUCCESS) {
        LogErr() << "Connection failed: " << connection_result_str(connection_result);
        return false;
    }

    return true;
}

void DroneCoreBackend::log_uuid_on_timeout()
{
    dc.register_on_timeout([](uint64_t uuid) {
        LogInfo() << "Device timed out [UUID: " << uuid << "]";
    });
}

void DroneCoreBackend::wait_for_discovery()
{
    LogInfo() << "Waiting to discover device...";
    auto discoveryFuture = wrapped_register_on_discover();
    discoveryFuture.wait();
    LogInfo() << "Device discovered [UUID: " << discoveryFuture.get() << "]";
}

std::future<uint64_t> DroneCoreBackend::wrapped_register_on_discover()
{
    auto promise = std::make_shared<std::promise<uint64_t>>();
    auto future = promise->get_future();

    dc.register_on_discover([this, promise](uint64_t uuid) {
        std::call_once(discovery_flag, [promise, uuid]() {
            promise->set_value(uuid);
        });
    });

    return future;
}

void DroneCoreBackend::setup_port(grpc::ServerBuilder &builder)
{
    std::string server_address("0.0.0.0:50051");
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    LogInfo() << "Server set to listen on " << server_address;
}

} // namespace backend
} //namespace dronecore
