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
MAVLinkCommands::send_command(MAVLinkCommands::CommandInt &cmd)
{
    struct PromiseResult {
        Result result;
        float progress;
    };

    // We wrap the async call with a promise and future.
    std::shared_ptr<std::promise<PromiseResult>> prom =
                                                  std::make_shared<std::promise<PromiseResult>>();

    queue_command_async(cmd,
    [prom](Result result, float progress) {
        PromiseResult promise_result {};
        promise_result.result = result;
        promise_result.progress = progress;
        prom->set_value(promise_result);
    });

    std::future<PromiseResult> res = prom->get_future();
    while (true) {
        // Block now to wait for result.
        res.wait();

        PromiseResult promise_result = res.get();

        if (promise_result.result == Result::IN_PROGRESS) {
            LogInfo() << "In progress: " << promise_result.progress;
            continue;
        }
        return promise_result.result;
    }
}

MAVLinkCommands::Result
MAVLinkCommands::send_command(MAVLinkCommands::CommandLong &cmd)
{
    struct PromiseResult {
        Result result;
        float progress;
    };

    // We wrap the async call with a promise and future.
    std::shared_ptr<std::promise<PromiseResult>> prom =
                                                  std::make_shared<std::promise<PromiseResult>>();

    cmd.target_system_id = _parent.get_system_id();

    queue_command_async(cmd,
    [prom](Result result, float progress) {
        PromiseResult promise_result {};
        promise_result.result = result;
        promise_result.progress = progress;
        prom->set_value(promise_result);
    });

    std::future<PromiseResult> res = prom->get_future();
    while (true) {
        // Block now to wait for result.
        res.wait();

        PromiseResult promise_result = res.get();

        if (promise_result.result == Result::IN_PROGRESS) {
            LogInfo() << "In progress: " << promise_result.progress;
            continue;
        }
        return promise_result.result;
    }
}



void
MAVLinkCommands::queue_command_async(CommandInt &cmd,
                                     command_result_callback_t callback)
{
    // LogDebug() << "Command " << (int)(cmd.command) << " to send to "
    //  << (int)(cmd.target_system_id)<< ", " << (int)(cmd.target_component_id;

    Work new_work {};
    mavlink_msg_command_int_pack(GCSClient::system_id,
                                 GCSClient::component_id,
                                 &new_work.mavlink_message,
                                 cmd.target_system_id,
                                 cmd.target_component_id,
                                 cmd.frame,
                                 cmd.command,
                                 cmd.current,
                                 cmd.autocontinue,
                                 cmd.params.param1,
                                 cmd.params.param2,
                                 cmd.params.param3,
                                 cmd.params.param4,
                                 cmd.params.lat_deg,
                                 cmd.params.lon_deg,
                                 cmd.params.alt_m);

    new_work.callback = callback;
    new_work.mavlink_command = cmd.command;
    _work_queue.push_back(new_work);
}

void
MAVLinkCommands::queue_command_async(CommandLong &cmd,
                                     command_result_callback_t callback)
{
    // LogDebug() << "Command " << (int)(cmd.command) << " to send to "
    //  << (int)(cmd.target_system_id)<< ", " << (int)(cmd.target_component_id;

    Work new_work {};
    mavlink_msg_command_long_pack(GCSClient::system_id,
                                  GCSClient::component_id,
                                  &new_work.mavlink_message,
                                  cmd.target_system_id,
                                  cmd.target_component_id,
                                  cmd.command,
                                  cmd.confirmation,
                                  cmd.params.param1,
                                  cmd.params.param2,
                                  cmd.params.param3,
                                  cmd.params.param4,
                                  cmd.params.param5,
                                  cmd.params.param6,
                                  cmd.params.param7);

    new_work.callback = callback;
    new_work.mavlink_command = cmd.command;
    _work_queue.push_back(new_work);
}

