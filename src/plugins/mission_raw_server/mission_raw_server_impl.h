#pragma once

#include "plugins/mission_raw_server/mission_raw_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class MissionRawServerImpl : public PluginImplBase {
public:
    explicit MissionRawServerImpl(System& system);
    explicit MissionRawServerImpl(std::shared_ptr<System> system);
    ~MissionRawServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void subscribe_incoming_mission(MissionRawServer::IncomingMissionCallback callback);
    void subscribe_current_item_changed(MissionRawServer::CurrentItemChangedCallback callback);
    void subscribe_clear_all(MissionRawServer::ClearAllCallback callback);
    void set_current_item_complete_async(const MissionRawServer::ResultCallback callback);
    void set_current_item_complete() const;

    MissionRawServer::MissionPlan incoming_mission() const;
    MissionRawServer::MissionItem current_item_changed() const;
    uint32_t clear_all() const;

private:
    MissionRawServer::IncomingMissionCallback _incoming_mission_callback;
    MissionRawServer::CurrentItemChangedCallback _current_item_changed_callback;
    MissionRawServer::ClearAllCallback _clear_all_callback;
    std::thread _thread_mission;
    std::atomic<int> _target_component;
    std::atomic<int> _mission_count;
    std::atomic<bool> _mission_completed;

    std::queue<std::function<void()>> _work_queue;
    std::condition_variable _wait_for_new_task;
    std::mutex _work_mutex;
    std::atomic<bool> _stop_work_thread = false;

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
        MissionRawServer::ResultCallback result_callback{nullptr};
        MissionRawServer::IncomingMissionCallback download_mission_callback{nullptr};
        int last_current_reported_mission_item{-1};
        int last_total_reported_mission_item{-1};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_upload{};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_download{};
        bool gimbal_v2_in_control{false};
    } _mission_data{};

    // FIXME: make static
    std::pair<MissionRawServer::Result, MissionRawServer::MissionPlan>
    convert_to_result_and_mission_items(
        MAVLinkMissionTransfer::Result result,
        const std::vector<MAVLinkMissionTransfer::ItemInt>& int_items);

    void set_current_seq(int32_t seq);

    void add_task(std::function<void()> task)
    {
        std::unique_lock<std::mutex> lock(_work_mutex);
        _work_queue.push(task);
        _wait_for_new_task.notify_one();
    }

    bool _enable_return_to_launch_after_mission{false};

    // FIXME: This is hardcoded for now because it is urgently needed for 3DR with Yuneec H520.
    //        Ultimate it needs a setter.
    bool _enable_absolute_gimbal_yaw_angle{true};
};

} // namespace mavsdk