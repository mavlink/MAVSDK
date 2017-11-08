#include "mission.h"
#include "mission.grpc.pb.h"
#include <google/protobuf/empty.pb.h>

using grpc::Status;
using grpc::ServerContext;
using dronecorerpc::MissionRPC;

using namespace dronecore;

class MissionRPCImpl final : public MissionRPC::Service
{
public:
    MissionRPCImpl(DroneCore *dc_obj)
    {
        dc = dc_obj;
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

        dc->device().mission().upload_mission_async(
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

    Status StartMission(ServerContext *context, const ::google::protobuf::Empty *request,
                        dronecorerpc::MissionResult *response) override
    {
        // TODO: there has to be a beter way than using std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        dc->device().mission().start_mission_async(
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
    DroneCore *dc;
};
