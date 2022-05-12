#include "mavlink_command_receiver.h"
#include "mavsdk_impl.h"
#include "log.h"
#include <cmath>
#include <future>
#include <memory>

namespace mavsdk {

MavlinkCommandReceiver::MavlinkCommandReceiver(MavsdkImpl& mavsdk_impl) : _mavsdk_impl(mavsdk_impl)
{
    _mavsdk_impl.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_COMMAND_LONG,
        [this](const mavlink_message_t& message) { receive_command_long(message); },
        this);

    _mavsdk_impl.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_COMMAND_INT,
        [this](const mavlink_message_t& message) { receive_command_int(message); },
        this);
}

MavlinkCommandReceiver::~MavlinkCommandReceiver()
{
    unregister_all_mavlink_command_handlers(this);

    _mavsdk_impl.mavlink_message_handler.unregister_all(this);
}

void MavlinkCommandReceiver::receive_command_int(const mavlink_message_t& message)
{
    MavlinkCommandReceiver::CommandInt cmd(message);

    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    for (auto& handler : _mavlink_command_int_handler_table) {
        if (handler.cmd_id == cmd.command) {
            // The client side can pack a COMMAND_ACK as a response to receiving the command.
            auto maybe_message = handler.callback(cmd);
            if (maybe_message) {
                _mavsdk_impl.send_message(maybe_message.value());
            }
        }
    }
}

void MavlinkCommandReceiver::receive_command_long(const mavlink_message_t& message)
{
    MavlinkCommandReceiver::CommandLong cmd(message);

    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    for (auto& handler : _mavlink_command_long_handler_table) {
        if (handler.cmd_id == cmd.command) {
            // The client side can pack a COMMAND_ACK as a response to receiving the command.
            auto maybe_message = handler.callback(cmd);
            if (maybe_message) {
                _mavsdk_impl.send_message(maybe_message.value());
            }
        }
    }
}

void MavlinkCommandReceiver::register_mavlink_command_handler(
    uint16_t cmd_id, const MavlinkCommandIntHandler& callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    MAVLinkCommandIntHandlerTableEntry entry = {cmd_id, callback, cookie};
    _mavlink_command_int_handler_table.push_back(entry);
}

void MavlinkCommandReceiver::register_mavlink_command_handler(
    uint16_t cmd_id, const MavlinkCommandLongHandler& callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    MAVLinkCommandLongHandlerTableEntry entry = {cmd_id, callback, cookie};
    _mavlink_command_long_handler_table.push_back(entry);
}

void MavlinkCommandReceiver::unregister_mavlink_command_handler(uint16_t cmd_id, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    // COMMAND_INT
    for (auto it = _mavlink_command_int_handler_table.begin();
         it != _mavlink_command_int_handler_table.end();
         /* no ++it */) {
        if (it->cmd_id == cmd_id && it->cookie == cookie) {
            it = _mavlink_command_int_handler_table.erase(it);
        } else {
            ++it;
        }
    }

    // COMMAND_LONG
    for (auto it = _mavlink_command_long_handler_table.begin();
         it != _mavlink_command_long_handler_table.end();
         /* no ++it */) {
        if (it->cmd_id == cmd_id && it->cookie == cookie) {
            it = _mavlink_command_long_handler_table.erase(it);
        } else {
            ++it;
        }
    }
}

void MavlinkCommandReceiver::unregister_all_mavlink_command_handlers(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    // COMMAND_INT
    for (auto it = _mavlink_command_int_handler_table.begin();
         it != _mavlink_command_int_handler_table.end();
         /* no ++it */) {
        if (it->cookie == cookie) {
            it = _mavlink_command_int_handler_table.erase(it);
        } else {
            ++it;
        }
    }

    // COMMAND_LONG
    for (auto it = _mavlink_command_long_handler_table.begin();
         it != _mavlink_command_long_handler_table.end();
         /* no ++it */) {
        if (it->cookie == cookie) {
            it = _mavlink_command_long_handler_table.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace mavsdk
