#pragma once

#include <atomic>
#include <vector>
#include <memory>
#include <mutex>

#include "mavlink_include.h"
#include "plugins/mission/mission.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "mavlink_mission_transfer_client.h"
#include "callback_list.h"

namespace mavsdk {

class MissionImpl : public PluginImplBase {
public:
    explicit MissionImpl(System& system);
    explicit MissionImpl(std::shared_ptr<System> system);
    ~MissionImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Mission::Result upload_mission(const Mission::MissionPlan& mission_plan);

    void upload_mission_async(
        const Mission::MissionPlan& mission_plan, const Mission::ResultCallback& callback);
    void upload_mission_with_progress_async(
        const Mission::MissionPlan& mission_plan,
        const Mission::UploadMissionWithProgressCallback callback);

    Mission::Result cancel_mission_upload() const;

    std::pair<Mission::Result, Mission::MissionPlan> download_mission();
    void download_mission_async(const Mission::DownloadMissionCallback& callback);
    void download_mission_with_progress_async(
        const Mission::DownloadMissionWithProgressCallback callback);

    Mission::Result cancel_mission_download() const;

    Mission::Result set_return_to_launch_after_mission(bool enable_rtl);

    std::pair<Mission::Result, bool> get_return_to_launch_after_mission();

    Mission::Result start_mission();
    void start_mission_async(const Mission::ResultCallback& callback);

    Mission::Result pause_mission();
    void pause_mission_async(const Mission::ResultCallback& callback);

    Mission::Result clear_mission();
    void clear_mission_async(const Mission::ResultCallback& callback);

    Mission::Result set_current_mission_item(int index);
    void set_current_mission_item_async(int current, const Mission::ResultCallback& callback);

    std::pair<Mission::Result, bool> is_mission_finished() const;

    int current_mission_item() const;
    int total_mission_items() const;

    Mission::MissionProgress mission_progress();
    Mission::MissionProgressHandle
    subscribe_mission_progress(const Mission::MissionProgressCallback& callback);
    void unsubscribe_mission_progress(Mission::MissionProgressHandle handle);

    // Non-copyable
    MissionImpl(const MissionImpl&) = delete;
    const MissionImpl& operator=(const MissionImpl&) = delete;

private:
    int current_mission_item_locked() const;
    int total_mission_items_locked() const;
    std::pair<Mission::Result, bool> is_mission_finished_locked() const;

    void process_mission_current(const mavlink_message_t& message);
    void process_mission_item_reached(const mavlink_message_t& message);
    void process_gimbal_manager_information(const mavlink_message_t& message);
    void receive_protocol_timeout();
    void wait_for_protocol();
    void wait_for_protocol_async(std::function<void()> callback);

    static bool has_valid_position(const Mission::MissionItem& item);
    static float hold_time(const Mission::MissionItem& item);
    static float acceptance_radius(const Mission::MissionItem& item);

    std::vector<MavlinkMissionTransferClient::ItemInt>
    convert_to_int_items(const std::vector<Mission::MissionItem>& mission_items);

    void report_progress_locked();
    void reset_mission_progress();

    void report_flight_mode_change(
        Mission::ResultCallback callback, MavlinkCommandSender::Result result);
    static Mission::Result command_result_to_mission_result(MavlinkCommandSender::Result result);

    // FIXME: make static
    std::pair<Mission::Result, Mission::MissionPlan> convert_to_result_and_mission_items(
        MavlinkMissionTransferClient::Result result,
        const std::vector<MavlinkMissionTransferClient::ItemInt>& int_items);

    static Mission::Result convert_result(MavlinkMissionTransferClient::Result result);

    void add_gimbal_items_v1(
        std::vector<MavlinkMissionTransferClient::ItemInt>& int_items,
        unsigned item_i,
        float pitch_deg,
        float yaw_deg);
    void add_gimbal_items_v2(
        std::vector<MavlinkMissionTransferClient::ItemInt>& int_items,
        unsigned item_i,
        float pitch_deg,
        float yaw_deg);

    void acquire_gimbal_control_v2(
        std::vector<MavlinkMissionTransferClient::ItemInt>& int_items, unsigned item_i);

    void release_gimbal_control_v2(
        std::vector<MavlinkMissionTransferClient::ItemInt>& int_items, unsigned item_i);

    struct MissionData {
        mutable std::mutex mutex{};
        int last_current_mavlink_mission_item{-1};
        int last_reached_mavlink_mission_item{-1};
        std::vector<int> mavlink_mission_item_to_mission_item_indices{};
        CallbackList<Mission::MissionProgress> mission_progress_callbacks{};
        int last_current_reported_mission_item{-1};
        int last_total_reported_mission_item{-1};
        std::weak_ptr<MavlinkMissionTransferClient::WorkItem> last_upload{};
        std::weak_ptr<MavlinkMissionTransferClient::WorkItem> last_download{};
        bool gimbal_v2_in_control{false};
    } _mission_data{};

    TimeoutHandler::Cookie _timeout_cookie{};

    bool _enable_return_to_launch_after_mission{false};

    // FIXME: This is hardcoded for now because it is urgently needed for 3DR with Yuneec H520.
    //        Ultimate it needs a setter.
    bool _enable_absolute_gimbal_yaw_angle{true};

    TimeoutHandler::Cookie _gimbal_protocol_cookie{};
    enum class GimbalProtocol { Unknown, V1, V2 };
    std::atomic<GimbalProtocol> _gimbal_protocol{GimbalProtocol::Unknown};
};

} // namespace mavsdk
