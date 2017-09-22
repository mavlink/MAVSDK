#include "mavlink_commands.h"
#include "device_impl.h"
#include <future>
#include <memory>
#include <algorithm>
#include <cstdint>

namespace dronecore {


// TODO: we need to handle duplicate commands.

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

    std::lock_guard<std::mutex> lock(_mutex);

    for (auto it = _work_list.begin(); it != _work_list.end(); /* ++it */) {
        if ((*it)->timeout_cookie != nullptr) {
            _parent->unregister_timeout_handler((*it)->timeout_cookie);
            it = _work_list.erase(it);
        }
    }
}

MavlinkCommands::Result MavlinkCommands::send_command(uint16_t command,
                                                      const MavlinkCommands::Params params,
                                                      uint8_t target_system_id,
                                                      uint8_t target_component_id)
{
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
    // Block now to wait for result.
    res.wait();

    PromiseResult promise_result = res.get();

    // We should not get notified for progress because `std::future` does not support
    // being called multiple times.

    //if (promise_result.result == Result::IN_PROGRESS) {
    //    LogInfo() << "In progress: " << promise_result.progress;
    //    continue;
    //}
    return promise_result.result;
}


void MavlinkCommands::queue_command_async(uint16_t command,
                                          const MavlinkCommands::Params params,
                                          uint8_t target_system_id,
                                          uint8_t target_component_id,
                                          command_result_callback_t callback)
{
    // LogDebug() << "Command " << (int)command << " to send to " << (int)target_system_id << ", "
    //         << (int)target_component_id;

    std::lock_guard<std::mutex> lock(_mutex);

    auto new_work = std::make_shared<Work>();
    mavlink_msg_command_long_pack(_parent->get_own_system_id(),
                                  _parent->get_own_component_id(),
                                  &new_work->mavlink_message,
                                  target_system_id,
                                  target_component_id,
                                  command,
                                  0,
                                  params.v[0], params.v[1], params.v[2], params.v[3],
                                  params.v[4], params.v[5], params.v[6]);
    new_work->callback = callback;
    new_work->mavlink_command = command;
    _work_list.push_back(new_work);
}

void MavlinkCommands::receive_command_ack(mavlink_message_t message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);
    // LogDebug() << "We got an ack: " << command_ack.command;

    _mutex.lock();

    // If nothing is in the queue, we ignore the message all together.
    if (_work_list.size() == 0) {
        LogDebug() << "Ignoring command ack (" << command_ack.command << ")";
        _mutex.unlock();
        return;
    }

    // Go through all commands that require a response:
    for (auto it = _work_list.begin(); it != _work_list.end(); /* ++it */) {

        if ((*it)->mavlink_command == command_ack.command) {
            // If the command does not match with our current command, ignore it.
            LogDebug() << "Matched ack with command: " << (int)(*it)->mavlink_command;

            switch (command_ack.result) {
                case MAV_RESULT_ACCEPTED:
                    _parent->unregister_timeout_handler((*it)->timeout_cookie);
                    if ((*it)->callback) {
                        auto callback_tmp = (*it)->callback;
                        _mutex.unlock();
                        callback_tmp(Result::SUCCESS, 1.0f);
                        _mutex.lock();
                    }
                    it = _work_list.erase(it);
                    continue;

                case MAV_RESULT_DENIED:
                    LogWarn() << "command denied (" << (*it)->mavlink_command << ").";
                // FALLTHRU

                case MAV_RESULT_UNSUPPORTED:
                    LogWarn() << "command unsupported (" << (*it)->mavlink_command << ").";
                // FALLTHRU

                case MAV_RESULT_TEMPORARILY_REJECTED:
                    LogWarn() << "command temporarily rejected (" << (*it)->mavlink_command << ").";
                // FALLTHRU
                case MAV_RESULT_FAILED:
                    _parent->unregister_timeout_handler((*it)->timeout_cookie);
                    LogWarn() << "command failed (" << (*it)->mavlink_command << ").";
                    if ((*it)->callback) {
                        auto callback_tmp = (*it)->callback;
                        _mutex.unlock();
                        callback_tmp(Result::COMMAND_DENIED, NAN);
                        _mutex.lock();
                    }
                    it = _work_list.erase(it);
                    continue;

                case MAV_RESULT_IN_PROGRESS:
                    if ((int)command_ack.progress != 255) {
                        LogInfo() << "progress: " << (int)command_ack.progress
                                  << " % (" << (*it)->mavlink_command << ").";
                    }
                    // FIXME: We can only call callbacks with promises once, so let's not do it
                    //        on IN_PROGRESS for now.
                    //if (callback) {
                    //    callback(Result::IN_PROGRESS, command_ack.progress / 100.0f);
                    //}

                    // If we get a progress update, we can raise the timeout
                    // to something higher because we know the initial command
                    // has arrived. A possible timeout for this case is the initial
                    // timeout * the possible retries because this should match the
                    // case where there is no progress update and we keep trying.
                    _parent->refresh_timeout_handler((*it)->timeout_cookie);
                    _parent->update_timeout_handler(DEFAULT_TIMEOUT_IN_PROGRESS_S,
                                                    (*it)->timeout_cookie);
                    break;
            }
        }
        ++it;
    }

    _mutex.unlock();
}

