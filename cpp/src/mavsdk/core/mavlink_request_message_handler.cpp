#include "mavlink_request_message_handler.h"
#include "mavsdk_impl.h"
#include "server_component_impl.h"
#include "mavlink_command_receiver.h"
#include "log.h"
#include <algorithm>

namespace mavsdk {

MavlinkRequestMessageHandler::MavlinkRequestMessageHandler(
    MavsdkImpl& mavsdk_impl,
    ServerComponentImpl& server_component_impl,
    MavlinkCommandReceiver& mavlink_command_receiver) :
    _mavsdk_impl(mavsdk_impl),
    _server_component_impl(server_component_impl),
    _mavlink_command_receiver(mavlink_command_receiver)
{
    _mavlink_command_receiver.register_mavlink_command_handler(
        MAV_CMD_REQUEST_MESSAGE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return handle_command_long(command);
        },
        this);
}

MavlinkRequestMessageHandler::~MavlinkRequestMessageHandler()
{
    _mavsdk_impl.mavlink_message_handler.unregister_all(this);
}

bool MavlinkRequestMessageHandler::register_handler(
    uint32_t message_id, const Callback& callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_table_mutex);

    if (std::find_if(_table.begin(), _table.end(), [message_id](const Entry& entry) {
            return entry.message_id == message_id;
        }) != _table.end()) {
        LogErr() << "message id " << message_id << " already registered, registration ignored";
        return false;
    }

    _table.emplace_back(Entry{message_id, callback, cookie});
    return true;
}

void MavlinkRequestMessageHandler::unregister_handler(uint32_t message_id, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_table_mutex);

    _table.erase(
        std::remove_if(
            _table.begin(),
            _table.end(),
            [&message_id, &cookie](const Entry& entry) {
                return entry.message_id == message_id && entry.cookie == cookie;
            }),
        _table.end());
}

void MavlinkRequestMessageHandler::unregister_all_handlers(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_table_mutex);

    _table.erase(
        std::remove_if(
            _table.begin(),
            _table.end(),
            [&cookie](const Entry& entry) { return entry.cookie == cookie; }),
        _table.end());
}

std::optional<mavlink_command_ack_t> MavlinkRequestMessageHandler::handle_command_long(
    const MavlinkCommandReceiver::CommandLong& command)
{
    std::lock_guard<std::mutex> lock(_table_mutex);

    for (auto& entry : _table) {
        if (entry.message_id == static_cast<uint32_t>(std::round(command.params.param1))) {
            if (entry.callback != nullptr) {
                const auto result = entry.callback(
                    command.origin_system_id,
                    command.origin_component_id,
                    {command.params.param2,
                     command.params.param3,
                     command.params.param4,
                     command.params.param5,
                     command.params.param6});

                if (result.has_value()) {
                    return _server_component_impl.make_command_ack_message(command, result.value());
                }
            }
            return {};
        }
    }

    // We could respond with MAV_RESULT_UNSUPPORTED here, however, it's not clear if maybe someone
    // else might be answering the command.

    return {};
}

std::optional<mavlink_command_ack_t>
MavlinkRequestMessageHandler::handle_command_int(const MavlinkCommandReceiver::CommandInt& command)
{
    std::lock_guard<std::mutex> lock(_table_mutex);

    for (auto& entry : _table) {
        if (entry.message_id == static_cast<uint32_t>(std::round(command.params.param1))) {
            if (entry.callback != nullptr) {
                const auto result = entry.callback(
                    command.origin_system_id,
                    command.origin_component_id,
                    {command.params.param2,
                     command.params.param3,
                     command.params.param4,
                     static_cast<float>(command.params.x),
                     static_cast<float>(command.params.y)});

                if (result.has_value()) {
                    return _server_component_impl.make_command_ack_message(command, result.value());
                }
            }
            return {};
        }
    }

    return {};
}

} // namespace mavsdk
