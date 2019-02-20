#pragma once

#include <memory>
#include <map>
#include <mutex>

#include "system.h"
#include "mavlink_include.h"
#include "plugins/mission/mission.h"
#include "plugin_impl_base.h"
#include <json11.hpp>

using namespace json11;

namespace dronecode_sdk {

class MissionImpl : public PluginImplBase {
public:
    MissionImpl(System &system);
    ~MissionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void upload_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                              const Mission::result_callback_t &callback);
    void upload_mission_cancel();

    void download_mission_async(const Mission::mission_items_and_result_callback_t &callback);
    void download_mission_cancel();

    void set_return_to_launch_after_mission(bool enable_rtl);
    bool get_return_to_launch_after_mission();

    void start_mission_async(const Mission::result_callback_t &callback);
    void pause_mission_async(const Mission::result_callback_t &callback);

    void set_current_mission_item_async(int current, Mission::result_callback_t &callback);

    bool is_mission_finished() const;

    int current_mission_item() const;
    int total_mission_items() const;

    void subscribe_progress(Mission::progress_callback_t callback);

    static Mission::Result import_qgroundcontrol_mission(Mission::mission_items_t &mission_items,
                                                         const std::string &qgc_plan_file);
    // Non-copyable
    MissionImpl(const MissionImpl &) = delete;
    const MissionImpl &operator=(const MissionImpl &) = delete;

private:
    void process_mission_request(const mavlink_message_t &message);
    void process_mission_request_int(const mavlink_message_t &message);
    void process_mission_ack(const mavlink_message_t &message);
    void process_mission_current(const mavlink_message_t &message);
    void process_mission_item_reached(const mavlink_message_t &message);
    void process_mission_count(const mavlink_message_t &message);
    void process_mission_item_int(const mavlink_message_t &message);

    void process_timeout();

    void upload_mission_item(uint16_t seq);

    void copy_mission_item_vector(const std::vector<std::shared_ptr<MissionItem>> &mission_items);

    void assemble_mavlink_messages();

    void report_mission_result(const Mission::result_callback_t &callback, Mission::Result result);

    void
    report_mission_items_and_result(const Mission::mission_items_and_result_callback_t &callback,
                                    Mission::Result result);

    void report_progress();

    void receive_command_result(MAVLinkCommands::Result result,
                                const Mission::result_callback_t callback);

    void download_next_mission_item();
    void request_list();

    void assemble_mission_items();

    static Mission::Result import_mission_items(Mission::mission_items_t &mission_items,
                                                const Json &mission_json);
    static Mission::Result build_mission_items(MAV_CMD command,
                                               std::vector<double> params,
                                               std::shared_ptr<MissionItem> &new_mission_item,
                                               Mission::mission_items_t &all_mission_items);

    struct Activity {
        mutable std::mutex mutex{};
        enum class State {
            NONE,
            SET_CURRENT,
            SET_MISSION_COUNT,
            SET_MISSION_ITEM,
            GET_MISSION_LIST,
            GET_MISSION_REQUEST,
            ABORTED,
            SEND_COMMAND
        } state{Activity::State::NONE};
    } _activity{};

    struct MissionData {
        mutable std::recursive_mutex mutex{};
        unsigned retries = 0;
        int last_current_mavlink_mission_item{-1};
        int last_reached_mavlink_mission_item{-1};
        std::vector<std::shared_ptr<MissionItem>> mission_items{};
        std::vector<std::shared_ptr<mavlink_message_t>> mavlink_mission_item_messages{};
        std::map<int, int> mavlink_mission_item_to_mission_item_indices{};
        int num_mission_items_to_download{-1};
        int next_mission_item_to_download{-1};
        std::vector<std::shared_ptr<mavlink_mission_item_int_t>> mavlink_mission_items_downloaded{};
        Mission::result_callback_t result_callback{nullptr};
        Mission::mission_items_and_result_callback_t mission_items_and_result_callback{nullptr};
        Mission::progress_callback_t progress_callback{nullptr};
        int last_current_reported_mission_item{-1};
        int last_total_reported_mission_item{-1};
    } _mission_data{};

    void *_timeout_cookie{nullptr};

    bool _enable_return_to_launch_after_mission{false};

    // FIXME: This is hardcoded for now because it is urgently needed for 3DR with Yuneec H520.
    //        Ultimate it needs a setter.
    bool _enable_absolute_gimbal_yaw_angle{true};

    static constexpr unsigned MAX_RETRIES = 3;

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;

    // FIXME: these chould potentially change anytime
    // derived from:
    // https://github.com/PX4/Firmware/blob/master/src/modules/commander/px4_custom_mode.h
    static constexpr uint8_t PX4_CUSTOM_MAIN_MODE_AUTO = 4;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_LOITER = 3;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_MISSION = 4;

    static constexpr double RETRY_TIMEOUT_S = 0.250;
    static constexpr double PROCESS_TIMEOUT_S = 1.5;
};

} // namespace dronecode_sdk
