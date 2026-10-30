#include "modes_impl.hpp"
#include "modes_fallback.hpp"
#include "callback_list.tpp"
#include "log.hpp"

#include <algorithm>
#include <cstring>
#include <future>

namespace mavsdk {

template class CallbackList<std::vector<Modes::Mode>>;
template class CallbackList<Modes::Mode>;

ModesImpl::ModesImpl(System& system) :
    PluginImplBase(system),
    _available_modes_callbacks(_system_impl->io_context()),
    _current_mode_callbacks(_system_impl->io_context())
{
    _system_impl->register_plugin(this);
}

ModesImpl::ModesImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system)),
    _available_modes_callbacks(_system_impl->io_context()),
    _current_mode_callbacks(_system_impl->io_context())
{
    _system_impl->register_plugin(this);
}

ModesImpl::~ModesImpl()
{
    _system_impl->unregister_plugin(this);
}

void ModesImpl::init()
{
    _system_impl->register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_AVAILABLE_MODES_MONITOR,
        MAV_COMP_ID_AUTOPILOT1,
        [this](const mavlink_message_t& message) { handle_available_modes_monitor(message); },
        &_message_cookie);
    _system_impl->register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_CURRENT_MODE,
        MAV_COMP_ID_AUTOPILOT1,
        [this](const mavlink_message_t& message) { handle_current_mode(message); },
        &_message_cookie);
    _system_impl->register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_HEARTBEAT,
        MAV_COMP_ID_AUTOPILOT1,
        [this](const mavlink_message_t& message) { handle_heartbeat(message); },
        &_message_cookie);
}

void ModesImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers_blocking(&_message_cookie);
}

void ModesImpl::enable()
{
    start_download();
}

void ModesImpl::disable()
{
    std::lock_guard<std::mutex> lock(_mutex);
    // Ignore the results of anything still in flight, and download again on reconnect, in case
    // the autopilot was changed or updated in the meantime.
    ++_generation;
    _downloading = false;
}

Modes::AvailableModesHandle
ModesImpl::subscribe_available_modes(const Modes::AvailableModesCallback& callback)
{
    return _available_modes_callbacks.subscribe(callback);
}

void ModesImpl::unsubscribe_available_modes(Modes::AvailableModesHandle handle)
{
    _available_modes_callbacks.unsubscribe(handle);
}

std::vector<Modes::Mode> ModesImpl::available_modes() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _modes;
}

Modes::CurrentModeHandle ModesImpl::subscribe_current_mode(const Modes::CurrentModeCallback& callback)
{
    return _current_mode_callbacks.subscribe(callback);
}

void ModesImpl::unsubscribe_current_mode(Modes::CurrentModeHandle handle)
{
    _current_mode_callbacks.unsubscribe(handle);
}

Modes::Mode ModesImpl::current_mode() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _current_mode;
}

void ModesImpl::set_mode_async(const std::string& name, const Modes::ResultCallback& callback)
{
    uint32_t custom_mode;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_source == Source::None) {
            call_result_callback(callback, Modes::Result::ModesNotKnownYet);
            return;
        }

        const auto it = std::find_if(_modes.begin(), _modes.end(), [&](const Modes::Mode& mode) {
            return mode.name == name;
        });
        if (it == _modes.end()) {
            call_result_callback(callback, Modes::Result::ModeNotFound);
            return;
        }
        custom_mode = it->custom_mode;
    }

    set_custom_mode(custom_mode, callback);
}

Modes::Result ModesImpl::set_mode(const std::string& name)
{
    auto prom = std::promise<Modes::Result>();
    auto fut = prom.get_future();

    set_mode_async(name, [&prom](Modes::Result result) { prom.set_value(result); });

    return fut.get();
}

