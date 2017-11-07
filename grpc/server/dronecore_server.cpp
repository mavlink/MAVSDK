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
#include "dronecore.grpc.pb.h"
#include "action/actionrpc_impl.h"
#include "mission/missionrpc_impl.h"
#include "telemetry/telemetryrpc_impl.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::Status;
using dronecorerpc::DroneCoreRPC;

using namespace dronecore;
using namespace std::placeholders;

static DroneCore dc;

template<typename T> ::grpc::Service *createInstances(DroneCore *dc_obj) {return new T(dc_obj);}

typedef std::map<std::string, ::grpc::Service*(*)(DroneCore *dc_obj)> map_type;

class DroneCoreRPCImpl final : public DroneCoreRPC::Service
{

public:

};

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    DroneCoreRPCImpl service;

    map_type map;
    map["action"] = &createInstances<ActionRPCImpl>;
    map["mission"] = &createInstances<MissionRPCImpl>;
    map["telemetry"] = &createInstances<TelemetryRPCImpl>;

    std::string plugin;
    std::fstream file;
    file.open("../proto/plugins.conf");
    std::vector<::grpc::Service *> list;
    while (file >> plugin) {
        auto service_obj = map[plugin](&dc);
        list.push_back(service_obj);
    }

    bool discovered_device = false;
    DroneCore::ConnectionResult connection_result = dc.add_udp_connection();
    if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
        std::cout << "Connection failed: " << DroneCore::connection_result_str(
                      connection_result) << std::endl;
        return;
    }
    std::cout << "Waiting to discover device..." << std::endl;
    dc.register_on_discover([&discovered_device](uint64_t uuid) {
        std::cout << "Discovered device with UUID: " << uuid << std::endl;
        discovered_device = true;
    });
    // We usually receive heartbeats at 1Hz, therefore we should find a device after around 2 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (!discovered_device) {
        std::cout << "No device found, exiting." << std::endl;
        return;
    }

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    for (auto service : list) {
        builder.RegisterService(service);
    }
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv)
{
    RunServer();
    return 0;
}