void MAVLinkCommands::receive_command_ack(mavlink_message_t message)
{
    // If nothing is in the queue, we ignore the message all together.
    if (_work_queue.size() == 0) {
        return;
    }

    Work &work = _work_queue.front();

    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    // LogDebug() << "We got an ack: " << command_ack.command;

    if (work.mavlink_command != command_ack.command) {
        // If the command does not match with our current command, ignore it.
        LogWarn() << "Command ack not matching our current command: " << work.mavlink_command;
        return;
    }

    std::lock_guard<std::mutex> lock(_state_mutex);
    switch (command_ack.result) {
        case MAV_RESULT_ACCEPTED:
            _state = State::DONE;
            if (work.callback) {
                work.callback(Result::SUCCESS, 1.0f);
            }
            break;

        case MAV_RESULT_DENIED:
            LogWarn() << "command denied (" << work.mavlink_command << ").";
            _state = State::FAILED;
            if (work.callback) {
                work.callback(Result::COMMAND_DENIED, NAN);
            }
            break;

        case MAV_RESULT_UNSUPPORTED:
            LogWarn() << "command unsupported (" << work.mavlink_command << ").";
            _state = State::FAILED;
            if (work.callback) {
                work.callback(Result::COMMAND_DENIED, NAN);
            }
            break;

        case MAV_RESULT_TEMPORARILY_REJECTED:
            LogWarn() << "command temporarily rejected (" << work.mavlink_command << ").";
            _state = State::FAILED;
            if (work.callback) {
                work.callback(Result::COMMAND_DENIED, NAN);
            }
            break;

        case MAV_RESULT_FAILED:
            LogWarn() << "command failed (" << work.mavlink_command << ").";
            _state = State::FAILED;
            if (work.callback) {
                work.callback(Result::COMMAND_DENIED, NAN);
            }
            break;

        case MAV_RESULT_IN_PROGRESS:
            if (static_cast<int>(command_ack.progress) != 255) {
                LogInfo() << "progress: " << static_cast<int>(command_ack.progress)
                          << " % (" << work.mavlink_command << ").";
            }
            // FIXME: We can only call callbacks with promises once, so let's not do it
            //        on IN_PROGRESS.
            //if (work.callback) {
            //    work.callback(Result::IN_PROGRESS, command_ack.progress / 100.0f);
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
                work.retries_to_do * work.timeout_s, &_timeout_cookie);
            break;
    }
}

void MAVLinkCommands::receive_timeout()
{
    // If nothing is in the queue, we ignore the timeout.
    if (_work_queue.size() == 0) {
        return;
    }

    Work &work = _work_queue.front();

    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state == State::WAITING) {

        if (work.retries_to_do > 0) {

            LogInfo() << "sending again, retries to do: " << work.retries_to_do
                      << "  (" << work.mavlink_command << ").";
            // We're not sure the command arrived, let's retransmit.
            if (!_parent.send_message(work.mavlink_message)) {
                LogErr() << "connection send error in retransmit (" << work.mavlink_command << ").";
                if (work.callback) {
                    work.callback(Result::CONNECTION_ERROR, NAN);
                }
                _state = State::FAILED;
            } else {
                --work.retries_to_do;
                _parent.register_timeout_handler(
                    std::bind(&MAVLinkCommands::receive_timeout, this),
                    work.timeout_s, &_timeout_cookie);
            }

        } else  {
            // We have tried retransmitting, giving up now.
            LogErr() << "Retrying failed (" << work.mavlink_command << ")";

            if (work.callback) {
                if (_state == State::WAITING) {
                    work.callback(Result::TIMEOUT, NAN);
                }
            }
            _state = State::FAILED;
        }
    }
}

void MAVLinkCommands::do_work()
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    // Clean up first
    switch (_state) {
        case State::NONE:
        // FALLTHROUGH
        case State::WAITING:
        // FALLTHROUGH
        case State::IN_PROGRESS:
            break;
        case State::DONE:
        // FALLTHROUGH
        case State::FAILED:
            _parent.unregister_timeout_handler(_timeout_cookie);
            _work_queue.pop_front();
            _state = State::NONE;
            break;
    }

    // Check if there is work to do.
    if (_work_queue.size() == 0) {
        // Nothing to do.
        return;
    }

    // If so, let's get the latest.
    Work &work = _work_queue.front();

    // If the work state is none, we can start the next command.
    switch (_state) {
        case State::NONE:
            // LogDebug() << "sending it the first time (" << work.mavlink_command << ")";
            if (!_parent.send_message(work.mavlink_message)) {
                LogErr() << "connection send error (" << work.mavlink_command << ")";
                if (work.callback) {
                    work.callback(Result::CONNECTION_ERROR, NAN);
                }
                _state = State::FAILED;
                break;
            } else {
                _state = State::WAITING;
                _parent.register_timeout_handler(
                    std::bind(&MAVLinkCommands::receive_timeout, this),
                    work.timeout_s, &_timeout_cookie);
            }
            break;
        case State::WAITING:
        case State::IN_PROGRESS:
            // LogWarn() << "wait until we can deal with this";
            break;
        case State::DONE:
        // FALLTHROUGH
        case State::FAILED:
            break;
    }
}


} // namespace dronecore
