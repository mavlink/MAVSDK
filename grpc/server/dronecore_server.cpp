#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>

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
using dronecorerpc::Bool;
using dronecorerpc::Empty;
using dronecorerpc::DroneCoreRPC;

using namespace dronecore;
using namespace std::placeholders;

enum class MissionState : unsigned {
    INIT = 0,
    UPLOADING,
    UPLOADING_DONE,
    ARMING,
    ARMING_DONE,
    STARTING,
    STARTING_DONE,
    MISSION,
    MISSION_PAUSING,
    PAUSE,
    MISSION_RESUMING,
    MISSION_DONE,
    RETURN,
    RETURN_DONE,
    DONE,
    ERROR
};

class DroneCoreRPCImpl final : public DroneCoreRPC::Service
{

    Status Arm(ServerContext *context, const Empty *request,
               dronecorerpc::ActionResult *response) override
    {
        Device &device = dc.device();
        const Action::Result action_result = device.action().arm();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status TakeOff(ServerContext *context, const Empty *request,
                   dronecorerpc::ActionResult *response) override
    {
        Device &device = dc.device();
        const Action::Result action_result = device.action().takeoff();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status Land(ServerContext *context, const Empty *request,
                dronecorerpc::ActionResult *response) override
    {
        Device &device = dc.device();
        const Action::Result action_result = device.action().land();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status AddMissionItem(ServerContext *context, const dronecorerpc::MissionItem *mission_item,
                          Bool *response) override
    {
        auto new_item = std::make_shared<MissionItem>();
        new_item->set_position(mission_item->latitude(), mission_item->longitude());
        new_item->set_relative_altitude(mission_item->altitude());
        new_item->set_speed(mission_item->speed());
        new_item->set_fly_through(mission_item->is_fly_through());
        new_item->set_gimbal_pitch_and_yaw(mission_item->pitch(), mission_item->yaw());
        new_item->set_camera_action(static_cast<dronecore::MissionItem::CameraAction>
                                    (mission_item->camera_action()));
        mission_items.push_back(new_item);
        response->set_ret(true);
        return Status::OK;
    }

    Status FlyMission(ServerContext *context, const Empty *request, Bool *response) override
    {
        Device &device = dc.device();
        finished = false;
        _mission_state = MissionState::INIT;
        while (!finished) {
            switch (_mission_state) {
                case MissionState::INIT:
                    device.mission().send_mission_async(
                        mission_items,
                        std::bind(&receive_send_mission_result, _1));
                    _mission_state = MissionState::UPLOADING;
                    break;
                case MissionState::UPLOADING:
                    break;
                case MissionState::UPLOADING_DONE:
                    std::cout << "arming!" << std::endl;
                    if (!device.telemetry().armed()) {
                        device.action().arm_async(std::bind(&receive_arm_result, _1));
                        _mission_state = MissionState::ARMING;
                    }
                    break;
                case MissionState::ARMING:
                    break;
                case MissionState::ARMING_DONE:
                    // TODO: There can be a race here if PX4 still listens to the armed flag in
                    // the message DO_SET_MODE. Once it ignores it as in the spec, this is not
                    // needed anymore.
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    device.mission().start_mission_async(
                        std::bind(&receive_start_mission_result, _1));
                    _mission_state = MissionState::STARTING;
                    break;
                case MissionState::STARTING:
                    break;
                case MissionState::STARTING_DONE:
                    _mission_state = MissionState::MISSION;
                    device.mission().subscribe_progress(
                        std::bind(&receive_mission_progress, _1, _2));
                    break;
                case MissionState::MISSION_DONE:
                    device.action().return_to_launch_async(
                        std::bind(&receive_return_to_launch_result, _1));
                    _mission_state = MissionState::RETURN;
                    break;
                case MissionState::RETURN:
                    if (!device.telemetry().in_air()) {
                        _mission_state = MissionState::RETURN_DONE;
                    }
                    break;
                case MissionState::RETURN_DONE:
                    device.action().disarm_async(std::bind(&receive_disarm_result, _1));
                    break;
                case MissionState::DONE:
                    finished = true;
                    break;
                case MissionState::ERROR:
                    finished = true;
                    break;

                case MissionState::MISSION:
                // FALLTHROUGH
                case MissionState::MISSION_PAUSING:
                // FALLTHROUGH
                case MissionState::PAUSE:
                // FALLTHROUGH
                case MissionState::MISSION_RESUMING:
                // FALLTHROUGH
                default:
                    // not handled
                    break;

            }
        }
        if (_mission_state == MissionState::DONE) {
            response->set_ret(true);
        } else {
            response->set_ret(false);
        }
        return Status::OK;
    }

public:
    DroneCore dc;
    std::vector<std::shared_ptr<MissionItem>> mission_items;
    static MissionState _mission_state;
    bool finished;

    static void receive_send_mission_result(Mission::Result result)
    {

        if (result == Mission::Result::SUCCESS) {
            _mission_state = MissionState::UPLOADING_DONE;
        } else {
            std::cerr << "Error: mission send result: " << unsigned(result) << std::endl;
            _mission_state = MissionState::ERROR;
        }
    }

    static void receive_start_mission_result(Mission::Result result)
    {

        if (result == Mission::Result::SUCCESS) {
            _mission_state = MissionState::STARTING_DONE;
        } else {
            std::cerr << "Error: mission start result: " << unsigned(result) << std::endl;
            _mission_state = MissionState::ERROR;
        }
    }

    static void receive_mission_progress(int current, int total)
    {
        std::cout << "Mission status update: " << current << " / " << total << std::endl;

        if (current > 0 && current == total) {
            _mission_state = MissionState::MISSION_DONE;
        }
    }

    static void receive_arm_result(Action::Result result)
    {

        if (result == Action::Result::SUCCESS) {
            _mission_state = MissionState::ARMING_DONE;
        } else {
            std::cerr << "Error: arming result: " << unsigned(result) << std::endl;
            _mission_state = MissionState::ERROR;
        }
    }

    static void receive_return_to_launch_result(Action::Result result)
    {

        if (result == Action::Result::SUCCESS) {
        } else {
            std::cerr << "Error: return to land result: " << unsigned(result) << std::endl;
            _mission_state = MissionState::ERROR;
        }
    }


    static void receive_disarm_result(Action::Result result)
    {

        if (result == Action::Result::SUCCESS) {
        } else {
            std::cerr << "Error: disarming result: " << unsigned(result) << std::endl;
        }

        _mission_state = MissionState::DONE;
    }
};

MissionState DroneCoreRPCImpl::_mission_state;

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
