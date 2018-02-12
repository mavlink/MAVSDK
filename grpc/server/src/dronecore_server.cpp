#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstdint>
#include <future>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "dronecore.h"
#include "log.h"
#include "action/actionrpc_impl.h"
#include "core/core.grpc.pb.h"
#include "core/corerpc_impl.h"
#include "mission/missionrpc_impl.h"
#include "telemetry/telemetryrpc_impl.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::Status;

using namespace dronecore;
using namespace std::placeholders;

static DroneCore dc;

template<typename T> ::grpc::Service *createInstances(DroneCore *dc_obj) { return new T(dc_obj); }

int RunServer()
{
    ConnectionResult connection_result = dc.add_udp_connection(14540);

    if (connection_result != ConnectionResult::SUCCESS) {
        LogErr() << "Connection failed: " << connection_result_str(connection_result);
        return 1;
    }

    LogInfo() << "Waiting to discover device...";
    dc.register_on_discover([](uint64_t uuid) {
        LogInfo() << "Device discovered [UUID: " << uuid << "]";
    });
    dc.register_on_timeout([](uint64_t uuid) {
        LogInfo() << "Device timed out [UUID: " << uuid << "]";
    });

    std::string server_address("0.0.0.0:50051");

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    CoreServiceImpl service(&dc);
    builder.RegisterService(&service);
    builder.RegisterService(createInstances<ActionServiceImpl>(&dc));
    builder.RegisterService(createInstances<TelemetryServiceImpl>(&dc));
    builder.RegisterService(createInstances<MissionServiceImpl>(&dc));

    std::unique_ptr<Server> server(builder.BuildAndStart());
    LogInfo() << "Server listening on " << server_address;
    server->Wait();
}

int main(int argc, char **argv)
{
    return RunServer();
}
