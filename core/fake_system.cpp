#include "fake_system.h"
#include "log.h"

namespace dronecode_sdk {

FakeSystem::FakeSystem(State state) : _state(state) {}

FakeSystem::~FakeSystem()
{
    stop();
}

bool FakeSystem::start()
{
    _connection = std::make_shared<UdpConnection>(
        std::bind(&FakeSystem::receive_message, this, std::placeholders::_1), "127.0.0.1", 14557);
    _connection->add_remote_port(14540);

    ConnectionResult ret = _connection->start();

    if (ret != ConnectionResult::SUCCESS) {
        return false;
    }

    _should_exit = false;
    _thread = std::make_shared<std::thread>(std::bind(&FakeSystem::run, this));

    return true;
}

void FakeSystem::stop()
{
    _should_exit = true;
    _thread->join();
    _thread = nullptr;

    _connection = nullptr;
}

void FakeSystem::run()
{
    while (!_should_exit) {
        send_heartbeat();
        send_extended_sys_state();
        // To accelerate tests, let's run the heartbeat at 10 Hz.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void FakeSystem::send_heartbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(MAV_TYPE_GCS,
                               MAV_AUTOPILOT_INVALID,
                               &message,
                               1, // target sysid
                               1, // target compid
                               0,
                               0,
                               0);
    _connection->send_message(message);
}

void FakeSystem::send_extended_sys_state()
{
    mavlink_message_t message;
    mavlink_msg_extended_sys_state_pack(MAV_TYPE_GCS,
                                        MAV_AUTOPILOT_INVALID,
                                        &message,
                                        MAV_VTOL_STATE_UNDEFINED,
                                        _landed ? MAV_LANDED_STATE_ON_GROUND :
                                                  MAV_LANDED_STATE_IN_AIR);

    _connection->send_message(message);
}

void FakeSystem::receive_message(const mavlink_message_t &message)
{
    switch (message.msgid) {
        case MAVLINK_MSG_ID_COMMAND_INT:
            respond_to_command_int(message);
            break;
        case MAVLINK_MSG_ID_COMMAND_LONG:
            respond_to_command_long(message);
            break;
        default:
            break;
    }
}

void FakeSystem::respond_to_command_int(const mavlink_message_t &message)
{
    mavlink_command_int_t command;
    mavlink_msg_command_int_decode(&message, &command);

    if (command.command == MAV_CMD_NAV_TAKEOFF && _state == State::LOYAL) {
        _landed = false;
    }

    if (command.command == MAV_CMD_NAV_LAND && _state == State::LOYAL) {
        _landed = true;
    }

    send_command_ack(command.command);
}

void FakeSystem::respond_to_command_long(const mavlink_message_t &message)
{
    mavlink_command_long_t command;
    mavlink_msg_command_long_decode(&message, &command);

    if (command.command == MAV_CMD_NAV_TAKEOFF && _state == State::LOYAL) {
        _landed = false;
    }

    if (command.command == MAV_CMD_NAV_LAND && _state == State::LOYAL) {
        _landed = true;
    }

    send_command_ack(command.command);
}

void FakeSystem::send_command_ack(uint16_t command)
{
    if (_state == State::LOYAL) {
        mavlink_message_t message;
        mavlink_msg_command_ack_pack(MAV_TYPE_GCS,
                                     MAV_AUTOPILOT_INVALID,
                                     &message,
                                     command,
                                     MAV_RESULT_ACCEPTED,
                                     255, // progress
                                     0, // result_param2
                                     1, // target sysid
                                     1 // target compid
        );
        _connection->send_message(message);
    }
}

} // namespace dronecode_sdk
