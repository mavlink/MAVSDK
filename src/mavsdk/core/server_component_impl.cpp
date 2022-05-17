#include "server_component_impl.h"
#include "server_plugin_impl_base.h"
#include "mavsdk_impl.h"

namespace mavsdk {

ServerComponentImpl::ServerComponentImpl(MavsdkImpl& mavsdk_impl, uint8_t component_id) :
    _mavsdk_impl(mavsdk_impl),
    _own_component_id(component_id),
    _our_sender(mavsdk_impl, *this),
    _mavlink_command_receiver(mavsdk_impl),
    _mission_transfer(
        _our_sender,
        mavsdk_impl.mavlink_message_handler,
        mavsdk_impl.timeout_handler,
        [this]() { return _mavsdk_impl.timeout_s(); }),
    _mavlink_parameter_server(_our_sender, mavsdk_impl.mavlink_message_handler),
    _mavlink_request_message_handler(mavsdk_impl, *this, _mavlink_command_receiver)
{
    register_mavlink_command_handler(
        MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            send_autopilot_version();
            return make_command_ack_message(command, MAV_RESULT_ACCEPTED);
        },
        this);

    register_mavlink_command_handler(
        MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
        [this](const MavlinkCommandReceiver::CommandInt& command) {
            send_autopilot_version();
            return make_command_ack_message(command, MAV_RESULT_ACCEPTED);
        },
        this);

    _mavlink_request_message_handler.register_handler(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        [this](uint8_t, uint8_t, const MavlinkRequestMessageHandler::Params&) {
            send_autopilot_version();
            return MAV_RESULT_ACCEPTED;
        },
        this);
}

ServerComponentImpl::~ServerComponentImpl()
{
    unregister_all_mavlink_command_handlers(this);
    _mavlink_request_message_handler.unregister_all_handlers(this);
}

void ServerComponentImpl::register_plugin(ServerPluginImplBase* server_plugin_impl)
{
    // This is a bit pointless now but just mirrors what is done for the client plugins.
    server_plugin_impl->init();
}

void ServerComponentImpl::unregister_plugin(ServerPluginImplBase* server_plugin_impl)
{
    // This is a bit pointless now but just mirrors what is done for the client plugins.
    server_plugin_impl->deinit();
}

void ServerComponentImpl::register_mavlink_command_handler(
    uint16_t cmd_id,
    const MavlinkCommandReceiver::MavlinkCommandIntHandler& callback,
    const void* cookie)
{
    _mavlink_command_receiver.register_mavlink_command_handler(cmd_id, callback, cookie);
}

void ServerComponentImpl::register_mavlink_command_handler(
    uint16_t cmd_id,
    const MavlinkCommandReceiver::MavlinkCommandLongHandler& callback,
    const void* cookie)
{
    _mavlink_command_receiver.register_mavlink_command_handler(cmd_id, callback, cookie);
}

void ServerComponentImpl::unregister_mavlink_command_handler(uint16_t cmd_id, const void* cookie)
{
    _mavlink_command_receiver.unregister_mavlink_command_handler(cmd_id, cookie);
}

void ServerComponentImpl::unregister_all_mavlink_command_handlers(const void* cookie)
{
    _mavlink_command_receiver.unregister_all_mavlink_command_handlers(cookie);
}

void ServerComponentImpl::register_mavlink_message_handler(
    uint16_t msg_id, const MavlinkMessageHandler& callback, const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.register_one(msg_id, callback, cookie);
}

void ServerComponentImpl::register_mavlink_message_handler(
    uint16_t msg_id, uint8_t cmp_id, const MavlinkMessageHandler& callback, const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.register_one(msg_id, cmp_id, callback, cookie);
}

void ServerComponentImpl::unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.unregister_one(msg_id, cookie);
}

void ServerComponentImpl::unregister_all_mavlink_message_handlers(const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.unregister_all(cookie);
}

