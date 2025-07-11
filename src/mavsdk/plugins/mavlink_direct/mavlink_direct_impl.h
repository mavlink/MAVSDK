#pragma once

#include "plugins/mavlink_direct/mavlink_direct.h"

#include "plugin_impl_base.h"

namespace mavsdk {

class MavlinkDirectImpl : public PluginImplBase {
public:
    explicit MavlinkDirectImpl(System& system);
    explicit MavlinkDirectImpl(std::shared_ptr<System> system);

    ~MavlinkDirectImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    MavlinkDirect::Result send_message(MavlinkDirect::MavlinkMessage message);

    MavlinkDirect::MessageHandle subscribe_message(const MavlinkDirect::MessageCallback& callback);

    void unsubscribe_message(MavlinkDirect::MessageHandle handle);

    MavlinkDirect::MessageTypeHandle
    subscribe_message_type(uint32_t message_id, const MavlinkDirect::MessageTypeCallback& callback);

    void unsubscribe_message_type(MavlinkDirect::MessageTypeHandle handle);

private:
};

} // namespace mavsdk