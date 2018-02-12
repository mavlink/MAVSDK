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

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    CoreServiceImpl service(&dc);

    int discovered_device = 0;
    ConnectionResult connection_result = dc.add_udp_connection(14550);
    if (connection_result != ConnectionResult::SUCCESS) {
        LogErr() << "Connection failed: " << connection_result_str(connection_result);
        return;
    }
    LogInfo() << "Waiting to discover device...";
    dc.register_on_discover([&discovered_device](uint64_t uuid) {
        LogInfo() << "Discovered device with UUID: " << uuid;
        discovered_device += 1;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a device after around 2 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(10));
    if (discovered_device == 0) {
        LogErr() << "No device found, exiting.";
        return;
    }

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
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
    RunServer();
    return 0;
}