void ServerComponentImpl::do_work()
{
    _mavlink_parameter_server.do_work();
    _mission_transfer.do_work();
}

uint8_t ServerComponentImpl::get_own_system_id() const
{
    return _mavsdk_impl.get_own_system_id();
}

void ServerComponentImpl::set_own_component_id(uint8_t own_component_id)
{
    _own_component_id = own_component_id;
}
uint8_t ServerComponentImpl::get_own_component_id() const
{
    return _own_component_id;
}

Time& ServerComponentImpl::get_time()
{
    return _mavsdk_impl.time;
}

bool ServerComponentImpl::send_message(mavlink_message_t& message)
{
    return _mavsdk_impl.send_message(message);
}

void ServerComponentImpl::add_call_every(
    std::function<void()> callback, float interval_s, void** cookie)
{
    _mavsdk_impl.call_every_handler.add(
        std::move(callback), static_cast<double>(interval_s), cookie);
}

void ServerComponentImpl::change_call_every(float interval_s, const void* cookie)
{
    _mavsdk_impl.call_every_handler.change(static_cast<double>(interval_s), cookie);
}

void ServerComponentImpl::reset_call_every(const void* cookie)
{
    _mavsdk_impl.call_every_handler.reset(cookie);
}

void ServerComponentImpl::remove_call_every(const void* cookie)
{
    _mavsdk_impl.call_every_handler.remove(cookie);
}

mavlink_message_t ServerComponentImpl::make_command_ack_message(
    const MavlinkCommandReceiver::CommandLong& command, MAV_RESULT result)
{
    const uint8_t progress = std::numeric_limits<uint8_t>::max();
    const uint8_t result_param2 = 0;

    mavlink_message_t msg{};
    mavlink_msg_command_ack_pack(
        get_own_system_id(),
        get_own_component_id(),
        &msg,
        command.command,
        result,
        progress,
        result_param2,
        command.origin_system_id,
        command.origin_component_id);
    return msg;
}

mavlink_message_t ServerComponentImpl::make_command_ack_message(
    const MavlinkCommandReceiver::CommandInt& command, MAV_RESULT result)
{
    const uint8_t progress = std::numeric_limits<uint8_t>::max();
    const uint8_t result_param2 = 0;

    mavlink_message_t msg{};
    mavlink_msg_command_ack_pack(
        get_own_system_id(),
        get_own_component_id(),
        &msg,
        command.command,
        result,
        progress,
        result_param2,
        command.origin_system_id,
        command.origin_component_id);
    return msg;
}

void ServerComponentImpl::send_heartbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(
        get_own_system_id(),
        get_own_component_id(),
        &message,
        _mavsdk_impl.get_mav_type(),
        get_own_component_id() == MAV_COMP_ID_AUTOPILOT1 ? MAV_AUTOPILOT_GENERIC :
                                                           MAV_AUTOPILOT_INVALID,
        get_own_component_id() == MAV_COMP_ID_AUTOPILOT1 ? _base_mode.load() : 0,
        get_own_component_id() == MAV_COMP_ID_AUTOPILOT1 ? _custom_mode.load() : 0,
        get_system_status());
    send_message(message);
}

void ServerComponentImpl::set_system_status(uint8_t system_status)
{
    _system_status = static_cast<MAV_STATE>(system_status);
}

uint8_t ServerComponentImpl::get_system_status() const
{
    return _system_status;
}

void ServerComponentImpl::set_base_mode(uint8_t base_mode)
{
    _base_mode = base_mode;
}

uint8_t ServerComponentImpl::get_base_mode() const
{
    return _base_mode;
}

void ServerComponentImpl::set_custom_mode(uint32_t custom_mode)
{
    _custom_mode = custom_mode;
}

uint32_t ServerComponentImpl::get_custom_mode() const
{
    return _custom_mode;
}

