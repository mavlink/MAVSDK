#include "server_component_impl.h"
#include "server_plugin_impl_base.h"
#include "mavsdk_impl.h"

namespace mavsdk {

ServerComponentImpl::ServerComponentImpl(MavsdkImpl& mavsdk_impl, uint8_t component_id) :
    _mavsdk_impl(mavsdk_impl),
    _own_component_id(component_id),
    _our_sender(*this),
    _mavlink_command_receiver(*this),
    _mission_transfer_server(
        _our_sender,
        mavsdk_impl.mavlink_message_handler,
        mavsdk_impl.timeout_handler,
        [this]() { return _mavsdk_impl.timeout_s(); }),
    _mavlink_parameter_server(_our_sender, mavsdk_impl.mavlink_message_handler),
    _mavlink_request_message_handler(mavsdk_impl, *this, _mavlink_command_receiver),
    _mavlink_ftp_server(*this)
{
    if (!MavlinkChannels::Instance().checkout_free_channel(_channel)) {
        // We use a default of channel 0 which will still work but not track
        // seq correctly.
        _channel = 0;
        LogErr() << "Could not get a MAVLink channel, using default 0";
    }

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

    MavlinkChannels::Instance().checkin_used_channel(_channel);
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
    _mission_transfer_server.do_work();
}

Sender& ServerComponentImpl::sender()
{
    return _our_sender;
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

bool ServerComponentImpl::send_command_ack(mavlink_command_ack_t& command_ack)
{
    return queue_message([&, this](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_command_ack_encode_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            &command_ack);
        return message;
    });
}

bool ServerComponentImpl::queue_message(
    std::function<mavlink_message_t(MavlinkAddress mavlink_address, uint8_t channel)> fun)
{
    std::lock_guard<std::mutex> lock(_mavlink_pack_mutex);
    MavlinkAddress mavlink_address{get_own_system_id(), get_own_component_id()};
    mavlink_message_t message = fun(mavlink_address, _channel);
    return _mavsdk_impl.send_message(message);
}

CallEveryHandler::Cookie
ServerComponentImpl::add_call_every(std::function<void()> callback, float interval_s)
{
    return _mavsdk_impl.call_every_handler.add(
        std::move(callback), static_cast<double>(interval_s));
}

void ServerComponentImpl::change_call_every(float interval_s, CallEveryHandler::Cookie cookie)
{
    _mavsdk_impl.call_every_handler.change(static_cast<double>(interval_s), cookie);
}

void ServerComponentImpl::reset_call_every(CallEveryHandler::Cookie cookie)
{
    _mavsdk_impl.call_every_handler.reset(cookie);
}

void ServerComponentImpl::remove_call_every(CallEveryHandler::Cookie cookie)
{
    _mavsdk_impl.call_every_handler.remove(cookie);
}

mavlink_command_ack_t ServerComponentImpl::make_command_ack_message(
    const MavlinkCommandReceiver::CommandLong& command, MAV_RESULT result)
{
    mavlink_command_ack_t command_ack{};
    command_ack.command = command.command;
    command_ack.result = result;
    command_ack.progress = std::numeric_limits<uint8_t>::max();
    command_ack.result_param2 = 0;
    command_ack.target_system = command.origin_system_id;
    command_ack.target_component = command.origin_component_id;

    return command_ack;
}

mavlink_command_ack_t ServerComponentImpl::make_command_ack_message(
    const MavlinkCommandReceiver::CommandInt& command, MAV_RESULT result)
{
    mavlink_command_ack_t command_ack{};
    command_ack.command = command.command;
    command_ack.result = result;
    command_ack.progress = std::numeric_limits<uint8_t>::max();
    command_ack.result_param2 = 0;
    command_ack.target_system = command.origin_system_id;
    command_ack.target_component = command.origin_component_id;

    return command_ack;
}

void ServerComponentImpl::send_heartbeat()
{
    queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_heartbeat_pack_chan(
            get_own_system_id(),
            mavlink_address.component_id,
            channel,
            &message,
            _mavsdk_impl.get_mav_type(),
            mavlink_address.component_id == MAV_COMP_ID_AUTOPILOT1 ? MAV_AUTOPILOT_GENERIC :
                                                                     MAV_AUTOPILOT_INVALID,
            mavlink_address.component_id == MAV_COMP_ID_AUTOPILOT1 ? _base_mode.load() : 0,
            mavlink_address.component_id == MAV_COMP_ID_AUTOPILOT1 ? _custom_mode.load() : 0,
            get_system_status());
        return message;
    });
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

TimeoutHandler::Cookie ServerComponentImpl::register_timeout_handler(
    const std::function<void()>& callback, double duration_s)
{
    return _mavsdk_impl.timeout_handler.add(callback, duration_s);
}

void ServerComponentImpl::refresh_timeout_handler(TimeoutHandler::Cookie cookie)
{
    _mavsdk_impl.timeout_handler.refresh(cookie);
}

void ServerComponentImpl::unregister_timeout_handler(TimeoutHandler::Cookie cookie)
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

    queue_message([&, this](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_autopilot_version_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
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
        return message;
    });
}

ServerComponentImpl::OurSender::OurSender(ServerComponentImpl& server_component_impl) :
    _server_component_impl(server_component_impl)
{}

bool ServerComponentImpl::OurSender::send_message(mavlink_message_t& message)
{
    return _server_component_impl.send_message(message);
}

bool ServerComponentImpl::OurSender::queue_message(
    std::function<mavlink_message_t(MavlinkAddress, uint8_t)> fun)
{
    return _server_component_impl.queue_message(fun);
}

uint8_t ServerComponentImpl::OurSender::get_own_system_id() const
{
    return _server_component_impl.get_own_system_id();
}

uint8_t ServerComponentImpl::OurSender::get_own_component_id() const
{
    return _server_component_impl.get_own_component_id();
}

Autopilot ServerComponentImpl::OurSender::autopilot() const
{
    // FIXME: hard-coded to PX4 for now to avoid the dependency into mavsdk_impl.
    return Autopilot::Px4;
}

} // namespace mavsdk
