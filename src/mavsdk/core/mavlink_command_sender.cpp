#include "mavlink_command_sender.h"
#include "mavlink_address.h"
#include "system_impl.h"
#include "unused.h"
#include <cmath>
#include <future>
#include <memory>

namespace mavsdk {

MavlinkCommandSender::MavlinkCommandSender(SystemImpl& system_impl) : _system_impl(system_impl)
{
    if (const char* env_p = std::getenv("MAVSDK_COMMAND_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Command debugging is on.";
            _command_debugging = true;
        }
    }

    _system_impl.register_mavlink_message_handler(
        MAVLINK_MSG_ID_COMMAND_ACK,
        [this](const mavlink_message_t& message) { receive_command_ack(message); },
        this);
}

MavlinkCommandSender::~MavlinkCommandSender()
{
    if (_command_debugging) {
        LogDebug() << "CommandSender destroyed";
    }
    _system_impl.unregister_all_mavlink_message_handlers(this);

    LockedQueue<Work>::Guard work_queue_guard(_work_queue);
    for (const auto& work : _work_queue) {
        _system_impl.unregister_timeout_handler(work->timeout_cookie);
    }
}

MavlinkCommandSender::Result MavlinkCommandSender::send_command(
    const MavlinkCommandSender::CommandInt& command, unsigned retries)
{
    // We wrap the async call with a promise and future.
    auto prom = std::make_shared<std::promise<Result>>();
    auto res = prom->get_future();

    queue_command_async(
        command,
        [prom](Result result, float progress) {
            UNUSED(progress);
            // We can only fulfill the promise once in C++11.
            // Therefore, we have to ignore the IN_PROGRESS state and wait
            // for the final result.
            if (result != Result::InProgress) {
                prom->set_value(result);
            }
        },
        retries);

    // Block now to wait for result.
    return res.get();
}

MavlinkCommandSender::Result MavlinkCommandSender::send_command(
    const MavlinkCommandSender::CommandLong& command, unsigned retries)
{
    // We wrap the async call with a promise and future.
    auto prom = std::make_shared<std::promise<Result>>();
    auto res = prom->get_future();

    queue_command_async(
        command,
        [prom](Result result, float progress) {
            UNUSED(progress);
            // We can only fulfill the promise once in C++11.
            // Therefore, we have to ignore the IN_PROGRESS state and wait
            // for the final result.
            if (result != Result::InProgress) {
                prom->set_value(result);
            }
        },
        retries);

    return res.get();
}

void MavlinkCommandSender::queue_command_async(
    const CommandInt& command, const CommandResultCallback& callback, unsigned retries)
{
    if (_command_debugging) {
        LogDebug() << "COMMAND_INT " << static_cast<int>(command.command) << " to send to "
                   << static_cast<int>(command.target_system_id) << ", "
                   << static_cast<int>(command.target_component_id);
    }

    CommandIdentification identification = identification_from_command(command);

    {
        LockedQueue<Work>::Guard work_queue_guard(_work_queue);
        for (const auto& work : _work_queue) {
            if (work->identification == identification && callback == nullptr) {
                if (_command_debugging) {
                    LogDebug() << "Dropping command " << static_cast<int>(identification.command)
                               << " that is already being sent";
                }
                return;
            }
        }
    }

    auto new_work = std::make_shared<Work>();
    new_work->timeout_s = _system_impl.timeout_s();
    new_work->command = command;
    new_work->identification = identification;
    new_work->callback = callback;
    new_work->retries_to_do = retries;
    _work_queue.push_back(new_work);
}

void MavlinkCommandSender::queue_command_async(
    const CommandLong& command, const CommandResultCallback& callback, unsigned retries)
{
    if (_command_debugging) {
        LogDebug() << "COMMAND_LONG " << static_cast<int>(command.command) << " to send to "
                   << static_cast<int>(command.target_system_id) << ", "
                   << static_cast<int>(command.target_component_id);
    }

    CommandIdentification identification = identification_from_command(command);

    {
        LockedQueue<Work>::Guard work_queue_guard(_work_queue);
        for (const auto& work : _work_queue) {
            if (work->identification == identification && callback == nullptr) {
                if (_command_debugging) {
                    LogDebug() << "Dropping command " << static_cast<int>(identification.command)
                               << " that is already being sent";
                }
                return;
            }
        }
    }

    auto new_work = std::make_shared<Work>();
    new_work->timeout_s = _system_impl.timeout_s();
    new_work->command = command;
    new_work->identification = identification;
    new_work->callback = callback;
    new_work->time_started = _system_impl.get_time().steady_time();
    new_work->retries_to_do = retries;
    _work_queue.push_back(new_work);
}

void MavlinkCommandSender::receive_command_ack(const mavlink_message_t& message)
{
    mavlink_command_ack_t command_ack;
    mavlink_msg_command_ack_decode(&message, &command_ack);

    if ((command_ack.target_system &&
         command_ack.target_system != _system_impl.get_own_system_id()) ||
        (command_ack.target_component &&
         command_ack.target_component != _system_impl.get_own_component_id())) {
        if (_command_debugging) {
            LogDebug() << "Ignoring command ack for command "
                       << static_cast<int>(command_ack.command) << " from "
                       << static_cast<int>(message.sysid) << '/' << static_cast<int>(message.compid)
                       << " to " << static_cast<int>(command_ack.target_system) << '/'
                       << static_cast<int>(command_ack.target_component);
        }
        return;
    }

    LockedQueue<Work>::Guard work_queue_guard(_work_queue);

    for (auto it = _work_queue.begin(); it != _work_queue.end(); ++it) {
        const auto& work = *it;

        if (!work) {
            LogErr() << "No work available! (should not happen #1)";
            return;
        }

        if (work->identification.command != command_ack.command ||
            (work->identification.target_system_id != 0 &&
             work->identification.target_system_id != message.sysid) ||
            (work->identification.target_component_id != 0 &&
             work->identification.target_component_id != message.compid)) {
            if (_command_debugging) {
                LogDebug() << "Command ack for " << command_ack.command
                           << " (from: " << std::to_string(message.sysid) << "/"
                           << std::to_string(message.compid) << ")" << " does not match command "
                           << work->identification.command
                           << " (to: " << std::to_string(work->identification.target_system_id)
                           << "/" << std::to_string(work->identification.target_component_id) << ")"
                           << " after "
                           << _system_impl.get_time().elapsed_since_s(work->time_started) << " s";
            }
            continue;
        }

        if (_command_debugging) {
            LogDebug() << "Received command ack for " << command_ack.command << " with result "
                       << static_cast<int>(command_ack.result) << " after "
                       << _system_impl.get_time().elapsed_since_s(work->time_started) << " s";
        }

        CommandResultCallback temp_callback = work->callback;
        std::pair<Result, float> temp_result{Result::UnknownError, NAN};

        switch (command_ack.result) {
            case MAV_RESULT_ACCEPTED:
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Success, 1.0f};
                _work_queue.erase(it);
                break;

            case MAV_RESULT_DENIED:
                if (_command_debugging) {
                    LogDebug() << "command denied (" << work->identification.command << ").";
                    if (work->identification.command == 512) {
                        LogDebug() << "(message " << work->identification.maybe_param1 << ")";
                    }
                }
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Denied, NAN};
                _work_queue.erase(it);
                break;

            case MAV_RESULT_UNSUPPORTED:
                if (_command_debugging) {
                    LogDebug() << "command unsupported (" << work->identification.command << ").";
                }
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Unsupported, NAN};
                _work_queue.erase(it);
                break;