void ModesImpl::set_standard_mode_async(
    Modes::StandardMode standard_mode, const Modes::ResultCallback& callback)
{
    if (standard_mode == Modes::StandardMode::NonStandard) {
        call_result_callback(callback, Modes::Result::Unsupported);
        return;
    }

    std::optional<uint32_t> maybe_custom_mode;
    Source source;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        source = _source;
        const auto it = std::find_if(_modes.begin(), _modes.end(), [&](const Modes::Mode& mode) {
            return mode.standard_mode == standard_mode;
        });
        if (it != _modes.end()) {
            maybe_custom_mode = it->custom_mode;
        }
    }

    // An autopilot without AVAILABLE_MODES won't know MAV_CMD_DO_SET_STANDARD_MODE either.
    if (source == Source::Fallback) {
        if (maybe_custom_mode) {
            set_custom_mode(maybe_custom_mode.value(), callback);
        } else {
            call_result_callback(callback, Modes::Result::Unsupported);
        }
        return;
    }

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_DO_SET_STANDARD_MODE;
    command.params.maybe_param1 = static_cast<float>(standard_mode);
    command.target_component_id = _system_impl->get_autopilot_id();

    _system_impl->send_command_async(
        command,
        [this, callback, maybe_custom_mode](MavlinkCommandSender::Result result, float) {
            if (result == MavlinkCommandSender::Result::InProgress) {
                return;
            }
            // Fall back to the custom mode if the autopilot reports the mode as standard but
            // doesn't implement the command.
            if (result == MavlinkCommandSender::Result::Unsupported && maybe_custom_mode) {
                set_custom_mode(maybe_custom_mode.value(), callback);
                return;
            }
            call_result_callback(callback, result_from_command_result(result));
        });
}

Modes::Result ModesImpl::set_standard_mode(Modes::StandardMode standard_mode)
{
    auto prom = std::promise<Modes::Result>();
    auto fut = prom.get_future();

    set_standard_mode_async(standard_mode, [&prom](Modes::Result result) {
        prom.set_value(result);
    });

    return fut.get();
}

void ModesImpl::set_custom_mode(uint32_t custom_mode, const Modes::ResultCallback& callback)
{
    _system_impl->set_custom_mode_async(
        custom_mode,
        [this, callback](MavlinkCommandSender::Result result, float) {
            if (result == MavlinkCommandSender::Result::InProgress) {
                return;
            }
            call_result_callback(callback, result_from_command_result(result));
        },
        _system_impl->get_autopilot_id());
}

void ModesImpl::start_download()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        ++_generation;
        _downloading = true;
        _number_modes = 0;
        _maybe_download_seq.reset();
        _received_modes.clear();
    }
    request_next_mode();
}

void ModesImpl::request_next_mode()
{
    unsigned generation;
    uint8_t index = 1;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_downloading) {
            return;
        }
        generation = _generation;

        // Request one mode at a time, like QGroundControl does, so a lost message only costs
        // a retry of that one mode rather than of the whole list.
        while (_received_modes.find(index) != _received_modes.end()) {
            ++index;
        }
        if (_number_modes > 0 && index > _number_modes) {
            finish_download();
            return;
        }
    }

    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_AVAILABLE_MODES,
        _system_impl->get_autopilot_id(),
        [this, generation](MavlinkCommandSender::Result result, const mavlink_message_t& message) {
            handle_available_modes_result(generation, result, message);
        },
        index);
}

