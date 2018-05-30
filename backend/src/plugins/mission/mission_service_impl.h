#include <future>

#include <memory>
#include <vector>

#include "mission/mission.h"
#include "mission/mission.grpc.pb.h"
#include "mission/mission_item.h"

namespace dronecore {
namespace backend {

template<typename Mission = Mission>
class MissionServiceImpl final : public dronecore::rpc::mission::MissionService::Service {
public:
    MissionServiceImpl(Mission &mission) : _mission(mission) {}

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

    grpc::Status DownloadMission(grpc::ServerContext * /* context */,
                                 const rpc::mission::DownloadMissionRequest * /* request */,
                                 rpc::mission::DownloadMissionResponse *response) override
    {
        _mission.download_mission_async(
            [this, response](const dronecore::Mission::Result result,
                             const std::vector<std::shared_ptr<MissionItem>> mission_items) {
                if (response != nullptr) {
                    auto rpc_mission_result = generateRPCMissionResult(result);
                    response->set_allocated_mission_result(rpc_mission_result);

                    auto rpc_mission = new rpc::mission::Mission();

                    for (const auto mission_item : mission_items) {
                        auto rpc_mission_item = rpc_mission->add_mission_item();
                        translateMissionItem(mission_item, rpc_mission_item);
                    }

                    response->set_allocated_mission(rpc_mission);
                }
            });

        return grpc::Status::OK;
    }