            case MAV_RESULT_TEMPORARILY_REJECTED:
                if (_command_debugging) {
                    LogDebug() << "command temporarily rejected (" << work->identification.command
                               << ").";
                }
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::TemporarilyRejected, NAN};
                _work_queue.erase(it);
                break;

            case MAV_RESULT_FAILED:
                if (_command_debugging) {
                    LogDebug() << "command failed (" << work->identification.command << ").";
                }
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Failed, NAN};
                _work_queue.erase(it);
                break;

            case MAV_RESULT_IN_PROGRESS:
                if (_command_debugging) {
                    if (static_cast<int>(command_ack.progress) != 255) {
                        LogDebug() << "progress: " << static_cast<int>(command_ack.progress)
                                   << " % (" << work->identification.command << ").";
                    }
                }
                // If we get a progress update, we can raise the timeout
                // to something higher because we know the initial command
                // has arrived.
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                work->timeout_cookie = _system_impl.register_timeout_handler(
                    [this, identification = work->identification] {
                        receive_timeout(identification);
                    },
                    3.0);

                temp_result = {
                    Result::InProgress, static_cast<float>(command_ack.progress) / 100.0f};
                break;

            case MAV_RESULT_CANCELLED:
                if (_command_debugging) {
                    LogDebug() << "command cancelled (" << work->identification.command << ").";
                }
                _system_impl.unregister_timeout_handler(work->timeout_cookie);
                temp_result = {Result::Cancelled, NAN};
                _work_queue.erase(it);
                break;

            default:
                LogWarn() << "Received unknown ack.";
                break;
        }

        if (temp_callback != nullptr) {
            call_callback(temp_callback, temp_result.first, temp_result.second);
        }

        return;
    }

    if (_command_debugging) {
        LogDebug() << "Received ack from " << static_cast<int>(message.sysid) << '/'
                   << static_cast<int>(message.compid)
                   << " for not-existing command: " << static_cast<int>(command_ack.command)
                   << "! Ignoring...";
    } else {
        LogWarn() << "Received ack for not-existing command: "
                  << static_cast<int>(command_ack.command) << "! Ignoring...";
    }
}

