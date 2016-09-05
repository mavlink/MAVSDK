#include "global_include.h"
#include "device_impl.h"
#include "dronelink_impl.h"
#include <unistd.h>

namespace dronelink {

DeviceImpl::DeviceImpl(DroneLinkImpl *parent) :
    _system_id(0),
    _component_id(0),
    _info(&_info_impl),
    _info_impl(),
    _telemetry(&_telemetry_impl),
    _telemetry_impl(),
    _parent(parent),
    _command_result(MAV_RESULT_FAILED),
    _command_state(CommandState::NONE)
{

}

DeviceImpl::~DeviceImpl()
{

}

void DeviceImpl::process_mavlink_message(const mavlink_message_t &message)
{
    switch(message.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
            process_heartbeat(message);
        break;

        case MAVLINK_MSG_ID_AUTOPILOT_VERSION:
            process_autopilot_version(message);
        break;

        case MAVLINK_MSG_ID_COMMAND_ACK:
            process_command_ack(message);
        break;

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            process_global_position_int(message);
        break;

        case MAVLINK_MSG_ID_EXTENDED_SYS_STATE:
            process_extended_sys_state(message);
        break;

        case MAVLINK_MSG_ID_ATTITUDE_QUATERNION:
            process_attitude_quaternion(message);
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

void DeviceImpl::process_autopilot_version(const mavlink_message_t &message)
{
    mavlink_autopilot_version_t autopilot_version;

    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    // TODO: remove hardcoded version
    _info_impl.set_version(1);
    _info_impl.set_uuid(autopilot_version.uid);
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

void DeviceImpl::process_global_position_int(const mavlink_message_t &message)
{
    mavlink_global_position_int_t global_position_int;
    mavlink_msg_global_position_int_decode(&message, &global_position_int);
    _telemetry_impl.set_absolute_altitude_m(float(global_position_int.alt) * 1e-3f);
    _telemetry_impl.set_relative_altitude_m(float(global_position_int.relative_alt) * 1e-3f);
    _telemetry_impl.set_coordinates(
        Telemetry::Coordinates({global_position_int.lat * 1e-7,
                                global_position_int.lon * 1e-7}));
}

void DeviceImpl::process_extended_sys_state(const mavlink_message_t &message)
{
    mavlink_extended_sys_state_t extended_sys_state;
    mavlink_msg_extended_sys_state_decode(&message, &extended_sys_state);
    if (extended_sys_state.landed_state == MAV_LANDED_STATE_IN_AIR) {
        _telemetry_impl.set_in_air(true);
    } else if (extended_sys_state.landed_state == MAV_LANDED_STATE_IN_AIR) {
        _telemetry_impl.set_in_air(false);
    }
    // If landed_state is undefined, we use what we have received last.
}

void DeviceImpl::process_attitude_quaternion(const mavlink_message_t &message)
{
    mavlink_attitude_quaternion_t attitude_quaternion;
    mavlink_msg_attitude_quaternion_decode(&message, &attitude_quaternion);

    Telemetry::Quaternion quaternion(
        {attitude_quaternion.q1,
         attitude_quaternion.q2,
         attitude_quaternion.q3,
         attitude_quaternion.q4}
    );

    _telemetry_impl.set_attitude_quaternion(quaternion);
}

void DeviceImpl::try_to_initialize_autopilot_capabilites()
{
    if (!_info.is_complete()) {
        mavlink_message_t message = {};
        mavlink_msg_command_long_pack(0, MAV_COMP_ID_SYSTEM_CONTROL,
                                     &message,
                                     _system_id, _component_id,
                                     MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
                                     0,
                                     1.0f, NAN, NAN, NAN, NAN, NAN, NAN);
        send_command(message);
    }
}

Result DeviceImpl::arm()
{
    mavlink_message_t message = {};
    mavlink_msg_command_long_pack(0, MAV_COMP_ID_SYSTEM_CONTROL,
                                  &message,
                                  _system_id, _component_id,
                                  MAV_CMD_COMPONENT_ARM_DISARM,
                                  0,
                                  1.0f, NAN, NAN, NAN, NAN, NAN, NAN);

    return send_command_with_ack(message);
}

Result DeviceImpl::disarm()
{
    mavlink_message_t message = {};
    mavlink_msg_command_long_pack(0, MAV_COMP_ID_SYSTEM_CONTROL,
                                  &message,
                                  _system_id, _component_id,
                                  MAV_CMD_COMPONENT_ARM_DISARM,
                                  0,
                                  0.0f, NAN, NAN, NAN, NAN, NAN, NAN);

    return send_command_with_ack(message);
}

Result DeviceImpl::takeoff()
{
    mavlink_message_t message = {};
    mavlink_msg_command_long_pack(0, MAV_COMP_ID_SYSTEM_CONTROL,
                                  &message,
                                  _system_id, _component_id,
                                  MAV_CMD_NAV_TAKEOFF,
                                  0,
                                  NAN, NAN, NAN, NAN, NAN, NAN, NAN);

    return send_command_with_ack(message);
}

Result DeviceImpl::land()
{
    mavlink_message_t message = {};
    mavlink_msg_command_long_pack(0, MAV_COMP_ID_SYSTEM_CONTROL,
                                  &message,
                                  _system_id, _component_id,
                                  MAV_CMD_NAV_LAND,
                                  0,
                                  NAN, NAN, NAN, NAN, NAN, NAN, NAN);

    return send_command_with_ack(message);
}

Result DeviceImpl::send_command(const mavlink_message_t &message)
{
    if (_system_id == 0 && _component_id == 0) {
        return Result::DEVICE_NOT_CONNECTED;
    }

    Result ret = _parent->send_message(message);
    if (ret != Result::SUCCESS) {
        return ret;
    }

    return Result::SUCCESS;
}

Result DeviceImpl::send_command_with_ack(const mavlink_message_t &message)
{

    if (_command_state == CommandState::WAITING) {
        return Result::DEVICE_BUSY;
    }

    _command_state = CommandState::WAITING;

    Result ret = _parent->send_message(message);
    if (ret != Result::SUCCESS) {
        return ret;
    }

    // Wait until we have received a result.
    for (unsigned i = 0; i < 100; ++i) {
        if (_command_state == CommandState::RECEIVED) {
            break;
        }
        usleep(1000);
    }

    if (_command_state != CommandState::RECEIVED) {
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
