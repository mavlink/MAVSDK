#include "mavlink_commands.h"
#include "system_impl.h"
#include <cmath>
#include <future>
#include <memory>

namespace mavsdk {

MavlinkCommandSender::CommandLong::CommandLong(const SystemImpl& system_impl)
{
    const float param_unset = [&]() {
        if (system_impl.autopilot() == SystemImpl::Autopilot::ArduPilot) {
            return 0.0f;
        } else {
            return NAN;
        }
    }();

    params.param1 = param_unset;
    params.param2 = param_unset;
    params.param3 = param_unset;
    params.param4 = param_unset;
    params.param5 = param_unset;
    params.param6 = param_unset;
    params.param7 = param_unset;
}

MavlinkCommandSender::MavlinkCommandSender(SystemImpl& system_impl) : _parent(system_impl)
{
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_ACK,
        std::bind(&MavlinkCommandSender::receive_command_ack, this, std::placeholders::_1),
        this);
}

MavlinkCommandSender::~MavlinkCommandSender()
{
    _parent.unregister_all_mavlink_message_handlers(this);
}

MavlinkCommandSender::Result
MavlinkCommandSender::send_command(const MavlinkCommandSender::CommandInt& command)
{
    // We wrap the async call with a promise and future.
    auto prom = std::make_shared<std::promise<Result>>();
    auto res = prom->get_future();

    queue_command_async(command, [prom](Result result, float progress) {
        UNUSED(progress);
        // We can only fulfill the promise once in C++11.
        // Therefore we have to ignore the IN_PROGRESS state and wait
        // for the final result.
        if (result != Result::InProgress) {
            prom->set_value(result);
        }
    });

    // Block now to wait for result.
    return res.get();
}

MavlinkCommandSender::Result
MavlinkCommandSender::send_command(const MavlinkCommandSender::CommandLong& command)
{
    // We wrap the async call with a promise and future.
    auto prom = std::make_shared<std::promise<Result>>();
    auto res = prom->get_future();

    queue_command_async(command, [prom](Result result, float progress) {
        UNUSED(progress);
        // We can only fulfill the promise once in C++11.
        // Therefore we have to ignore the IN_PROGRESS state and wait
        // for the final result.
        if (result != Result::InProgress) {
            prom->set_value(result);
        }
    });

    return res.get();
}

void MavlinkCommandSender::queue_command_async(
    const CommandInt& command, CommandResultCallback callback)
{
    // LogDebug() << "Command " << (int)(command.command) << " to send to "
    //  << (int)(command.target_system_id)<< ", " << (int)(command.target_component_id);

    auto new_work = std::make_shared<Work>();
    new_work->timeout_s = _parent.timeout_s();
    new_work->command = command;
    new_work->mavlink_command = command.command;
    new_work->callback = callback;
    _work_queue.push_back(new_work);
}

void MavlinkCommandSender::queue_command_async(
    const CommandLong& command, CommandResultCallback callback)
{
    // LogDebug() << "Command " << (int)(command.command) << " to send to "
    //  << (int)(command.target_system_id)<< ", " << (int)(command.target_component_id);

    auto new_work = std::make_shared<Work>();
    new_work->timeout_s = _parent.timeout_s();
    new_work->command = command;
    new_work->mavlink_command = command.command;
    new_work->callback = callback;
    new_work->time_started = _parent.get_time().steady_time();
    _work_queue.push_back(new_work);
}