void MavlinkCommandSender::receive_timeout(const CommandIdentification& identification)
{
    if (_command_debugging) {
        LogDebug() << "Got timeout!";
    }
    bool found_command = false;
    CommandResultCallback temp_callback = nullptr;
    std::pair<Result, float> temp_result{Result::UnknownError, NAN};

    LockedQueue<Work>::Guard work_queue_guard(_work_queue);

    for (auto it = _work_queue.begin(); it != _work_queue.end(); ++it) {
        const auto& work = *it;

        if (!work) {
            LogErr() << "No work available! (should not happen #2)";
            return;
        }

        if (work->identification != identification) {
            continue;
        }

        found_command = true;

        if (work->retries_to_do > 0) {
            // We're not sure the command arrived, let's retransmit.
            if (_command_debugging) {
                LogWarn() << "sending again after "
                          << _system_impl.get_time().elapsed_since_s(work->time_started)
                          << " s, retries to do: " << work->retries_to_do << "  ("
                          << work->identification.command << ").";

                if (work->identification.command == MAV_CMD_REQUEST_MESSAGE) {
                    LogWarn() << "Request was for msg ID: " << work->identification.maybe_param1;
                }
            }

            if (!send_mavlink_message(work->command)) {
                LogErr() << "connection send error in retransmit (" << work->identification.command
                         << ").";
                temp_callback = work->callback;
                temp_result = {Result::ConnectionError, NAN};
                _work_queue.erase(it);
                break;
            } else {
                --work->retries_to_do;
                work->timeout_cookie = _system_impl.register_timeout_handler(
                    [this, identification = work->identification] {
                        receive_timeout(identification);
                    },
                    work->timeout_s);
            }
        } else {
            // We have tried retransmitting, giving up now.
            if (work->identification.command == 512) {
                uint8_t target_sysid;
                uint8_t target_compid;
                if (auto command_int = std::get_if<CommandInt>(&work->command)) {
                    target_sysid = command_int->target_system_id;
                    target_compid = command_int->target_component_id;
                } else if (auto command_long = std::get_if<CommandLong>(&work->command)) {
                    target_sysid = command_long->target_system_id;
                    target_compid = command_long->target_component_id;
                } else {
                    LogErr() << "No command, that's awkward";
                    continue;
                }
                if (_command_debugging) {
                    LogWarn() << "Retrying failed for REQUEST_MESSAGE command for message: "
                              << work->identification.maybe_param1 << ", to ("
                              << std::to_string(target_sysid) << "/"
                              << std::to_string(target_compid) << ")";
                }
            } else {
                if (_command_debugging) {
                    LogWarn() << "Retrying failed for command: " << work->identification.command
                              << ")";
                }
            }

            temp_callback = work->callback;
            temp_result = {Result::Timeout, NAN};
            _work_queue.erase(it);
            break;
        }
    }

    if (temp_callback != nullptr) {
        call_callback(temp_callback, temp_result.first, temp_result.second);
    }

    if (!found_command) {
        LogWarn() << "Timeout for not-existing command: "
                  << static_cast<int>(identification.command) << "! Ignoring...";
    }
}

