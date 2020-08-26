#include <functional>
#include "mavlink_passthrough_impl.h"
#include "system.h"
#include "global_include.h"

namespace mavsdk {

MavlinkPassthroughImpl::MavlinkPassthroughImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MavlinkPassthroughImpl::~MavlinkPassthroughImpl()
{
    _parent->unregister_plugin(this);
}

void MavlinkPassthroughImpl::init() {}

void MavlinkPassthroughImpl::deinit()
{
    _parent->intercept_incoming_messages(nullptr);
    _parent->intercept_outgoing_messages(nullptr);
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MavlinkPassthroughImpl::enable() {}

void MavlinkPassthroughImpl::disable() {}

MavlinkPassthrough::Result MavlinkPassthroughImpl::send_message(mavlink_message_t& message)
{
    if (!_parent->send_message(message)) {
        return MavlinkPassthrough::Result::ConnectionError;
    }
    return MavlinkPassthrough::Result::Success;
}

void MavlinkPassthroughImpl::subscribe_message_async(
    uint16_t message_id, std::function<void(const mavlink_message_t&)> callback)
{
    if (callback == nullptr) {
        _parent->unregister_mavlink_message_handler(message_id, this);
    } else {
        auto temp_callback = callback;
        _parent->register_mavlink_message_handler(
            message_id,
            [this, temp_callback](const mavlink_message_t& message) {
                _parent->call_user_callback([temp_callback, message]() { temp_callback(message); });
            },
            this);
    }
}

uint8_t MavlinkPassthroughImpl::get_our_sysid() const
{
    return _parent->get_own_system_id();
}

uint8_t MavlinkPassthroughImpl::get_our_compid() const
{
    return _parent->get_own_component_id();
}

uint8_t MavlinkPassthroughImpl::get_target_sysid() const
{
    return _parent->get_system_id();
}

uint8_t MavlinkPassthroughImpl::get_target_compid() const
{
    return _parent->get_autopilot_id();
}

void MavlinkPassthroughImpl::intercept_incoming_messages_async(
    std::function<bool(mavlink_message_t&)> callback)
{
    _parent->intercept_incoming_messages(callback);
}

void MavlinkPassthroughImpl::intercept_outgoing_messages_async(
    std::function<bool(mavlink_message_t&)> callback)
{
    _parent->intercept_outgoing_messages(callback);
}

} // namespace mavsdk
