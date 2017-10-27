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
#include "action.h"
#include "action.grpc.pb.h"
#include "mission.h"
#include "mission.grpc.pb.h"
#include "telemetry.h"
#include "telemetry.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using dronecorerpc::Empty;
using dronecorerpc::ActionEmpty;
using dronecorerpc::MissionEmpty;
using dronecorerpc::TelemetryEmpty;
using dronecorerpc::DroneCoreRPC;
using dronecorerpc::ActionRPC;
using dronecorerpc::MissionRPC;
using dronecorerpc::TelemetryRPC;

using namespace dronecore;
using namespace std::placeholders;

static DroneCore dc;

template<typename T> ::grpc::Service *createInstances() {return new T;}

typedef std::map<std::string, ::grpc::Service*(*)()> map_type;

class DroneCoreRPCImpl final : public DroneCoreRPC::Service
{

public:

};

class ActionRPCImpl final : public ActionRPC::Service
{
public:
    Status Arm(ServerContext *context, const ActionEmpty *request,
               dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc.device().action().arm();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status TakeOff(ServerContext *context, const ActionEmpty *request,
                   dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc.device().action().takeoff();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status Land(ServerContext *context, const ActionEmpty *request,
                dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc.device().action().land();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }
};

class MissionRPCImpl final : public MissionRPC::Service
{
public:
    Status SendMission(ServerContext *context, const dronecorerpc::Mission *mission,
                       dronecorerpc::MissionResult *response) override
    {
        // TODO: there has to be a beter way than using std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        std::vector<std::shared_ptr<MissionItem>> mission_items;
        for (auto &mission_item : mission->mission_items()) {
            auto new_item = std::make_shared<MissionItem>();
            new_item->set_position(mission_item.latitude_deg(), mission_item.longitude_deg());
            new_item->set_relative_altitude(mission_item.relative_altitude_m());
            new_item->set_speed(mission_item.speed_m_s());
            new_item->set_fly_through(mission_item.is_fly_through());
            new_item->set_gimbal_pitch_and_yaw(mission_item.gimbal_pitch_deg(),
                                               mission_item.gimbal_yaw_deg());
            new_item->set_camera_action(static_cast<dronecore::MissionItem::CameraAction>
                                        (mission_item.camera_action()));
            mission_items.push_back(new_item);
        }

        dc.device().mission().upload_mission_async(
        mission_items, [prom, response](Mission::Result result) {
            response->set_result(static_cast<dronecorerpc::MissionResult::Result>(result));
            response->set_result_str(Mission::result_str(result));
            prom->set_value();
        }
        );

        future_result.wait();
        future_result.get();
        return Status::OK;
    }

    Status StartMission(ServerContext *context, const MissionEmpty *request,
                        dronecorerpc::MissionResult *response) override
    {
        // TODO: there has to be a beter way than using std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        dc.device().mission().start_mission_async(
        [prom, response](Mission::Result result) {
            response->set_result(static_cast<dronecorerpc::MissionResult::Result>(result));
            response->set_result_str(Mission::result_str(result));
            prom->set_value();
        }
        );

        future_result.wait();
        future_result.get();
        return Status::OK;
    }

private:
};

class TelemetryRPCImpl final : public TelemetryRPC::Service
{

    Status TelemetryPositionSubscription(ServerContext *context, const TelemetryEmpty *request,
                                         ServerWriter<dronecorerpc::TelemetryPosition> *writer) override
    {
        dc.device().telemetry().position_async([&writer](Telemetry::Position position) {
            dronecorerpc::TelemetryPosition rpc_position;
            rpc_position.set_latitude_deg(position.latitude_deg);
            rpc_position.set_longitude_deg(position.longitude_deg);
            rpc_position.set_relative_altitude_m(position.relative_altitude_m);
            rpc_position.set_absolute_altitude_m(position.absolute_altitude_m);
            writer->Write(rpc_position);
        });
        // TODO: This is probably not the best idea.
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return Status::OK;
    }
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
        auto service_obj = map[plugin]();
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