void ServerComponentImpl::call_user_callback_located(
    const std::string& filename, const int linenumber, const std::function<void()>& func)
{
    _mavsdk_impl.call_user_callback_located(filename, linenumber, func);
}

void ServerComponentImpl::register_timeout_handler(
    const std::function<void()>& callback, double duration_s, void** cookie)
{
    _mavsdk_impl.timeout_handler.add(callback, duration_s, cookie);
}

void ServerComponentImpl::refresh_timeout_handler(const void* cookie)
{
    _mavsdk_impl.timeout_handler.refresh(cookie);
}

void ServerComponentImpl::unregister_timeout_handler(const void* cookie)
{
    _mavsdk_impl.timeout_handler.remove(cookie);
}

void ServerComponentImpl::add_capabilities(uint64_t add_capabilities)
{
    {
        std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
        _autopilot_version.capabilities |= add_capabilities;
    }

    // We need to resend capabilities.
    send_autopilot_version();
}

void ServerComponentImpl::set_flight_sw_version(uint32_t flight_sw_version)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    _autopilot_version.flight_sw_version = flight_sw_version;
}

void ServerComponentImpl::set_middleware_sw_version(uint32_t middleware_sw_version)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    _autopilot_version.middleware_sw_version = middleware_sw_version;
}

void ServerComponentImpl::set_os_sw_version(uint32_t os_sw_version)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    _autopilot_version.os_sw_version = os_sw_version;
}

void ServerComponentImpl::set_board_version(uint32_t board_version)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    _autopilot_version.board_version = board_version;
}

void ServerComponentImpl::set_vendor_id(uint16_t vendor_id)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    _autopilot_version.vendor_id = vendor_id;
}

void ServerComponentImpl::set_product_id(uint16_t product_id)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    _autopilot_version.product_id = product_id;
}

bool ServerComponentImpl::set_uid2(std::string uid2)
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    if (uid2.size() > _autopilot_version.uid2.size()) {
        return false;
    }
    _autopilot_version.uid2 = {0};
    std::copy(uid2.begin(), uid2.end(), _autopilot_version.uid2.data());
    return true;
}

void ServerComponentImpl::send_autopilot_version()
{
    std::lock_guard<std::mutex> lock(_autopilot_version_mutex);
    const uint8_t custom_values[8] = {0}; // TO-DO: maybe?

    mavlink_message_t msg;
    mavlink_msg_autopilot_version_pack(
        _mavsdk_impl.get_own_system_id(),
        get_own_component_id(),
        &msg,
        _autopilot_version.capabilities,
        _autopilot_version.flight_sw_version,
        _autopilot_version.middleware_sw_version,
        _autopilot_version.os_sw_version,
        _autopilot_version.board_version,
        custom_values,
        custom_values,
        custom_values,
        _autopilot_version.vendor_id,
        _autopilot_version.product_id,
        0,
        _autopilot_version.uid2.data());

    _mavsdk_impl.send_message(msg);
}

ServerComponentImpl::OurSender::OurSender(
    MavsdkImpl& mavsdk_impl, ServerComponentImpl& server_component_impl) :
    _mavsdk_impl(mavsdk_impl),
    _server_component_impl(server_component_impl)
{}

bool ServerComponentImpl::OurSender::send_message(mavlink_message_t& message)
{
    return _mavsdk_impl.send_message(message);
}

uint8_t ServerComponentImpl::OurSender::get_own_system_id() const
{
    return _mavsdk_impl.get_own_system_id();
}

uint8_t ServerComponentImpl::OurSender::get_own_component_id() const
{
    return _server_component_impl.get_own_component_id();
}
uint8_t ServerComponentImpl::OurSender::get_system_id() const
{
    return current_target_system_id;
}

Sender::Autopilot ServerComponentImpl::OurSender::autopilot() const
{
    // FIXME: hard-coded to PX4 for now to avoid the dependency into mavsdk_impl.
    return Sender::Autopilot::Px4;
}

} // namespace mavsdk
