#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class MavlinkPassthroughImpl : public PluginImplBase {
public:
    explicit MavlinkPassthroughImpl(System& system);
    explicit MavlinkPassthroughImpl(std::shared_ptr<System> system);
    ~MavlinkPassthroughImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    MavlinkPassthrough::Result send_message(mavlink_message_t& message);
    MavlinkPassthrough::Result send_command_long(const MavlinkPassthrough::CommandLong& command);
    MavlinkPassthrough::Result send_command_int(const MavlinkPassthrough::CommandInt& command);
    mavlink_message_t make_command_ack_message(
        const uint8_t target_sysid,
        const uint8_t target_compid,
        const uint16_t command,
        MAV_RESULT result);
    std::pair<MavlinkPassthrough::Result, int32_t> get_param_int(
        const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended);
    std::pair<MavlinkPassthrough::Result, float> get_param_float(
        const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended);

    MavlinkPassthrough::MessageHandle
    subscribe_message(uint16_t message_id, const MavlinkPassthrough::MessageCallback& callback);

    void unsubscribe_message(MavlinkPassthrough::MessageHandle handle);

    uint8_t get_our_sysid() const;
    uint8_t get_our_compid() const;
    uint8_t get_target_sysid() const;
    uint8_t get_target_compid() const;

private:
    void receive_mavlink_message(const mavlink_message_t& message);

    static MavlinkPassthrough::Result
    to_mavlink_passthrough_result_from_mavlink_commands_result(MavlinkCommandSender::Result result);

    static MavlinkPassthrough::Result
    to_mavlink_passthrough_result_from_mavlink_params_result(MavlinkParameterClient::Result result);

    std::unordered_map<uint16_t, CallbackList<const mavlink_message_t&>> _message_subscriptions{};
};

} // namespace mavsdk