void ModesImpl::handle_available_modes_result(
    unsigned generation, MavlinkCommandSender::Result result, const mavlink_message_t& message)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_downloading || generation != _generation) {
            return;
        }

        if (result == MavlinkCommandSender::Result::Busy) {
            // The same request is already in flight, its result will get us going again.
            return;
        }

        if (result != MavlinkCommandSender::Result::Success) {
            _downloading = false;
            if (_source == Source::None || _source == Source::Fallback) {
                LogInfo("AVAILABLE_MODES not supported ({}), using known modes", static_cast<int>(result));
                use_fallback();
            } else {
                LogWarn(
                    "Updating available modes failed ({}), keeping previous list",
                    static_cast<int>(result));
            }
            return;
        }

        mavlink_available_modes_t available_modes;
        mavlink_msg_available_modes_decode(&message, &available_modes);

        if (available_modes.number_modes == 0 || available_modes.mode_index == 0 ||
            available_modes.mode_index > available_modes.number_modes) {
            LogWarn(
                "Ignoring invalid AVAILABLE_MODES (index {} of {})",
                available_modes.mode_index,
                available_modes.number_modes);
        } else {
            // The list changed while we downloaded it, start over.
            const bool seq_changed =
                _maybe_download_seq && _maybe_download_seq.value() != available_modes.seq;
            const bool number_changed =
                _number_modes != 0 && _number_modes != available_modes.number_modes;
            if (seq_changed || number_changed) {
                constexpr unsigned max_restarts = 3;
                if (++_restarts > max_restarts) {
                    LogWarn("Available modes kept changing during download, giving up");
                    _downloading = false;
                    _restarts = 0;
                    return;
                }
                _number_modes = 0;
                _maybe_download_seq.reset();
                _received_modes.clear();
            } else {
                _number_modes = available_modes.number_modes;
                _maybe_download_seq = available_modes.seq;

                Modes::Mode mode{};
                mode.standard_mode = standard_mode_from_mavlink(available_modes.standard_mode);
                if (mode.standard_mode == Modes::StandardMode::NonStandard) {
                    char name[sizeof(available_modes.mode_name) + 1]{};
                    std::memcpy(name, available_modes.mode_name, sizeof(available_modes.mode_name));
                    mode.name = name;
                } else {
                    // Standard modes come without a name, so we give them a fixed one.
                    mode.name = modes_standard_mode_name(mode.standard_mode);
                }
                mode.custom_mode = available_modes.custom_mode;
                mode.is_user_selectable =
                    (available_modes.properties & MAV_MODE_PROPERTY_NOT_USER_SELECTABLE) == 0;
                mode.is_advanced = (available_modes.properties & MAV_MODE_PROPERTY_ADVANCED) != 0;
                mode.is_auto_mode =
                    (available_modes.properties & MAV_MODE_PROPERTY_AUTO_MODE) != 0;
                _received_modes[available_modes.mode_index] = mode;
            }
        }
    }

    request_next_mode();
}

void ModesImpl::finish_download()
{
    _downloading = false;
    _restarts = 0;
    _maybe_last_seq = _maybe_download_seq;
    _source = Source::AvailableModes;

    _modes.clear();
    for (const auto& [index, mode] : _received_modes) {
        // Skip exact duplicates, they would only show up as "name (1)".
        const bool duplicate =
            std::any_of(_modes.begin(), _modes.end(), [&](const Modes::Mode& existing) {
                return existing.custom_mode == mode.custom_mode && existing.name == mode.name;
            });
        if (!duplicate) {
            _modes.push_back(mode);
        }
    }
    _received_modes.clear();
    ensure_unique_names(_modes);

    publish_available_modes();
}

void ModesImpl::use_fallback()
{
    _source = Source::Fallback;
    _modes = modes_fallback(_system_impl->autopilot(), _system_impl->get_vehicle_type());
    publish_available_modes();
}

void ModesImpl::publish_available_modes()
{
    _available_modes_callbacks.queue(
        _modes, [this](const auto& func) { _system_impl->call_user_callback(func); });

    // The current mode might have been unknown until now.
    if (_maybe_custom_mode) {
        update_current_mode(_maybe_custom_mode.value(), _maybe_standard_mode);
    }
}

void ModesImpl::handle_available_modes_monitor(const mavlink_message_t& message)
{
    mavlink_available_modes_monitor_t monitor;
    mavlink_msg_available_modes_monitor_decode(&message, &monitor);

    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_maybe_last_seq && _maybe_last_seq.value() == monitor.seq) {
            return;
        }
        if (_downloading && _maybe_download_seq && _maybe_download_seq.value() == monitor.seq) {
            return;
        }
        _maybe_last_seq = monitor.seq;
    }

    start_download();
}