void MavlinkCommandSender::receive_command_ack(mavlink_message_t message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    if ((command_ack.target_system && command_ack.target_system != _parent.get_own_system_id()) ||
        (command_ack.target_component &&
         command_ack.target_component != _parent.get_own_component_id())) {
        return;
    }

    CommandResultCallback temp_callback = nullptr;
    std::pair<Result, float> temp_result{Result::UnknownError, NAN};

    {
        std::lock_guard<std::mutex> lock(_sent_commands_mutex);

        if (_sent_commands.find(command_ack.command) == _sent_commands.end()) {
            LogWarn() << "Received ack for unexisting command: "
                      << static_cast<int>(command_ack.command) << " from "
                      << static_cast<int>(message.sysid) << "/" << static_cast<int>(message.compid)
                      << "! Ignoring...";
            return;
        }

        auto work = _sent_commands.at(command_ack.command);

        if (!work) {
            return;
        }

        // LogDebug() << "We got an ack: " << command_ack.command << " after: "
        //     << _parent.get_time().elapsed_since_s(work->time_started) << " s";
        temp_callback = work->callback;

        switch (command_ack.result) {
            case MAV_RESULT_ACCEPTED:
                _parent.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Success, 1.0f};
                _sent_commands.erase(work->mavlink_command);
                break;

            case MAV_RESULT_DENIED:
                LogWarn() << "command denied (" << work->mavlink_command << ") from "
                          << static_cast<int>(message.sysid) << "/"
                          << static_cast<int>(message.compid);
                _parent.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::CommandDenied, NAN};
                _sent_commands.erase(work->mavlink_command);
                break;

            case MAV_RESULT_UNSUPPORTED:
                LogWarn() << "command unsupported (" << work->mavlink_command << ") from "
                          << static_cast<int>(message.sysid) << "/"
                          << static_cast<int>(message.compid);
                _parent.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Unsupported, NAN};
                _sent_commands.erase(work->mavlink_command);
                break;

            case MAV_RESULT_TEMPORARILY_REJECTED:
                LogWarn() << "command temporarily rejected (" << work->mavlink_command << " from "
                          << static_cast<int>(message.sysid) << "/"
                          << static_cast<int>(message.compid);
                _parent.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::CommandDenied, NAN};
                _sent_commands.erase(work->mavlink_command);
                break;

            case MAV_RESULT_FAILED:
                _parent.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::CommandDenied, NAN};
                _sent_commands.erase(work->mavlink_command);
                break;

            case MAV_RESULT_IN_PROGRESS:
                if (static_cast<int>(command_ack.progress) != 255) {
                    LogInfo() << "progress: " << static_cast<int>(command_ack.progress) << " % ("
                              << work->mavlink_command << ").";
                }
                // If we get a progress update, we can raise the timeout
                // to something higher because we know the initial command
                // has arrived. A possible timeout for this case is the initial
                // timeout * the possible retries because this should match the
                // case where there is no progress update and we keep trying.
                _parent.unregister_timeout_handler(work->timeout_cookie);
                _parent.register_timeout_handler(
                    std::bind(&MavlinkCommandSender::receive_timeout, this, work->mavlink_command),
                    work->retries_to_do * work->timeout_s,
                    &work->timeout_cookie);

                temp_result = {Result::InProgress, command_ack.progress / 100.0f};
                break;

            default:
                LogWarn() << "Received unknown ack.";
                break;
        }
    }

    if (temp_callback != nullptr) {
        call_callback(temp_callback, temp_result.first, temp_result.second);
    }
}

void MavlinkCommandSender::receive_timeout(const uint16_t command)
{
    CommandResultCallback temp_callback = nullptr;
    std::pair<Result, float> temp_result{Result::UnknownError, NAN};

    {
        std::lock_guard<std::mutex> lock(_sent_commands_mutex);

        if (_sent_commands.find(command) == _sent_commands.end()) {
            LogWarn() << "Timeout for unexisting command: " << static_cast<int>(command)
                      << "! Ignoring...";
            return;
        }

        auto work = _sent_commands.at(command);

        if (!work) {
            LogErr() << "Received timeout without item in queue.";
            return;
        }

        if (work->retries_to_do > 0) {
            // We're not sure the command arrived, let's retransmit.
            LogWarn() << "sending again after "
                      << _parent.get_time().elapsed_since_s(work->time_started)
                      << " s, retries to do: " << work->retries_to_do << "  ("
                      << work->mavlink_command << ").";

            mavlink_message_t message = create_mavlink_message(work->command);
            if (!_parent.send_message(message)) {
                LogErr() << "connection send error in retransmit (" << work->mavlink_command
                         << ").";
                temp_callback = work->callback;
                temp_result = {Result::ConnectionError, NAN};
                _sent_commands.erase(work->mavlink_command);
            } else {
                --work->retries_to_do;
                _parent.register_timeout_handler(
                    std::bind(&MavlinkCommandSender::receive_timeout, this, work->mavlink_command),
                    work->timeout_s,
                    &work->timeout_cookie);
            }

        } else {
            // We have tried retransmitting, giving up now.
            LogErr() << "Retrying failed (" << work->mavlink_command << ")";

            temp_callback = work->callback;
            temp_result = {Result::ConnectionError, NAN};
            _sent_commands.erase(work->mavlink_command);
        }
    }

    if (temp_callback != nullptr) {
        call_callback(temp_callback, temp_result.first, temp_result.second);
    }
}