void MavlinkCommands::do_work()
{
    _mutex.lock();

    // Check if there is work to do.
    if (_work_list.size() == 0) {
        // Nothing to do.
        _mutex.unlock();
        return;
    }

    // We support multiple commands at the same time but we only ever want to
    // have one command_id active at any time because otherwise, we have no idea which
    // ack belongs to what sent message.
    std::vector<uint16_t> active_command_ids {};

    for (auto it = _work_list.begin(); it != _work_list.end(); /* ++it */) {

        // We ignore what has already been sent and is not timed out yet.
        if ((*it)->num_sent != 0 && !(*it)->timed_out) {
            // Mark it as active.
            active_command_ids.push_back((*it)->mavlink_command);

            ++it;
            continue;
        }

        // We remove what has already been resent a few times.
        if ((*it)->timed_out && (*it)->num_sent >= RETRIES) {
            if ((*it)->callback) {
                auto callback_tmp = (*it)->callback;
                _mutex.unlock();
                callback_tmp(Result::TIMEOUT, NAN);
                _mutex.lock();
            }
            it = _work_list.erase(it);
            continue;
        }

        // Let's check if this command ID is already being sent.
        auto found_command_id =
            (find(active_command_ids.begin(), active_command_ids.end(), (*it)->mavlink_command)
             != active_command_ids.end());

        if (found_command_id) {
            LogDebug() << "We need to wait to send " << (int)(*it)->mavlink_command;
            ++it;
            continue;
        }

        // Now we can actually send it.
        LogDebug() << "Sending command " << (*it)->num_sent
                   << " time (" << (int)(*it)->mavlink_command << ")";

        if (!_parent->send_message((*it)->mavlink_message)) {
            LogErr() << "connection send error (" << (*it)->mavlink_command << ")";
            if ((*it)->callback) {
                auto callback_tmp = (*it)->callback;
                _mutex.unlock();
                callback_tmp(Result::CONNECTION_ERROR, NAN);
                _mutex.lock();
            }
            // We tried and failed badly, no incentive to retry at this point.
            // Let's delete the entry.
            it = _work_list.erase(it);
            continue;
        }

        ++((*it)->num_sent);

        std::shared_ptr<Work> work_ptr = *it;
        // Let's set a timer for retransmission if needed.
        _parent->register_timeout_handler(
        std::bind([work_ptr]() {
            // Send it again later, or remove it later.
            LogDebug() << "timeout, need to send it again, ore remove it later.";
            work_ptr->timed_out = true;
        }),
        (*it)->timeout_s, &((*it)->timeout_cookie));

        active_command_ids.push_back((*it)->mavlink_command);

        ++it;
    }
    _mutex.unlock();
}


} // namespace dronecore
