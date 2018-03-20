#pragma once

#include <memory>
#include <map>
#include <mutex>

#include "system.h"
#include "mavlink_system.h"
#include "mavlink_include.h"
#include "mission.h"
#include "plugin_impl_base.h"
#include <json11.hpp>

using namespace json11;

namespace dronecore {

class MissionImpl : public PluginImplBase
{
public:
    MissionImpl(System &system);
    ~MissionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void upload_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                              const Mission::result_callback_t &callback);

    void download_mission_async(const Mission::mission_items_and_result_callback_t &callback);

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

    static void report_mission_result(const Mission::result_callback_t &callback,
                                      Mission::Result result);

    void report_mission_items_and_result(const Mission::mission_items_and_result_callback_t &callback,
                                         Mission::Result result);

    void report_progress();

    void receive_command_result(MAVLinkCommands::Result result,
                                const Mission::result_callback_t &callback);

    void download_next_mission_item();
    void assemble_mission_items();

    static Mission::Result
    import_mission_items(Mission::mission_items_t &mission_items,
                         const Json &mission_json);
    static Mission::Result
    build_mission_items(MAV_CMD cmd, std::vector<double> params,
                        std::shared_ptr<MissionItem> &new_mission_item,
                        Mission::mission_items_t &all_mission_items);

    std::mutex _mutex {};
    Mission::result_callback_t _result_callback = nullptr;
    Mission::mission_items_and_result_callback_t _mission_items_and_result_callback = nullptr;

    enum class Activity {
        NONE,
        SET_CURRENT,
        SET_MISSION,
        GET_MISSION,
        SEND_COMMAND
    } _activity = Activity::NONE;

    unsigned _retries = 0;
    static constexpr unsigned MAX_RETRIES = 3;

    int _last_current_mavlink_mission_item = -1;
    int _last_reached_mavlink_mission_item = -1;

    std::vector<std::shared_ptr<MissionItem>> _mission_items {};
    std::vector<std::shared_ptr<mavlink_message_t>> _mavlink_mission_item_messages {};

    std::map<int, int> _mavlink_mission_item_to_mission_item_indices {};

    Mission::progress_callback_t _progress_callback = nullptr;

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;

    // FIXME: these chould potentially change anytime
    // derived from:
    // https://github.com/PX4/Firmware/blob/master/src/modules/commander/px4_custom_mode.h
    static constexpr uint8_t PX4_CUSTOM_MAIN_MODE_AUTO = 4;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_LOITER = 3;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_MISSION = 4;

    int _num_mission_items_to_download = -1;
    int _next_mission_item_to_download = -1;
    std::vector<std::shared_ptr<mavlink_mission_item_int_t>> _mavlink_mission_items_downloaded {};

    static constexpr double RETRY_TIMEOUT_S = 0.250;
    static constexpr double PROCESS_TIMEOUT_S = 1.5;
    void *_timeout_cookie = nullptr;
};

} // namespace dronecore
