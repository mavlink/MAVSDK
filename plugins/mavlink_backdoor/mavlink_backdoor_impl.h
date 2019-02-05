#pragma once

#include "plugins/mavlink_backdoor/mavlink_backdoor.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronecode_sdk {

class MavlinkBackdoorImpl : public PluginImplBase {
public:
    MavlinkBackdoorImpl(System &system);
    ~MavlinkBackdoorImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    MavlinkBackdoor::Result send_message(/*const mavlink_message_t& message*/);

    void subscribe_message_async(std::function<void(/*const mavlink_message_t&*/)> callback);

private:
};

} // namespace dronecode_sdk
