#pragma once

#include "plugins/mission_server/mission_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class MissionServerImpl : public PluginImplBase {
public:
    explicit MissionServerImpl(System& system);
    explicit MissionServerImpl(std::shared_ptr<System> system);
    ~MissionServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void subscribe_incoming_mission(MissionServer::IncomingMissionCallback callback);
    void subscribe_current_item_changed(MissionServer::CurrentItemChangedCallback callback);
    void subscribe_clear_all(MissionServer::ClearAllCallback callback);
    void set_current_item_complete_async(const MissionServer::ResultCallback callback);
    void set_current_item_complete() const;

    MissionServer::MissionPlan incoming_mission() const;
    MissionServer::MissionItem current_item_changed() const;
    uint32_t clear_all() const;

private:
    MissionServer::IncomingMissionCallback _incoming_mission_callback;
    MissionServer::CurrentItemChangedCallback _current_item_changed_callback;
    MissionServer::ClearAllCallback _clear_all_callback;
    std::thread _thread_mission;
    std::atomic<int> _target_component;
    std::atomic<bool> _do_upload;
    std::atomic<int> _mission_count;
    std::atomic<bool> _mission_completed;

    std::vector<MAVLinkMissionTransfer::ItemInt> _current_mission;
    uint32_t _current_seq;

    struct MissionData {
        mutable std::recursive_mutex mutex{};
        int last_current_mavlink_mission_item{-1};
        int last_reached_mavlink_mission_item{-1};
        std::vector<int> mavlink_mission_item_to_mission_item_indices{};
        int num_mission_items_to_download{-1};
        int next_mission_item_to_download{-1};
        int last_mission_item_to_upload{-1};
        MissionServer::ResultCallback result_callback{nullptr};
        MissionServer::IncomingMissionCallback download_mission_callback{nullptr};
        int last_current_reported_mission_item{-1};
        int last_total_reported_mission_item{-1};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_upload{};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_download{};
        bool gimbal_v2_in_control{false};
    } _mission_data{};

    // FIXME: make static
    std::pair<MissionServer::Result, MissionServer::MissionPlan>
    convert_to_result_and_mission_items(
        MAVLinkMissionTransfer::Result result,
        const std::vector<MAVLinkMissionTransfer::ItemInt>& int_items);

    void set_current_seq(int32_t seq);

    bool _enable_return_to_launch_after_mission{false};

    // FIXME: This is hardcoded for now because it is urgently needed for 3DR with Yuneec H520.
    //        Ultimate it needs a setter.
    bool _enable_absolute_gimbal_yaw_angle{true};
};

} // namespace mavsdk