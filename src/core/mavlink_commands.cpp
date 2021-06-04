#include "mavlink_commands.h"
#include "system_impl.h"
#include <future>
#include <memory>

namespace mavsdk {

// TODO: Currently the mavlink command handling is made in a way to only
//       process one command at any time. Therefore, the work state is global
//       for this whole class.
//       The limitation is made because:
//       - We are not sure what exactly will happen if the commands are sent in parallel
//         and what kind of edge cases we might run into.
//       - The queue used does not support going through and checking each and every
//         item yet.

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

    auto new_work = std::make_shared<Work>(_parent.timeout_s());

    mavlink_msg_command_int_pack(
        _parent.get_own_system_id(),
        _parent.get_own_component_id(),
        &new_work->mavlink_message,
        command.target_system_id,
        command.target_component_id,
        command.frame,
        command.command,
        command.current,
        command.autocontinue,
        command.params.param1,
        command.params.param2,
        command.params.param3,
        command.params.param4,
        command.params.x,
        command.params.y,
        command.params.z);

    new_work->callback = callback;
    new_work->mavlink_command = command.command;
    _work_queue.push_back(new_work);
}

void MavlinkCommandSender::queue_command_async(
    const CommandLong& command, CommandResultCallback callback)
{
    // LogDebug() << "Command " << (int)(command.command) << " to send to "
    //  << (int)(command.target_system_id)<< ", " << (int)(command.target_component_id);

    auto new_work = std::make_shared<Work>(_parent.timeout_s());
    mavlink_msg_command_long_pack(
        _parent.get_own_system_id(),
        _parent.get_own_component_id(),
        &new_work->mavlink_message,
        command.target_system_id,
        command.target_component_id,
        command.command,
        command.confirmation,
        command.params.param1,
        command.params.param2,
        command.params.param3,
        command.params.param4,
        command.params.param5,
        command.params.param6,
        command.params.param7);

    new_work->callback = callback;
    new_work->mavlink_command = command.command;
    new_work->time_started = _parent.get_time().steady_time();
    _work_queue.push_back(new_work);
}

void MavlinkCommandSender::receive_command_ack(mavlink_message_t message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    if (command_ack.target_system != _parent.get_own_system_id() &&
        command_ack.target_component != _parent.get_own_component_id()) {
        return;
    }

    CommandResultCallback temp_callback = nullptr;
    std::pair<Result, float> temp_result{Result::UnknownError, NAN};

    {
        LockedQueue<Work>::Guard work_queue_guard(_work_queue);
        auto work = work_queue_guard.get_front();

        if (!work) {
            return;
        }

        if (work->mavlink_command != command_ack.command) {
            // If the command does not match with our current command, ignore it.
            LogWarn() << "Command ack " << int(command_ack.command)
                      << " not matching our current command: " << work->mavlink_command;
            return;
        }

        // LogDebug() << "We got an ack: " << command_ack.command << " after: "
        //     << _parent.get_time().elapsed_since_s(work->time_started) << " s";
        temp_callback = work->callback;

        switch (command_ack.result) {
            case MAV_RESULT_ACCEPTED:
                _parent.unregister_timeout_handler(_timeout_cookie);
                temp_result = {Result::Success, 1.0f};
                work_queue_guard.pop_front();
                break;

            case MAV_RESULT_DENIED:
                LogWarn() << "command denied (" << work->mavlink_command << ").";
                _parent.unregister_timeout_handler(_timeout_cookie);
                temp_result = {Result::CommandDenied, NAN};
                work_queue_guard.pop_front();
                break;

            case MAV_RESULT_UNSUPPORTED:
                LogWarn() << "command unsupported (" << work->mavlink_command << ").";
                _parent.unregister_timeout_handler(_timeout_cookie);
                temp_result = {Result::Unsupported, NAN};
                work_queue_guard.pop_front();
                break;

            case MAV_RESULT_TEMPORARILY_REJECTED:
                LogWarn() << "command temporarily rejected (" << work->mavlink_command << ").";
                _parent.unregister_timeout_handler(_timeout_cookie);
                temp_result = {Result::CommandDenied, NAN};
                work_queue_guard.pop_front();
                break;

            case MAV_RESULT_FAILED:
                _parent.unregister_timeout_handler(_timeout_cookie);
                temp_result = {Result::CommandDenied, NAN};
                work_queue_guard.pop_front();
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
                _parent.unregister_timeout_handler(_timeout_cookie);
                _parent.register_timeout_handler(
                    std::bind(&MavlinkCommandSender::receive_timeout, this),
                    work->retries_to_do * work->timeout_s,
                    &_timeout_cookie);

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

void MavlinkCommandSender::receive_timeout()
{
    CommandResultCallback temp_callback = nullptr;
    std::pair<Result, float> temp_result{Result::UnknownError, NAN};

    {
        // If we're not waiting, we ignore this.
        LockedQueue<Work>::Guard work_queue_guard(_work_queue);
        auto work = work_queue_guard.get_front();

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
            if (!_parent.send_message(work->mavlink_message)) {
                LogErr() << "connection send error in retransmit (" << work->mavlink_command
                         << ").";
                temp_callback = work->callback;
                temp_result = {Result::ConnectionError, NAN};
                work_queue_guard.pop_front();

            } else {
                --work->retries_to_do;
                _parent.register_timeout_handler(
                    std::bind(&MavlinkCommandSender::receive_timeout, this),
                    work->timeout_s,
                    &_timeout_cookie);
            }

        } else {
            // We have tried retransmitting, giving up now.
            LogErr() << "Retrying failed (" << work->mavlink_command << ")";

            temp_callback = work->callback;
            temp_result = {Result::ConnectionError, NAN};
            work_queue_guard.pop_front();
        }
    }

    if (temp_callback != nullptr) {
        call_callback(temp_callback, temp_result.first, temp_result.second);
    }
}

void MavlinkCommandSender::do_work()
{
    CommandResultCallback temp_callback = nullptr;
    std::pair<Result, float> temp_result{Result::UnknownError, NAN};

    {
        LockedQueue<Work>::Guard work_queue_guard(_work_queue);
        auto work = work_queue_guard.get_front();

        if (!work) {
            // Nothing to do.
            return;
        }

        if (!work->already_sent) {
            // LogDebug() << "sending it the first time (" << work->mavlink_command << ")";
            work->time_started = _parent.get_time().steady_time();
            if (!_parent.send_message(work->mavlink_message)) {
                LogErr() << "connection send error (" << work->mavlink_command << ")";
                temp_callback = work->callback;
                temp_result = {Result::ConnectionError, NAN};
                work_queue_guard.pop_front();
            } else {
                work->already_sent = true;
                _parent.register_timeout_handler(
                    std::bind(&MavlinkCommandSender::receive_timeout, this),
                    work->timeout_s,
                    &_timeout_cookie);
            }
        }
    }

    if (temp_callback != nullptr) {
        call_callback(temp_callback, temp_result.first, temp_result.second);
    }
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
