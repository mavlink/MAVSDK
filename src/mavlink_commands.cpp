#include "mavlink_commands.h"
#include "device_impl.h"
#include <future>
#include <memory>

namespace dronelink {

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
    // We wrap the async call with a promise and future.
    std::shared_ptr<std::promise<Result>> prom =
                                           std::make_shared<std::promise<Result>>();

    queue_command_async(command, params, target_system_id, target_component_id,
    [prom](Result result) {
        prom->set_value(result);
    }
                       );

    std::future<Result> res = prom->get_future();
    // Block now to wait for result.
    res.wait();
    return res.get();
}

void MavlinkCommands::queue_command_async(uint16_t command,
                                          const MavlinkCommands::Params params,
                                          uint8_t target_system_id,
                                          uint8_t target_component_id,
                                          command_result_callback_t callback)
{
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

    if (work.mavlink_command != command_ack.command) {
        // If the command does not match with our current command, ignore it.
        return;
    }

    switch (command_ack.result) {
        case MAV_RESULT_ACCEPTED:
            work.state = Work::State::DONE;
            if (work.callback) {
                work.callback(Result::SUCCESS);
            }
            break;

        case MAV_RESULT_TEMPORARILY_REJECTED:
            Debug() << "command temporarily rejected.";
            // The timeout will trigger and re-transmit the message.
            work.state = Work::State::TEMPORARILY_REJECTED;
            break;

        case MAV_RESULT_DENIED:
            Debug() << "command denied.";
        // no break

        case MAV_RESULT_UNSUPPORTED:
            Debug() << "command unsupported.";
        // no break

        case MAV_RESULT_FAILED:
            Debug() << "command failed.";
            work.state = Work::State::FAILED;
            if (work.callback) {
                work.callback(Result::COMMAND_DENIED);
            }
            break;

        case MAV_RESULT_IN_PROGRESS:
            Debug() << "progress: " << (int)command_ack.progress << " %";
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

    if (work.state == Work::State::WAITING ||
        work.state == Work::State::TEMPORARILY_REJECTED) {


        if (work.retries_to_do > 0) {

            // We're not sure the command arrived, let's retransmit.
            if (!_parent->send_message(work.mavlink_message)) {
                Debug() << "connection send error in retransmit";
                if (work.callback) {
                    work.callback(Result::CONNECTION_ERROR);
                }
                work.state = Work::State::FAILED;
            } else {
                --work.retries_to_do;
                _parent->register_timeout_handler(
                    std::bind(&MavlinkCommands::receive_timeout, this),
                    work.timeout_s, (void *)this);
            }

        } else  {
            // We have tried retransmitting, giving up now.
            Debug() << "Retrying failed (command: " << work.mavlink_command << ")";

            if (work.callback) {
                if (work.state == Work::State::WAITING) {
                    work.callback(Result::TIMEOUT);
                } else if (work.state == Work::State::TEMPORARILY_REJECTED) {
                    work.callback(Result::COMMAND_DENIED);
                }
            }
            work.state = Work::State::FAILED;
        }
    }
}

void MavlinkCommands::do_work()
{
    if (_work_queue.size() == 0) {
        // Nothing to do.
        return;
    }

    Work &work = _work_queue.front();

    switch (work.state) {
        case Work::State::NONE:
            if (!_parent->send_message(work.mavlink_message)) {
                Debug() << "connection send error";
                if (work.callback) {
                    work.callback(Result::CONNECTION_ERROR);
                }
                work.state = Work::State::FAILED;
                break;
            } else {
                work.state = Work::State::WAITING;
                _parent->register_timeout_handler(
                    std::bind(&MavlinkCommands::receive_timeout, this),
                    work.timeout_s, (void *)this);
            }
            break;
        case Work::State::WAITING:
        case Work::State::IN_PROGRESS:
        case Work::State::TEMPORARILY_REJECTED:
            // Nothing to do yet, timeout will be triggered
            // anyway for a retransmission.
            break;
        case Work::State::DONE:
        case Work::State::FAILED:
            _parent->unregister_timeout_handler((void *)this);
            _work_queue.pop_front();
            break;
    }
}


} // namespace dronelink
