#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class MissionRawImpl : public PluginImplBase {
public:
    explicit MissionRawImpl(System& system);
    explicit MissionRawImpl(std::shared_ptr<System> system);
    ~MissionRawImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<MissionRaw::Result, std::vector<MissionRaw::MissionItem>> download_mission();
    void download_mission_async(const MissionRaw::DownloadMissionCallback& callback);
    MissionRaw::Result cancel_mission_download();

    MissionRaw::Result upload_mission(std::vector<MissionRaw::MissionItem> mission_items);
    void upload_mission_async(
        const std::vector<MissionRaw::MissionItem>& mission_raw,
        const MissionRaw::ResultCallback& callback);
    MissionRaw::Result cancel_mission_upload();

    void subscribe_mission_changed(MissionRaw::MissionChangedCallback callback);

    MissionRaw::Result start_mission();
    void start_mission_async(const MissionRaw::ResultCallback& callback);
    MissionRaw::Result pause_mission();
    void pause_mission_async(const MissionRaw::ResultCallback& callback);
    MissionRaw::Result clear_mission();
    void clear_mission_async(const MissionRaw::ResultCallback& callback);

    MissionRaw::Result set_current_mission_item(int index);
    void set_current_mission_item_async(int index, const MissionRaw::ResultCallback& callback);

    int current_mavlink_mission_item() const;
    int total_mavlink_mission_items() const;

    MissionRaw::MissionProgress mission_progress();
    void subscribe_mission_progress(MissionRaw::MissionProgressCallback callback);

    std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
    import_qgroundcontrol_mission(std::string qgc_plan_path);

#if 0
    void import_qgroundcontrol_mission_async(
        std::string qgc_plan_path, const MissionRaw::ImportQgroundcontrolMissionCallback callback);

    static std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
    import_qgroundcontrol_mission(const std::string& qgc_plan_path);
#endif

    MissionRawImpl(const MissionRawImpl&) = delete;
    const MissionRawImpl& operator=(const MissionRawImpl&) = delete;

private:
    void reset_mission_progress();

    void process_mission_ack(const mavlink_message_t& message);
    void process_mission_current(const mavlink_message_t& message);
    void process_mission_item_reached(const mavlink_message_t& message);

    void report_progress_current();

    void report_flight_mode_change(
        MissionRaw::ResultCallback callback, MavlinkCommandSender::Result result);
    static MissionRaw::Result command_result_to_mission_result(MavlinkCommandSender::Result result);

    std::vector<MavlinkMissionTransfer::ItemInt>
    convert_to_int_items(const std::vector<MissionRaw::MissionItem>& mission_raw);

    MavlinkMissionTransfer::ItemInt
    convert_mission_raw(const MissionRaw::MissionItem transfer_mission_raw);

    static MissionRaw::Result convert_result(MavlinkMissionTransfer::Result result);
    MissionRaw::MissionItem static convert_item(
        const MavlinkMissionTransfer::ItemInt& transfer_item);
    std::vector<MissionRaw::MissionItem>
    convert_items(const std::vector<MavlinkMissionTransfer::ItemInt>& transfer_items);

    // TODO: check if these need a mutex as well.
    std::weak_ptr<MavlinkMissionTransfer::WorkItem> _last_upload{};
    std::weak_ptr<MavlinkMissionTransfer::WorkItem> _last_download{};

    struct {
        std::mutex mutex{};
        MissionRaw::MissionProgress last{};
        MissionRaw::MissionProgress last_reported{};
        MissionRaw::MissionProgressCallback callback{nullptr};
        int32_t last_reached{};
    } _mission_progress{};

    struct {
        std::mutex mutex{};
        MissionRaw::MissionChangedCallback callback{nullptr};
    } _mission_changed{};
};

} // namespace mavsdk
