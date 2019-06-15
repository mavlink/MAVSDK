#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class MissionRawImpl : public PluginImplBase {
public:
    MissionRawImpl(System &system);
    ~MissionRawImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void download_mission_async(const MissionRaw::mission_items_and_result_callback_t &callback);
    void download_mission_cancel();

    void subscribe_mission_changed(MissionRaw::mission_changed_callback_t callback);

    MissionRawImpl(const MissionRawImpl &) = delete;
    const MissionRawImpl &operator=(const MissionRawImpl &) = delete;

private:
    void process_mission_ack(const mavlink_message_t &message);
    void process_mission_count(const mavlink_message_t &message);
    void process_mission_item_int(const mavlink_message_t &message);
    void do_download_step();
    void request_list();
    void request_item();
    void send_ack();

    struct MissionChanged {
        std::mutex mutex{};
        MissionRaw::mission_changed_callback_t callback{nullptr};
    } _mission_changed{};

    struct MissionDownload {
        std::mutex mutex{};
        enum class State { NONE, REQUEST_LIST, REQUEST_ITEM, SHOULD_ACK } state{State::NONE};
        unsigned retries{0};
        std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>
            mavlink_mission_items_downloaded{};
        MissionRaw::mission_items_and_result_callback_t callback{nullptr};
        unsigned num_mission_items_to_download{0};
        unsigned next_mission_item_to_download{0};
    } _mission_download{};

    void *_timeout_cookie{nullptr};
    static constexpr double RETRY_TIMEOUT_S = 0.250;
};

} // namespace mavsdk