    grpc::Status StartMission(grpc::ServerContext * /* context */,
                              const rpc::mission::StartMissionRequest * /* request */,
                              rpc::mission::StartMissionResponse *response) override
    {
        std::promise<void> result_promise;
        const auto result_future = result_promise.get_future();

        _mission.start_mission_async(
            [this, response, &result_promise](const dronecore::Mission::Result result) {
                if (response != nullptr) {
                    auto rpc_mission_result = generateRPCMissionResult(result);
                    response->set_allocated_mission_result(rpc_mission_result);
                }

                result_promise.set_value();
            });

        result_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status IsMissionFinished(grpc::ServerContext * /* context */,
                                   const rpc::mission::IsMissionFinishedRequest * /* request */,
                                   rpc::mission::IsMissionFinishedResponse *response) override
    {
        if (response != nullptr) {
            auto is_mission_finished = _mission.mission_finished();
            response->set_is_finished(is_mission_finished);
        }

        return grpc::Status::OK;
    }

    grpc::Status PauseMission(grpc::ServerContext * /* context */,
                              const rpc::mission::PauseMissionRequest * /* request */,
                              rpc::mission::PauseMissionResponse *response) override
    {
        std::promise<void> result_promise;
        const auto result_future = result_promise.get_future();

        _mission.pause_mission_async(
            [this, response, &result_promise](const dronecore::Mission::Result result) {
                if (response != nullptr) {
                    auto rpc_mission_result = generateRPCMissionResult(result);
                    response->set_allocated_mission_result(rpc_mission_result);
                }

                result_promise.set_value();
            });

        result_future.wait();
        return grpc::Status::OK;
    }

    static void translateMissionItem(const std::shared_ptr<MissionItem> mission_item,
                                     rpc::mission::MissionItem *rpc_mission_item)
    {
        rpc_mission_item->set_latitude_deg(mission_item->get_latitude_deg());
        rpc_mission_item->set_longitude_deg(mission_item->get_longitude_deg());
        rpc_mission_item->set_relative_altitude_m(mission_item->get_relative_altitude_m());
        rpc_mission_item->set_speed_m_s(mission_item->get_speed_m_s());
        rpc_mission_item->set_is_fly_through(mission_item->get_fly_through());
        rpc_mission_item->set_gimbal_pitch_deg(mission_item->get_gimbal_pitch_deg());
        rpc_mission_item->set_gimbal_yaw_deg(mission_item->get_gimbal_yaw_deg());
        rpc_mission_item->set_camera_action(
            translateCameraAction(mission_item->get_camera_action()));
    }

    static rpc::mission::MissionItem::CameraAction
    translateCameraAction(const MissionItem::CameraAction camera_action)
    {
        switch (camera_action) {
            case MissionItem::CameraAction::TAKE_PHOTO:
                return rpc::mission::MissionItem_CameraAction_TAKE_PHOTO;
            case MissionItem::CameraAction::START_PHOTO_INTERVAL:
                return rpc::mission::MissionItem_CameraAction_START_PHOTO_INTERVAL;
            case MissionItem::CameraAction::STOP_PHOTO_INTERVAL:
                return rpc::mission::MissionItem_CameraAction_STOP_PHOTO_INTERVAL;
            case MissionItem::CameraAction::START_VIDEO:
                return rpc::mission::MissionItem_CameraAction_START_VIDEO;
            case MissionItem::CameraAction::STOP_VIDEO:
                return rpc::mission::MissionItem_CameraAction_STOP_VIDEO;
            case MissionItem::CameraAction::NONE:
            default:
                return rpc::mission::MissionItem_CameraAction_NONE;
        }
    }

    static std::shared_ptr<MissionItem>
    translateRPCMissionItem(const rpc::mission::MissionItem &rpc_mission_item)
    {
        auto mission_item = std::make_shared<MissionItem>();
        mission_item->set_position(rpc_mission_item.latitude_deg(),
                                   rpc_mission_item.longitude_deg());
        mission_item->set_relative_altitude(rpc_mission_item.relative_altitude_m());
        mission_item->set_speed(rpc_mission_item.speed_m_s());
        mission_item->set_fly_through(rpc_mission_item.is_fly_through());
        mission_item->set_gimbal_pitch_and_yaw(rpc_mission_item.gimbal_pitch_deg(),
                                               rpc_mission_item.gimbal_yaw_deg());
        mission_item->set_camera_action(translateRPCCameraAction(rpc_mission_item.camera_action()));

        return mission_item;
    }

    static MissionItem::CameraAction
    translateRPCCameraAction(const rpc::mission::MissionItem::CameraAction rpc_camera_action)
    {
        switch (rpc_camera_action) {
            case rpc::mission::MissionItem::CameraAction::MissionItem_CameraAction_TAKE_PHOTO:
                return MissionItem::CameraAction::TAKE_PHOTO;
            case rpc::mission::MissionItem::CameraAction::
                MissionItem_CameraAction_START_PHOTO_INTERVAL:
                return MissionItem::CameraAction::START_PHOTO_INTERVAL;
            case rpc::mission::MissionItem::CameraAction::
                MissionItem_CameraAction_STOP_PHOTO_INTERVAL:
                return MissionItem::CameraAction::STOP_PHOTO_INTERVAL;
            case rpc::mission::MissionItem::CameraAction::MissionItem_CameraAction_START_VIDEO:
                return MissionItem::CameraAction::START_VIDEO;
            case rpc::mission::MissionItem::CameraAction::MissionItem_CameraAction_STOP_VIDEO:
                return MissionItem::CameraAction::STOP_VIDEO;
            case rpc::mission::MissionItem::CameraAction::MissionItem_CameraAction_NONE:
            default:
                return MissionItem::CameraAction::NONE;
        }
    }

private:
    std::vector<std::shared_ptr<MissionItem>>
    extractMissionItems(const rpc::mission::UploadMissionRequest *request) const
    {
        std::vector<std::shared_ptr<MissionItem>> mission_items;

        if (request != nullptr) {
            for (auto rpc_mission_item : request->mission().mission_item()) {
                mission_items.push_back(translateRPCMissionItem(rpc_mission_item));
            }
        }

        return mission_items;
    }

    void uploadMissionItems(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                            rpc::mission::UploadMissionResponse *response,
                            std::promise<void> &result_promise) const
    {
        _mission.upload_mission_async(
            mission_items,
            [this, response, &result_promise](const dronecore::Mission::Result result) {
                if (response != nullptr) {
                    auto rpc_mission_result = generateRPCMissionResult(result);
                    response->set_allocated_mission_result(rpc_mission_result);
                }

                result_promise.set_value();
            });
    }

    rpc::mission::MissionResult *
    generateRPCMissionResult(const dronecore::Mission::Result result) const
    {
        auto rpc_result = static_cast<rpc::mission::MissionResult::Result>(result);

        auto rpc_mission_result = new rpc::mission::MissionResult();
        rpc_mission_result->set_result(rpc_result);
        rpc_mission_result->set_result_str(dronecore::Mission::result_str(result));

        return rpc_mission_result;
    }

    grpc::Status SubscribeMissionProgress(grpc::ServerContext * /* context */,
                                          const dronecore::rpc::mission::SubscribeMissionProgressRequest * /* request */,
                                          grpc::ServerWriter<rpc::mission::MissionProgressResponse> *writer) override
    {
        std::promise<void> mission_finished_promise;
        auto mission_finished_future = mission_finished_promise.get_future();

        _mission.subscribe_progress([&writer, &mission_finished_promise](int current, int total) {
            dronecore::rpc::mission::MissionProgressResponse rpc_mission_progress_response;
            rpc_mission_progress_response.set_current_item_index(current);
            rpc_mission_progress_response.set_mission_count(total);
            writer->Write(rpc_mission_progress_response);

            if (current == total - 1) {
                mission_finished_promise.set_value();
            }
        });

        mission_finished_future.wait();
        return grpc::Status::OK;
    }

    Mission &_mission;
};

} // namespace backend
} // namespace dronecore
