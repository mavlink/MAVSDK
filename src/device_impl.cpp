#include "global_include.h"
#include "device_impl.h"
#include "dronelink_impl.h"
#include <unistd.h>

namespace dronelink {

DeviceImpl::DeviceImpl(DroneLinkImpl *parent) :
    _handler_table(),
    _system_id(0),
    _component_id(0),
    _info(&_info_impl),
    _info_impl(this),
    _telemetry(&_telemetry_impl),
    _telemetry_impl(this),
    _control(&_control_impl),
    _control_impl(this),
    _parent(parent),
    _command_result(MAV_RESULT_FAILED),
    _command_state(CommandState::NONE)
{
    _telemetry_impl.init();
    _info_impl.init();
}

DeviceImpl::~DeviceImpl()
{
    _telemetry_impl.deinit();
    _info_impl.deinit();
}

void DeviceImpl::register_mavlink_message_handler(uint8_t msg_id,
                                                  mavlink_message_handler_t callback,
                                                  const void *cookie)
{
    HandlerTableEntry entry = {msg_id, callback, cookie};
    _handler_table.push_back(entry);
}
void DeviceImpl::unregister_all_mavlink_message_handlers(const void *cookie)
{
    for (auto it = _handler_table.begin(); it != _handler_table.end(); /* no ++it */) {

        if (it->cookie == cookie) {
            it = _handler_table.erase(it);
        } else {
            ++it;
        }
    }
}

void DeviceImpl::process_mavlink_message(const mavlink_message_t &message)
{
    for (auto it = _handler_table.begin(); it != _handler_table.end(); ++it) {
        if (it->msg_id == message.msgid) {
            it->callback(message);
        }
    }

    // TODO: also register these handlers
    switch(message.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
            process_heartbeat(message);
        break;

        case MAVLINK_MSG_ID_COMMAND_ACK:
            process_command_ack(message);
        break;

        default:
        break;
    }
}

void DeviceImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    if (_system_id == 0) {
        _system_id = message.sysid;
        _component_id = message.compid;
    }

    try_to_initialize_autopilot_capabilites();
}

void DeviceImpl::process_command_ack(const mavlink_message_t &message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    // Ignore it if we're not waiting for an ack result.
    if (_command_state == CommandState::WAITING) {
        _command_result = (MAV_RESULT)command_ack.result;
        _command_state = CommandState::RECEIVED;
    }
}


void DeviceImpl::try_to_initialize_autopilot_capabilites()
{
    if (!_info.is_complete()) {
        send_command(MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES, {1.0f, NAN, NAN, NAN, NAN, NAN, NAN});
    }
}

Result DeviceImpl::send_command(uint16_t command, const DeviceImpl::CommandParams &params)
{
    if (_system_id == 0 && _component_id == 0) {
        return Result::DEVICE_NOT_CONNECTED;
    }

    mavlink_message_t message = {};

    mavlink_msg_command_long_pack(0, MAV_COMP_ID_SYSTEM_CONTROL,
                                  &message,
                                  _system_id, _component_id,
                                  command,
                                  0,
                                  params.v[0], params.v[1], params.v[2], params.v[3],
                                  params.v[4], params.v[5], params.v[6]);

    Result ret = _parent->send_message(message);
    if (ret != Result::SUCCESS) {
        return ret;
    }

    return Result::SUCCESS;
}

Result DeviceImpl::send_command_with_ack(uint16_t command, const DeviceImpl::CommandParams &params)
{
    if (_command_state == CommandState::WAITING) {
        return Result::DEVICE_BUSY;
    }

    _command_state = CommandState::WAITING;

    Result ret = send_command(command, params);
    if (ret != Result::SUCCESS) {
        return ret;
    }

    // Wait until we have received a result.
    for (unsigned i = 0; i < 1000; ++i) {
        if (_command_state == CommandState::RECEIVED) {
            break;
        }
        usleep(1000);
    }

    if (_command_state != CommandState::RECEIVED) {
        _command_state = CommandState::NONE;
        return Result::TIMEOUT;
    }

    // Reset
    _command_state = CommandState::NONE;

    if (_command_result != MAV_RESULT_ACCEPTED) {
        return Result::COMMAND_DENIED;
    }

    return Result::SUCCESS;
}

} // namespace dronelink
