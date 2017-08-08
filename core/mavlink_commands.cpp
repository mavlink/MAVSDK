#include "mavlink_commands.h"
#include "device_impl.h"
#include <future>
#include <memory>

namespace dronelink {

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

MavlinkCommands::MavlinkCommands(DeviceImpl *parent) :
    _parent(parent)
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_ACK,
        std::bind(&MavlinkCommands::receive_command_ack,
                  this, std::placeholders::_1), (void *)this);
}

MavlinkCommands::~MavlinkCommands()
{
    _parent->unregister_all_mavlink_message_handlers((void *)this);
}

MavlinkCommands::Result MavlinkCommands::send_command(uint16_t command,
                                                      const MavlinkCommands::Params params,
                                                      uint8_t target_system_id,
                                                      uint8_t target_component_id)
{
    // Some architectures sadly don't have the promises (yet). Therefore, we have this crude
    // while loop to wait until the async task is done.
#ifdef NO_PROMISES
    {
        std::lock_guard<std::mutex> lock(_promise_mutex);
        // We can't buffer with this implementation.
        if (_promise_state != PromiseState::IDLE) {
            return Result::BUSY;
        }

        _promise_state = PromiseState::BUSY;
        queue_command_async(command, params, target_system_id, target_component_id,
                            std::bind(&MavlinkCommands::_promise_receive_command_result,
                                      this, std::placeholders::_1, std::placeholders::_2));
    }
    while (true) {
        {
            std::lock_guard<std::mutex> lock(_promise_mutex);
            if (_promise_state == PromiseState::DONE) {
                _promise_state = PromiseState::IDLE;
                return _promise_last_result;
            }
        }
        // Check at 100 Hz.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
#else
    struct PromiseResult {
        Result result;
        float progress;
    };

    // We wrap the async call with a promise and future.
    std::shared_ptr<std::promise<PromiseResult>> prom =
                                                  std::make_shared<std::promise<PromiseResult>>();

    queue_command_async(command, params, target_system_id, target_component_id,
    [prom](Result result, float progress) {
        PromiseResult promise_result {};
        promise_result.result = result;
        promise_result.progress = progress;
        prom->set_value(promise_result);
    }
                       );

    std::future<PromiseResult> res = prom->get_future();
    while (true) {
        // Block now to wait for result.
        res.wait();

        PromiseResult promise_result = res.get();

        if (promise_result.result == Result::IN_PROGRESS) {
            Debug() << "In progress: " << promise_result.progress;
            continue;
        }
        return promise_result.result;
    }
#endif
}

#ifdef NO_PROMISES
void MavlinkCommands::_promise_receive_command_result(Result result, float progress)
{
    std::lock_guard<std::mutex> lock(_promise_mutex);
    if (_promise_state == PromiseState::BUSY) {
        if (result != Result::IN_PROGRESS) {
            _promise_state = PromiseState::DONE;
            _promise_last_result = result;
        } else {
            Debug() << "In progress: " << progress;
        }
    }
}
#endif

void MavlinkCommands::queue_command_async(uint16_t command,
                                          const MavlinkCommands::Params params,
                                          uint8_t target_system_id,
                                          uint8_t target_component_id,
                                          command_result_callback_t callback)
{
    // Debug() << "Command " << (int)command << " to send to " << (int)target_system_id << ", "
    //         << (int)target_component_id;

    Work new_work {};
    mavlink_msg_command_long_pack(_parent->get_own_system_id(),
                                  _parent->get_own_component_id(),
                                  &new_work.mavlink_message,
                                  target_system_id,
                                  target_component_id,
                                  command,
                                  0,
                                  params.v[0], params.v[1], params.v[2], params.v[3],
                                  params.v[4], params.v[5], params.v[6]);
    new_work.callback = callback;
    new_work.mavlink_command = command;
    _work_queue.push_back(new_work);
}

void MavlinkCommands::receive_command_ack(mavlink_message_t message)
{
    // If nothing is in the queue, we ignore the message all together.
    if (_work_queue.size() == 0) {
        return;
    }

    Work &work = _work_queue.front();

    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    // Debug() << "We got an ack: " << command_ack.command;

    if (work.mavlink_command != command_ack.command) {
        // If the command does not match with our current command, ignore it.
        Debug() << "Command ack not matching our current command: " << work.mavlink_command;
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
            Debug() << "command denied (" << work.mavlink_command << ").";
        // FALLTHRU

        case MAV_RESULT_UNSUPPORTED:
            Debug() << "command unsupported (" << work.mavlink_command << ").";
        // FALLTHRU

        case MAV_RESULT_TEMPORARILY_REJECTED:
            Debug() << "command temporarily rejected (" << work.mavlink_command << ").";
        // FALLTHRU
        case MAV_RESULT_FAILED:
            Debug() << "command failed (" << work.mavlink_command << ").";
            _state = State::FAILED;
            if (work.callback) {
                work.callback(Result::COMMAND_DENIED, NAN);
            }
            break;

        case MAV_RESULT_IN_PROGRESS:
            Debug() << "progress: " << (int)command_ack.progress
                    << " % (" << work.mavlink_command << ").";
            if (work.callback) {
                work.callback(Result::IN_PROGRESS, command_ack.progress / 100.0f);
            }
            _state = State::IN_PROGRESS;
            // If we get a progress update, we can raise the timeout
            // to something higher because we know the initial command
            // has arrived. A possible timeout for this case is the initial
            // timeout * the possible retries because this should match the
            // case where there is no progress update and we keep trying.
            _parent->unregister_timeout_handler((void *)this);
            _parent->register_timeout_handler(
                std::bind(&MavlinkCommands::receive_timeout, this),
                work.retries_to_do * work.timeout_s, (void *)this);
            break;
    }
}

void MavlinkCommands::receive_timeout()
{
    // If nothing is in the queue, we ignore the timeout.
    if (_work_queue.size() == 0) {
        return;
    }

    Work &work = _work_queue.front();

    std::lock_guard<std::mutex> lock(_state_mutex);

    if (_state == State::WAITING) {

        if (work.retries_to_do > 0) {

            Debug() << "sending again, retries to do: " << work.retries_to_do
                    << "  (" << work.mavlink_command << ").";
            // We're not sure the command arrived, let's retransmit.
            if (!_parent->send_message(work.mavlink_message)) {
                Debug() << "connection send error in retransmit (" << work.mavlink_command << ").";
                if (work.callback) {
                    work.callback(Result::CONNECTION_ERROR, NAN);
                }
                _state = State::FAILED;
            } else {
                --work.retries_to_do;
                _parent->register_timeout_handler(
                    std::bind(&MavlinkCommands::receive_timeout, this),
                    work.timeout_s, (void *)this);
            }

        } else  {
            // We have tried retransmitting, giving up now.
            Debug() << "Retrying failed (" << work.mavlink_command << ")";

            if (work.callback) {
                if (_state == State::WAITING) {
                    work.callback(Result::TIMEOUT, NAN);
                }
            }
            _state = State::FAILED;
        }
    }
}

void MavlinkCommands::do_work()
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
            _parent->unregister_timeout_handler((void *)this);
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
            // Debug() << "sending it the first time (" << work.mavlink_command << ")";
            if (!_parent->send_message(work.mavlink_message)) {
                Debug() << "connection send error (" << work.mavlink_command << ")";
                if (work.callback) {
                    work.callback(Result::CONNECTION_ERROR, NAN);
                }
                _state = State::FAILED;
                break;
            } else {
                _state = State::WAITING;
                _parent->register_timeout_handler(
                    std::bind(&MavlinkCommands::receive_timeout, this),
                    work.timeout_s, (void *)this);
            }
            break;
        case State::WAITING:
        case State::IN_PROGRESS:
            // Debug() << "wait until we can deal with this";
            break;
        case State::DONE:
        // FALLTHROUGH
        case State::FAILED:
            break;
    }
}


} // namespace dronelink