void MavlinkCommandSender::do_work()
{
    LockedQueue<Work>::Guard work_queue_guard(_work_queue);

    for (const auto& work : _work_queue) {
        if (work->already_sent) {
            continue;
        }

        bool already_being_sent = false;
        for (const auto& other_work : _work_queue) {
            // Ignore itself:
            if (other_work == work) {
                continue;
            }

            // Check if command with same command ID is already being sent.
            if (other_work->already_sent &&
                other_work->identification.command == work->identification.command) {
                if (_command_debugging) {
                    LogDebug() << "Command " << static_cast<int>(work->identification.command)
                               << " is already being sent, waiting...";
                }
                already_being_sent = true;
                break;
            }
        }

        if (already_being_sent) {
            continue;
        }

        // LogDebug() << "sending it the first time (" << work->mavlink_command << ")";
        work->time_started = _system_impl.get_time().steady_time();

        {
            if (!send_mavlink_message(work->command)) {
                LogErr() << "connection send error (" << work->identification.command << ")";
                // In this case we try again after the timeout. Chances are slim it will work next
                // time though.
            } else {
                if (_command_debugging) {
                    LogDebug() << "Sent command " << static_cast<int>(work->identification.command);
                }
            }
        }

        work->already_sent = true;

        work->timeout_cookie = _system_impl.register_timeout_handler(
            [this, identification = work->identification] { receive_timeout(identification); },
            work->timeout_s);
    }
}

void MavlinkCommandSender::call_callback(
    const CommandResultCallback& callback, Result result, float progress) const
{
    if (!callback) {
        return;
    }

    // It seems that we need to queue the callback on the thread pool otherwise
    // we lock ourselves out when we send a command in the callback receiving a command result.
    auto temp_callback = callback;
    _system_impl.call_user_callback(
        [temp_callback, result, progress]() { temp_callback(result, progress); });
}

bool MavlinkCommandSender::send_mavlink_message(const Command& command) const
{
    if (auto command_int = std::get_if<CommandInt>(&command)) {
        return _system_impl.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_command_int_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                command_int->target_system_id,
                command_int->target_component_id,
                command_int->frame,
                command_int->command,
                command_int->current,
                command_int->autocontinue,
                maybe_reserved(command_int->params.maybe_param1),
                maybe_reserved(command_int->params.maybe_param2),
                maybe_reserved(command_int->params.maybe_param3),
                maybe_reserved(command_int->params.maybe_param4),
                command_int->params.x,
                command_int->params.y,
                maybe_reserved(command_int->params.maybe_z));
            return message;
        });

    } else if (auto command_long = std::get_if<CommandLong>(&command)) {
        return _system_impl.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_command_long_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                command_long->target_system_id,
                command_long->target_component_id,
                command_long->command,
                command_long->confirmation,
                maybe_reserved(command_long->params.maybe_param1),
                maybe_reserved(command_long->params.maybe_param2),
                maybe_reserved(command_long->params.maybe_param3),
                maybe_reserved(command_long->params.maybe_param4),
                maybe_reserved(command_long->params.maybe_param5),
                maybe_reserved(command_long->params.maybe_param6),
                maybe_reserved(command_long->params.maybe_param7));
            return message;
        });
    } else {
        return false;
    }
}

float MavlinkCommandSender::maybe_reserved(const std::optional<float>& maybe_param) const
{
    if (maybe_param) {
        return maybe_param.value();

    } else {
        if (_system_impl.autopilot() == Autopilot::ArduPilot) {
            return 0.0f;
        } else {
            return NAN;
        }
    }
}

} // namespace mavsdk