void MavlinkCommandSender::do_work()
{
    LockedQueue<Work>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        // Nothing to do.
        return;
    }

    if (work->already_sent) {
        return;
    }

    // LogDebug() << "sending it the first time (" << work->mavlink_command << ")";
    work->time_started = _parent.get_time().steady_time();

    {
        std::lock_guard<std::mutex> lock(_sent_commands_mutex);
        const auto was_inserted =
            _sent_commands.insert(std::make_pair(work->mavlink_command, work)).second;

        // The command is inserted in the list only if another command with the same id does
        // not exist. If such an element exists, we refuse to send the new command.
        if (!was_inserted) {
            // LogWarn()
            //     << "A command cannot be sent if another command with the same command_id is still
            //     processing! Ignoring command "
            //     << static_cast<int>(work->mavlink_command);
            auto temp_callback = work->callback;
            auto temp_result = std::make_pair<Result, float>(Result::CommandDenied, NAN);

            if (temp_callback != nullptr) {
                call_callback(temp_callback, temp_result.first, temp_result.second);
            }

            return;
        }

        mavlink_message_t message = create_mavlink_message(work->command);
        if (!_parent.send_message(message)) {
            LogErr() << "connection send error (" << work->mavlink_command << ")";
            auto temp_callback = work->callback;
            auto temp_result = std::make_pair<Result, float>(Result::ConnectionError, NAN);

            if (temp_callback != nullptr) {
                call_callback(temp_callback, temp_result.first, temp_result.second);
            }

            _sent_commands.erase(work->mavlink_command);

            return;
        }
    }

    work->already_sent = true;
    _parent.register_timeout_handler(
        std::bind(&MavlinkCommandSender::receive_timeout, this, work->mavlink_command),
        work->timeout_s,
        &work->timeout_cookie);

    work_queue_guard.pop_front();
}

void MavlinkCommandSender::call_callback(
    const CommandResultCallback& callback, Result result, float progress)
{
    if (!callback) {
        return;
    }

    // It seems that we need to queue the callback on the thread pool otherwise
    // we lock ourselves out when we send a command in the callback receiving a command result.
    _parent.call_user_callback([callback, result, progress]() { callback(result, progress); });
}

mavlink_message_t MavlinkCommandSender::create_mavlink_message(const Command& command)
{
    mavlink_message_t message;

    if (auto command_int = std::get_if<CommandInt>(&command)) {
        mavlink_msg_command_int_pack(
            _parent.get_own_system_id(),
            _parent.get_own_component_id(),
            &message,
            command_int->target_system_id,
            command_int->target_component_id,
            command_int->frame,
            command_int->command,
            command_int->current,
            command_int->autocontinue,
            command_int->params.param1,
            command_int->params.param2,
            command_int->params.param3,
            command_int->params.param4,
            command_int->params.x,
            command_int->params.y,
            command_int->params.z);

    } else if (auto command_long = std::get_if<CommandLong>(&command)) {
        mavlink_msg_command_long_pack(
            _parent.get_own_system_id(),
            _parent.get_own_component_id(),
            &message,
            command_long->target_system_id,
            command_long->target_component_id,
            command_long->command,
            command_long->confirmation,
            command_long->params.param1,
            command_long->params.param2,
            command_long->params.param3,
            command_long->params.param4,
            command_long->params.param5,
            command_long->params.param6,
            command_long->params.param7);
    }
    return message;
}

MavlinkCommandReceiver::MavlinkCommandReceiver(SystemImpl& system_impl) : _parent(system_impl)
{
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_LONG,
        std::bind(&MavlinkCommandReceiver::receive_command_long, this, std::placeholders::_1),
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_INT,
        std::bind(&MavlinkCommandReceiver::receive_command_int, this, std::placeholders::_1),
        this);
}

MavlinkCommandReceiver::~MavlinkCommandReceiver()
{
    unregister_all_mavlink_command_handlers(this);

    _parent.unregister_all_mavlink_message_handlers(this);
}

void MavlinkCommandReceiver::receive_command_int(const mavlink_message_t& message)
{
    MavlinkCommandReceiver::CommandInt cmd(message);

    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    for (auto it = _mavlink_command_int_handler_table.begin();
         it != _mavlink_command_int_handler_table.end();
         ++it) {
        if (it->cmd_id == cmd.command) {
            // The client side can pack a COMMAND_ACK as a response to receiving the command.
            auto maybe_message = it->callback(cmd);
            if (maybe_message) {
                _parent.send_message(maybe_message.value());
            }
        }
    }
}

void MavlinkCommandReceiver::receive_command_long(const mavlink_message_t& message)
{
    MavlinkCommandReceiver::CommandLong cmd(message);

    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    for (auto it = _mavlink_command_long_handler_table.begin();
         it != _mavlink_command_long_handler_table.end();
         ++it) {
        if (it->cmd_id == cmd.command) {
            // The client side can pack a COMMAND_ACK as a response to receiving the command.
            auto maybe_message = it->callback(cmd);
            if (maybe_message) {
                _parent.send_message(maybe_message.value());
            }
        }
    }
}

void MavlinkCommandReceiver::register_mavlink_command_handler(
    uint16_t cmd_id, MavlinkCommandIntHandler callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_command_handler_table_mutex);

    MAVLinkCommandIntHandlerTableEntry entry = {cmd_id, callback, cookie};
    _mavlink_command_int_handler_table.push_back(entry);
}

void MavlinkCommandReceiver::register_mavlink_command_handler(
    uint16_t cmd_id, MavlinkCommandLongHandler callback, const void* cookie)
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
