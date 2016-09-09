#include "global_include.h"
#include "device_impl.h"
#include "dronelink_impl.h"
#include <functional>
#include <unistd.h>

namespace dronelink {

DeviceImpl::DeviceImpl(DroneLinkImpl *parent) :
    _handler_table(),
    _system_id(0),
    _component_id(0),
    _uuid(0),
    _parent(parent),
    _command_result(MAV_RESULT_FAILED),
    _command_state(CommandState::NONE),
    _result_callback(nullptr)
{
    using namespace std::placeholders; // for `_1`

    register_mavlink_message_handler(MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&DeviceImpl::process_heartbeat, this, _1), this);

    register_mavlink_message_handler(MAVLINK_MSG_ID_COMMAND_ACK,
        std::bind(&DeviceImpl::process_command_ack, this, _1), this);

    register_mavlink_message_handler(MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&DeviceImpl::process_autopilot_version, this, _1), this);
}

DeviceImpl::~DeviceImpl()
{
    unregister_all_mavlink_message_handlers(this);
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
}

void DeviceImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    if (_system_id == 0) {
        _system_id = message.sysid;
        _component_id = message.compid;
    }
    if (_uuid == 0) {
        request_autopilot_version();
    }
}

void DeviceImpl::process_command_ack(const mavlink_message_t &message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    // Ignore it if we're not waiting for an ack result.
    if (_command_state == CommandState::WAITING) {
        _command_result = (MAV_RESULT)command_ack.result;
        _command_state = CommandState::RECEIVED;

        if (_command_result == MAV_RESULT_ACCEPTED) {
            report_result(_result_callback, Result::SUCCESS);
        } else {
            report_result(_result_callback, Result::COMMAND_DENIED);
        }
        _result_callback = nullptr;
    }
}

void DeviceImpl::process_autopilot_version(const mavlink_message_t &message)
{
    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    _uuid = autopilot_version.uid;
}

void DeviceImpl::request_autopilot_version()
{
    send_command(MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
                 {1.0f, NAN, NAN, NAN, NAN, NAN, NAN});
}

uint64_t DeviceImpl::get_uuid() const
{
    return _uuid;
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

void DeviceImpl::send_command_with_ack_async(uint16_t command,
                                             const DeviceImpl::CommandParams &params,
                                             result_callback_t callback)
{
    if (_command_state == CommandState::WAITING) {
        report_result(callback, Result::DEVICE_BUSY);
    }

    Result ret = send_command(command, params);
    if (ret != Result::SUCCESS) {
        report_result(callback, ret);
        // Reset
        _command_state = CommandState::NONE;
        return;
    }

    _command_state = CommandState::WAITING;
    _result_callback = callback;
}


void DeviceImpl::report_result(result_callback_t callback, Result result)
{
    // Never use a nullptr as a callback!
    if (callback != nullptr) {
        return;
    } else {
        Debug() << "Callback is NULL";
    }

    callback(result);
}

} // namespace dronelink
