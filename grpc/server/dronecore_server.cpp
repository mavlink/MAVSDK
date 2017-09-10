#include <iostream>
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

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using dronecorerpc::Empty;
using dronecorerpc::DroneCoreRPC;

using namespace dronecore;
using namespace std::placeholders;

class DroneCoreRPCImpl final : public DroneCoreRPC::Service
{

public:
    Status Arm(ServerContext *context, const Empty *request,
               dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc.device().action().arm();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status TakeOff(ServerContext *context, const Empty *request,
                   dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc.device().action().takeoff();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status Land(ServerContext *context, const Empty *request,
                dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc.device().action().land();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status SendMission(ServerContext *context, const dronecorerpc::Mission *mission,
                       dronecorerpc::MissionResult *response) override
    {
        // TODO: there has to be a beter way than using std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        std::vector<std::shared_ptr<MissionItem>> mission_items;
        for (auto &mission_item : mission->mission_items()) {
            auto new_item = std::make_shared<MissionItem>();
            new_item->set_position(mission_item.latitude(), mission_item.longitude());
            new_item->set_relative_altitude(mission_item.altitude());
            new_item->set_speed(mission_item.speed());
            new_item->set_fly_through(mission_item.is_fly_through());
            new_item->set_gimbal_pitch_and_yaw(mission_item.pitch(), mission_item.yaw());
            new_item->set_camera_action(static_cast<dronecore::MissionItem::CameraAction>
                                        (mission_item.camera_action()));
            mission_items.push_back(new_item);
        }

        dc.device().mission().send_mission_async(
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

    Status StartMission(ServerContext *context, const Empty *request,
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

    DroneCore dc;

private:
};

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    DroneCoreRPCImpl service;

    bool discovered_device = false;
    DroneCore::ConnectionResult connection_result = service.dc.add_udp_connection();
    if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
        std::cout << "Connection failed: " << DroneCore::connection_result_str(
                      connection_result) << std::endl;
        return;
    }
    std::cout << "Waiting to discover device..." << std::endl;
    service.dc.register_on_discover([&discovered_device](uint64_t uuid) {
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
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv)
{
    RunServer();
    return 0;
}
