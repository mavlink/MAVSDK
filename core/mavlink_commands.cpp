#include "mavlink_commands.h"
#include "mavlink_system.h"
#include <future>
#include <memory>

namespace dronecore {

// TODO: Currently the mavlink command handling is made in a way to only
//       process one command at any time. Therefore, the work state is global
//       for this whole class.
//       The limitation is made because:
//       - We are not sure what exactly will happen if the commands are sent in parallel
//         and what kind of edge cases we might run into.
//       - The timeout handler only supports the (void *)this cookie and therefore only
//         really supports one timeout per object. We could use (void *)this of the work
//         item but it also seems a bit dodgy.
//       - The queue used does not support going through and checking each and every
//         item yet.

MAVLinkCommands::MAVLinkCommands(MAVLinkSystem &parent) :
    _parent(parent)
{
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_ACK,
        std::bind(&MAVLinkCommands::receive_command_ack,
                  this, std::placeholders::_1), this);
}

MAVLinkCommands::~MAVLinkCommands()
{
    _parent.unregister_all_mavlink_message_handlers(this);
}

MAVLinkCommands::Result
MAVLinkCommands::send_command(const MAVLinkCommands::CommandInt &command)
{
    // We wrap the async call with a promise and future.
    auto prom = std::make_shared<std::promise<Result>>();
    auto res = prom->get_future();

    queue_command_async(command,
    [&prom](Result result, float progress) {
        UNUSED(progress);
        // We can only fulfill the promise once in C++11.
        // Therefore we have to ignore the IN_PROGRESS state and wait
        // for the final result.
        if (result != Result::IN_PROGRESS) {
            prom->set_value(result);
        }
    });

    // Block now to wait for result.
    return res.get();
}

MAVLinkCommands::Result
MAVLinkCommands::send_command(const MAVLinkCommands::CommandLong &command)
{
    // We wrap the async call with a promise and future.
    auto prom = std::make_shared<std::promise<Result>>();
    auto res = prom->get_future();

    queue_command_async(command,
    [&prom](Result result, float progress) {
        UNUSED(progress);
        // We can only fulfill the promise once in C++11.
        // Therefore we have to ignore the IN_PROGRESS state and wait
        // for the final result.
        if (result != Result::IN_PROGRESS) {
            prom->set_value(result);
        }
    });

    return res.get();
}