void ModesImpl::handle_current_mode(const mavlink_message_t& message)
{
    mavlink_current_mode_t current_mode;
    mavlink_msg_current_mode_decode(&message, &current_mode);

    std::lock_guard<std::mutex> lock(_mutex);
    _current_mode_received = true;
    update_current_mode(
        current_mode.custom_mode, standard_mode_from_mavlink(current_mode.standard_mode));
}

void ModesImpl::handle_heartbeat(const mavlink_message_t& message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    if ((heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) == 0) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    // CURRENT_MODE is more complete, the heartbeat is for autopilots without it.
    if (_current_mode_received) {
        return;
    }
    update_current_mode(heartbeat.custom_mode, std::nullopt);
}

void ModesImpl::update_current_mode(
    uint32_t custom_mode, std::optional<Modes::StandardMode> maybe_standard_mode)
{
    _maybe_custom_mode = custom_mode;
    _maybe_standard_mode = maybe_standard_mode;

    Modes::Mode new_mode{};
    const auto it = std::find_if(_modes.begin(), _modes.end(), [&](const Modes::Mode& mode) {
        return mode.custom_mode == custom_mode;
    });
    if (it != _modes.end()) {
        new_mode = *it;
    } else {
        // Not in the list (yet), so all we know is what we were told.
        new_mode.custom_mode = custom_mode;
        new_mode.standard_mode = maybe_standard_mode.value_or(Modes::StandardMode::NonStandard);
        new_mode.name = modes_standard_mode_name(new_mode.standard_mode);
    }

    if (new_mode == _current_mode) {
        return;
    }
    _current_mode = new_mode;

    _current_mode_callbacks.queue(
        _current_mode, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void ModesImpl::call_result_callback(const Modes::ResultCallback& callback, Modes::Result result)
{
    if (callback) {
        _system_impl->call_user_callback([callback, result]() { callback(result); });
    }
}

Modes::Result ModesImpl::result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return Modes::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Modes::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return Modes::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return Modes::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            return Modes::Result::Denied;
        case MavlinkCommandSender::Result::Unsupported:
            return Modes::Result::Unsupported;
        case MavlinkCommandSender::Result::Timeout:
            return Modes::Result::Timeout;
        case MavlinkCommandSender::Result::Failed:
            return Modes::Result::Failed;
        default:
            return Modes::Result::Unknown;
    }
}

Modes::StandardMode ModesImpl::standard_mode_from_mavlink(uint8_t standard_mode)
{
    switch (standard_mode) {
        case MAV_STANDARD_MODE_POSITION_HOLD:
            return Modes::StandardMode::PositionHold;
        case MAV_STANDARD_MODE_ORBIT:
            return Modes::StandardMode::Orbit;
        case MAV_STANDARD_MODE_CRUISE:
            return Modes::StandardMode::Cruise;
        case MAV_STANDARD_MODE_ALTITUDE_HOLD:
            return Modes::StandardMode::AltitudeHold;
        case MAV_STANDARD_MODE_SAFE_RECOVERY:
            return Modes::StandardMode::SafeRecovery;
        case MAV_STANDARD_MODE_MISSION:
            return Modes::StandardMode::Mission;
        case MAV_STANDARD_MODE_LAND:
            return Modes::StandardMode::Land;
        case MAV_STANDARD_MODE_TAKEOFF:
            return Modes::StandardMode::Takeoff;
        default:
            return Modes::StandardMode::NonStandard;
    }
}

void ModesImpl::ensure_unique_names(std::vector<Modes::Mode>& modes)
{
    // Names are what users select modes by, so they need to be unique. Same scheme as
    // QGroundControl: append " (1)", " (2)", ... to later duplicates.
    for (auto it = modes.begin(); it != modes.end(); ++it) {
        unsigned duplicate_index = 0;
        for (auto it2 = std::next(it); it2 != modes.end(); ++it2) {
            if (it2->name == it->name) {
                it2->name += " (" + std::to_string(++duplicate_index) + ")";
            }
        }
    }
}

} // namespace mavsdk
