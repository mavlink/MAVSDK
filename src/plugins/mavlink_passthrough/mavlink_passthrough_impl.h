#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class MavlinkPassthroughImpl : public PluginImplBase {
public:
    explicit MavlinkPassthroughImpl(System& system);
    explicit MavlinkPassthroughImpl(std::shared_ptr<System> system);
    ~MavlinkPassthroughImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    MavlinkPassthrough::Result send_message(mavlink_message_t& message);
    MavlinkPassthrough::Result send_command_long(const MavlinkPassthrough::CommandLong& command);
    MavlinkPassthrough::Result send_command_int(const MavlinkPassthrough::CommandInt& command);

    void subscribe_message_async(
        uint16_t message_id, std::function<void(const mavlink_message_t&)> callback);

    uint8_t get_our_sysid() const;
    uint8_t get_our_compid() const;
    uint8_t get_target_sysid() const;
    uint8_t get_target_compid() const;

    void intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback);
    void intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback);

private:
    static MavlinkPassthrough::Result
    to_mavlink_passthrough_result_from_mavlink_commands_result(MavlinkCommandSender::Result result);
};

} // namespace mavsdk