void
MAVLinkCommands::queue_command_async(const CommandInt &command,
                                     command_result_callback_t callback)
{
    // LogDebug() << "Command " << (int)(command.command) << " to send to "
    //  << (int)(command.target_system_id)<< ", " << (int)(command.target_component_id;

    Work new_work {};
    mavlink_msg_command_int_pack(GCSClient::system_id,
                                 GCSClient::component_id,
                                 &new_work.mavlink_message,
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

    new_work.callback = callback;
    new_work.mavlink_command = command.command;
    _work_queue.push_back(new_work);
}

void
MAVLinkCommands::queue_command_async(const CommandLong &command,
                                     command_result_callback_t callback)
{
    // LogDebug() << "Command " << (int)(command.command) << " to send to "
    //  << (int)(command.target_system_id)<< ", " << (int)(command.target_component_id;

    Work new_work {};
    mavlink_msg_command_long_pack(GCSClient::system_id,
                                  GCSClient::component_id,
                                  &new_work.mavlink_message,
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

    new_work.callback = callback;
    new_work.mavlink_command = command.command;
    _work_queue.push_back(new_work);
}

void MAVLinkCommands::receive_command_ack(mavlink_message_t message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    LogDebug() << "We got an ack: " << command_ack.command;

    auto work = _work_queue.borrow_front();
    if (!work) {
        return;
    }

    if (work->mavlink_command != command_ack.command) {
        // If the command does not match with our current command, ignore it.
        LogWarn() << "Command ack not matching our current command: " << work->mavlink_command;
        _work_queue.return_front();
        return;
    }

    std::lock_guard<std::mutex> lock(_state_mutex);
    switch (command_ack.result) {
        case MAV_RESULT_ACCEPTED:
            _state = State::NONE;
            if (work->callback) {
                work->callback(Result::SUCCESS, 1.0f);
            }
            _work_queue.pop_front();
            _parent.unregister_timeout_handler(_timeout_cookie);
            break;

        case MAV_RESULT_DENIED:
            LogWarn() << "command denied (" << work->mavlink_command << ").";
            _state = State::NONE;
            if (work->callback) {
                work->callback(Result::COMMAND_DENIED, NAN);
            }
            _work_queue.pop_front();
            _parent.unregister_timeout_handler(_timeout_cookie);
            break;

        case MAV_RESULT_UNSUPPORTED:
            LogWarn() << "command unsupported (" << work->mavlink_command << ").";
            _state = State::NONE;
            if (work->callback) {
                work->callback(Result::COMMAND_DENIED, NAN);
            }
            _work_queue.pop_front();
            _parent.unregister_timeout_handler(_timeout_cookie);
            break;

        case MAV_RESULT_TEMPORARILY_REJECTED:
            LogWarn() << "command temporarily rejected (" << work->mavlink_command << ").";
            _state = State::NONE;
            if (work->callback) {
                work->callback(Result::COMMAND_DENIED, NAN);
            }
            _work_queue.pop_front();
            _parent.unregister_timeout_handler(_timeout_cookie);
            break;

        case MAV_RESULT_FAILED:
            LogWarn() << "command failed (" << work->mavlink_command << ").";
            _state = State::NONE;
            if (work->callback) {
                work->callback(Result::COMMAND_DENIED, NAN);
            }
            _work_queue.pop_front();
            _parent.unregister_timeout_handler(_timeout_cookie);
            break;

        case MAV_RESULT_IN_PROGRESS:
            if (static_cast<int>(command_ack.progress) != 255) {
                LogInfo() << "progress: " << static_cast<int>(command_ack.progress)
                          << " % (" << work->mavlink_command << ").";
            }
            // FIXME: We can only call callbacks with promises once, so let's not do it
            //        on IN_PROGRESS.
            //if (work->callback) {
            //    work->callback(Result::IN_PROGRESS, command_ack.progress / 100.0f);
            //}
            _state = State::IN_PROGRESS;
            // If we get a progress update, we can raise the timeout
            // to something higher because we know the initial command
            // has arrived. A possible timeout for this case is the initial
            // timeout * the possible retries because this should match the
            // case where there is no progress update and we keep trying.
            _parent.unregister_timeout_handler(_timeout_cookie);
            _parent.register_timeout_handler(
                std::bind(&MAVLinkCommands::receive_timeout, this),
                work->retries_to_do * work->timeout_s, &_timeout_cookie);
            break;
    }
}

void MAVLinkCommands::receive_timeout()
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state != State::WAITING) {
        LogWarn() << "Received command timeout but not waiting.";
        return;
    }

    // If we're not waiting, we ignore this.
    auto work = _work_queue.borrow_front();

    if (!work) {
        // Nevermind, there is nothing to do.
        return;
    }

    if (work->retries_to_do > 0) {

        // We're not sure the command arrived, let's retransmit.
        LogWarn() << "sending again, retries to do: " << work->retries_to_do
                  << "  (" << work->mavlink_command << ").";
        if (!_parent.send_message(work->mavlink_message)) {
            LogErr() << "connection send error in retransmit (" << work->mavlink_command << ").";
            if (work->callback) {
                work->callback(Result::CONNECTION_ERROR, NAN);
            }
            _state = State::NONE;
            _work_queue.pop_front();

        } else {
            --work->retries_to_do;
            _parent.register_timeout_handler(
                std::bind(&MAVLinkCommands::receive_timeout, this),
                work->timeout_s, &_timeout_cookie);
            _work_queue.return_front();
        }

    } else  {
        // We have tried retransmitting, giving up now.
        LogErr() << "Retrying failed (" << work->mavlink_command << ")";

        if (work->callback) {
            if (_state == State::WAITING) {
                work->callback(Result::TIMEOUT, NAN);
            }
        }
        _state = State::NONE;
        _work_queue.pop_front();
    }
}

void MAVLinkCommands::do_work()
{
    auto work = _work_queue.borrow_front();
    if (!work) {
        // Nothing to do.
        return;
    }

    std::lock_guard<std::mutex> lock(_state_mutex);

    // If the work state is none, we can start the next command.
    switch (_state) {
        case State::NONE:
            // LogDebug() << "sending it the first time (" << work->mavlink_command << ")";
            if (!_parent.send_message(work->mavlink_message)) {
                LogErr() << "connection send error (" << work->mavlink_command << ")";
                if (work->callback) {
                    work->callback(Result::CONNECTION_ERROR, NAN);
                }
                _work_queue.pop_front();
                _state = State::NONE;
                break;
            } else {
                _state = State::WAITING;
                _parent.register_timeout_handler(
                    std::bind(&MAVLinkCommands::receive_timeout, this),
                    work->timeout_s, &_timeout_cookie);
                _work_queue.return_front();
            }
            break;
        case State::WAITING:
        // FALLTHROUGH
        case State::IN_PROGRESS:
            _work_queue.return_front();
            break;
    }
}


} // namespace dronecore
