#pragma once

#include <json/json.h>
#include <map>
#include <memory>
#include <mutex>

#include "mavlink_include.h"
#include "plugins/mission/mission.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "mavlink_mission_transfer.h"

namespace mavsdk {

class MissionImpl : public PluginImplBase {
public:
    MissionImpl(System& system);
    ~MissionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Mission::Result upload_mission(const Mission::MissionPlan& mission_plan);

    void upload_mission_async(
        const Mission::MissionPlan& mission_plan, const Mission::result_callback_t& callback);

    void cancel_mission_upload_async(const Mission::result_callback_t callback);
    Mission::Result cancel_mission_upload();

    std::pair<Mission::Result, Mission::MissionPlan> download_mission();
    void download_mission_async(const Mission::download_mission_callback_t& callback);

    Mission::Result cancel_mission_download();
    void cancel_mission_download_async(const Mission::result_callback_t& callback);

    Mission::Result set_return_to_launch_after_mission(bool enable_rtl);

    std::pair<Mission::Result, bool> get_return_to_launch_after_mission();

    Mission::Result start_mission();
    void start_mission_async(const Mission::result_callback_t& callback);

    Mission::Result pause_mission();
    void pause_mission_async(const Mission::result_callback_t& callback);

    Mission::Result clear_mission();
    void clear_mission_async(const Mission::result_callback_t& callback);

    Mission::Result set_current_mission_item(int index);
    void set_current_mission_item_async(int current, const Mission::result_callback_t& callback);

    std::pair<Mission::Result, bool> is_mission_finished() const;

    int current_mission_item() const;
    int total_mission_items() const;

    Mission::MissionProgress mission_progress();
    void mission_progress_async(Mission::mission_progress_callback_t callback);

    void import_qgroundcontrol_mission_async(
        std::string qgc_plan_path,
        const Mission::import_qgroundcontrol_mission_callback_t callback);

    static std::pair<Mission::Result, Mission::MissionPlan>
    import_qgroundcontrol_mission(const std::string& qgc_plan_path);

    // Non-copyable
    MissionImpl(const MissionImpl&) = delete;
    const MissionImpl& operator=(const MissionImpl&) = delete;

private:
    void process_mission_current(const mavlink_message_t& message);
    void process_mission_item_reached(const mavlink_message_t& message);

    static bool has_valid_position(const Mission::MissionItem& item);
    static float hold_time(const Mission::MissionItem& item);
    static float acceptance_radius(const Mission::MissionItem& item);

    std::vector<MAVLinkMissionTransfer::ItemInt>
    convert_to_int_items(const std::vector<Mission::MissionItem>& mission_items);

    void report_progress();
    void reset_mission_progress();

    void
    report_flight_mode_change(Mission::result_callback_t callback, MAVLinkCommands::Result result);
    static Mission::Result command_result_to_mission_result(MAVLinkCommands::Result result);

    // FIXME: make static
    std::pair<Mission::Result, Mission::MissionPlan> convert_to_result_and_mission_items(
        MAVLinkMissionTransfer::Result result,
        const std::vector<MAVLinkMissionTransfer::ItemInt>& int_items);

    static Mission::Result convert_result(MAVLinkMissionTransfer::Result result);

    static Mission::Result import_mission_items(
        std::vector<Mission::MissionItem>& all_mission_items, const Json::Value& qgc_plan_json);

    static Mission::Result build_mission_items(
        MAV_CMD command,
        std::vector<double> params,
        Mission::MissionItem& new_mission_item,
        std::vector<Mission::MissionItem>& all_mission_items);

    struct MissionData {
        mutable std::recursive_mutex mutex{};
        int last_current_mavlink_mission_item{-1};
        int last_reached_mavlink_mission_item{-1};
        std::map<int, int> mavlink_mission_item_to_mission_item_indices{};
        int num_mission_items_to_download{-1};
        int next_mission_item_to_download{-1};
        int last_mission_item_to_upload{-1};
        Mission::result_callback_t result_callback{nullptr};
        Mission::download_mission_callback_t download_mission_callback{nullptr};
        Mission::mission_progress_callback_t mission_progress_callback{nullptr};
        int last_current_reported_mission_item{-1};
        int last_total_reported_mission_item{-1};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_upload{};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_download{};
    } _mission_data{};

    void* _timeout_cookie{nullptr};

    bool _enable_return_to_launch_after_mission{false};

    // FIXME: This is hardcoded for now because it is urgently needed for 3DR with Yuneec H520.
    //        Ultimate it needs a setter.
    bool _enable_absolute_gimbal_yaw_angle{true};

    static constexpr unsigned MAX_RETRIES = 10;

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;

    // FIXME: these chould potentially change anytime
    // derived from:
    // https://github.com/PX4/Firmware/blob/master/src/modules/commander/px4_custom_mode.h
    static constexpr uint8_t PX4_CUSTOM_MAIN_MODE_AUTO = 4;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_LOITER = 3;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_MISSION = 4;

    static constexpr double RETRY_TIMEOUT_S = 0.250;
};

} // namespace mavsdk
