#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class MissionRawImpl : public PluginImplBase {
public:
    MissionRawImpl(System& system);
    ~MissionRawImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void download_mission_async(const MissionRaw::mission_items_and_result_callback_t& callback);
    void download_mission_cancel();

    void upload_mission_async(
        const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>& mission_raw,
        const MissionRaw::result_callback_t& callback);
    void upload_mission_cancel();

    void subscribe_mission_changed(MissionRaw::mission_changed_callback_t callback);

    void start_mission_async(const MissionRaw::result_callback_t& callback);
    void pause_mission_async(const MissionRaw::result_callback_t& callback);
    void clear_mission_async(const MissionRaw::result_callback_t& callback);

    void
    set_current_mission_item_async(int current_mavlink, MissionRaw::result_callback_t& callback);

    bool is_mission_finished() const;

    int current_mavlink_mission_item() const;
    int total_mavlink_mission_items() const;

    void subscribe_progress(MissionRaw::progress_callback_t callback);

    MissionRawImpl(const MissionRawImpl&) = delete;
    const MissionRawImpl& operator=(const MissionRawImpl&) = delete;

private:
    void process_mission_ack(const mavlink_message_t& message);
    void process_mission_current(const mavlink_message_t& message);
    void process_mission_item_reached(const mavlink_message_t& message);

    void report_progress();
    void reset_mission_progress();

    void report_flight_mode_change(
        MissionRaw::result_callback_t callback, MAVLinkCommands::Result result);
    static MissionRaw::Result command_result_to_mission_result(MAVLinkCommands::Result result);

    std::vector<MAVLinkMissionTransfer::ItemInt> convert_to_int_items(
        const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>& mission_raw);
    MAVLinkMissionTransfer::ItemInt convert_mission_raw(
        const std::shared_ptr<MissionRaw::MavlinkMissionItemInt> transfer_mission_raw);

    static MissionRaw::Result convert_result(MAVLinkMissionTransfer::Result result);
    MissionRaw::MavlinkMissionItemInt static convert_item(
        const MAVLinkMissionTransfer::ItemInt& transfer_item);
    std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>
    convert_items(const std::vector<MAVLinkMissionTransfer::ItemInt>& transfer_items);

    struct MissionData {
        mutable std::mutex mutex{};
        int last_current_mavlink_mission_item{-1};
        int last_reached_mavlink_mission_item{-1};
        int last_total_mavlink_mission_item{-1};
        int last_current_reported_mavlink_mission_item{-1};
        int last_total_reported_mavlink_mission_item{-1};
        int num_mission_items_to_download{-1};
        int next_mission_item_to_download{-1};
        int last_mission_item_to_upload{-1};
        MissionRaw::result_callback_t result_callback{nullptr};
        MissionRaw::mission_items_and_result_callback_t mission_items_and_result_callback{nullptr};
        MissionRaw::progress_callback_t progress_callback{nullptr};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_upload{};
        std::weak_ptr<MAVLinkMissionTransfer::WorkItem> last_download{};
    } _mission_data{};

    void* _timeout_cookie{nullptr};

    struct MissionChanged {
        std::mutex mutex{};
        MissionRaw::mission_changed_callback_t callback{nullptr};
    } _mission_changed{};
};

} // namespace mavsdk
