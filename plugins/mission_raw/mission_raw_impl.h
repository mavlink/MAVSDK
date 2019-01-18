#pragma once

#include "system.h"
#include "mavlink_include.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugin_impl_base.h"

namespace dronecode_sdk {

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
};

} // namespace dronecode_sdk
