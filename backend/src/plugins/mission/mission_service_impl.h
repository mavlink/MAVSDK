#include <future>

#include <memory>
#include <vector>

#include "mission/mission.h"
#include "mission/mission.grpc.pb.h"
#include "mission/mission_item.h"

namespace dronecore {
namespace backend {

template <typename Mission = Mission>
class MissionServiceImpl final : public dronecore::rpc::mission::MissionService::Service
{
public:
    MissionServiceImpl(Mission &mission)
        : _mission(mission) {}

    grpc::Status UploadMission(grpc::ServerContext * /* context */,
                               const rpc::mission::UploadMissionRequest *request,
                               rpc::mission::UploadMissionResponse *response) override
    {
        std::promise<void> result_promise;
        const auto result_future = result_promise.get_future();

        const auto mission_items = extractMissionItems(request);
        uploadMissionItems(mission_items, response, result_promise);

        result_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status StartMission(grpc::ServerContext * /* context */,
                              const rpc::mission::StartMissionRequest * /* request */,
                              rpc::mission::StartMissionResponse *response) override
    {
        std::promise<void> result_promise;
        const auto result_future = result_promise.get_future();

        _mission.start_mission_async([this, response,
              &result_promise](const dronecore::Mission::Result result) {
            if (response != nullptr) {
                auto rpc_mission_result = generateRPCMissionResult(result);
                response->set_allocated_mission_result(rpc_mission_result);
            }

            result_promise.set_value();
        });

        result_future.wait();
        return grpc::Status::OK;
    }

private:
    std::vector<std::shared_ptr<MissionItem>> extractMissionItems(const
                                                                  rpc::mission::UploadMissionRequest *request) const
    {
        std::vector<std::shared_ptr<MissionItem>> mission_items;

        if (request != nullptr) {
            for (auto rpc_mission_item : request->mission().mission_item()) {
                mission_items.push_back(translateRPCMissionItem(rpc_mission_item));
            }
        }

        return mission_items;
    }

    std::shared_ptr<MissionItem>
    translateRPCMissionItem(const rpc::mission::MissionItem &rpc_mission_item) const
    {
        auto mission_item = std::make_shared<MissionItem>();
        mission_item->set_position(rpc_mission_item.latitude_deg(), rpc_mission_item.longitude_deg());
        mission_item->set_relative_altitude(rpc_mission_item.relative_altitude_m());
        mission_item->set_speed(rpc_mission_item.speed_m_s());
        mission_item->set_fly_through(rpc_mission_item.is_fly_through());
        mission_item->set_gimbal_pitch_and_yaw(rpc_mission_item.gimbal_pitch_deg(),
                                               rpc_mission_item.gimbal_yaw_deg());
        mission_item->set_camera_action(translateRPCCameraAction(rpc_mission_item.camera_action()));

        return mission_item;
    }

    MissionItem::CameraAction
    translateRPCCameraAction(const rpc::mission::MissionItem::CameraAction rpc_camera_action) const
    {
        return static_cast<MissionItem::CameraAction>(rpc_camera_action);
    }

    void uploadMissionItems(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                            rpc::mission::UploadMissionResponse *response,
                            std::promise<void> &result_promise) const
    {
        _mission.upload_mission_async(mission_items, [this, response,
              &result_promise](const dronecore::Mission::Result result) {
            if (response != nullptr) {
                auto rpc_mission_result = generateRPCMissionResult(result);
                response->set_allocated_mission_result(rpc_mission_result);
            }

            result_promise.set_value();
        });
    }

    rpc::mission::MissionResult *generateRPCMissionResult(const dronecore::Mission::Result result) const
    {
        auto rpc_result = static_cast<rpc::mission::MissionResult::Result>(result);

        auto rpc_mission_result = new rpc::mission::MissionResult();
        rpc_mission_result->set_result(rpc_result);
        rpc_mission_result->set_result_str(dronecore::Mission::result_str(result));

        return rpc_mission_result;
    }

    Mission &_mission;
};

} // namespace backend
} // namespace dronecore
