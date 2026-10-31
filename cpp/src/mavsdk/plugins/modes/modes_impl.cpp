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
    return modes();
}

std::vector<Modes::Mode> ModesImpl::modes() const
{
    std::vector<Modes::Mode> modes;
    modes.reserve(_entries.size());
    for (const auto& entry : _entries) {
        modes.push_back(entry.mode);
    }
    return modes;
}

Modes::CurrentModeHandle
ModesImpl::subscribe_current_mode(const Modes::CurrentModeCallback& callback)
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
    ModeEntry entry;
    Source source;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_source == Source::None) {
            call_result_callback(callback, Modes::Result::ModesNotKnownYet);
            return;
        }

        const auto it = std::find_if(_entries.begin(), _entries.end(), [&](const ModeEntry& e) {
            return e.mode.name == name;
        });
        if (it == _entries.end()) {
            call_result_callback(callback, Modes::Result::ModeNotFound);
            return;
        }
        entry = *it;
        source = _source;
    }

    // An autopilot without AVAILABLE_MODES won't know MAV_CMD_DO_SET_STANDARD_MODE either.
    if (entry.standard_mode == MAV_STANDARD_MODE_NON_STANDARD || source == Source::Fallback) {
        set_custom_mode(entry.mode.custom_mode, callback);
        return;
    }

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_DO_SET_STANDARD_MODE;
    command.params.maybe_param1 = static_cast<float>(entry.standard_mode);
    command.target_component_id = _system_impl->get_autopilot_id();

    _system_impl->send_command_async(
        command,
        [this, callback, custom_mode = entry.mode.custom_mode](
            MavlinkCommandSender::Result result, float) {
            if (result == MavlinkCommandSender::Result::InProgress) {
                return;
            }
            // The autopilot reports the mode as standard but doesn't implement the command.
            if (result == MavlinkCommandSender::Result::Unsupported) {
                set_custom_mode(custom_mode, callback);
                return;
            }
            call_result_callback(callback, result_from_command_result(result));
        });
}

Modes::Result ModesImpl::set_mode(const std::string& name)
{
    auto prom = std::promise<Modes::Result>();
    auto fut = prom.get_future();

    set_mode_async(name, [&prom](Modes::Result result) { prom.set_value(result); });

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
                LogInfo(
                    "AVAILABLE_MODES not supported ({}), using known modes",
                    static_cast<int>(result));
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

                ModeEntry entry{};
                entry.standard_mode = available_modes.standard_mode;
                auto& mode = entry.mode;
                // Standard modes come without a name, so we give them a fixed one. Unknown
                // standard modes (newer than us) are treated as non-standard.
                const auto standard_name = standard_mode_name(entry.standard_mode);
                mode.is_standard = !standard_name.empty();
                if (mode.is_standard) {
                    mode.name = standard_name;
                } else {
                    char name[sizeof(available_modes.mode_name) + 1]{};
                    std::memcpy(name, available_modes.mode_name, sizeof(available_modes.mode_name));
                    mode.name = name;
                }
                mode.custom_mode = available_modes.custom_mode;
                mode.is_user_selectable =
                    (available_modes.properties & MAV_MODE_PROPERTY_NOT_USER_SELECTABLE) == 0;
                mode.is_advanced = (available_modes.properties & MAV_MODE_PROPERTY_ADVANCED) != 0;
                mode.is_auto_mode = (available_modes.properties & MAV_MODE_PROPERTY_AUTO_MODE) != 0;
                _received_modes[available_modes.mode_index] = entry;
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

    _entries.clear();
    for (const auto& [index, entry] : _received_modes) {
        // Skip exact duplicates, they would only show up as "name (1)".
        const bool duplicate =
            std::any_of(_entries.begin(), _entries.end(), [&](const ModeEntry& existing) {
                return existing.mode.custom_mode == entry.mode.custom_mode &&
                       existing.mode.name == entry.mode.name;
            });
        if (!duplicate) {
            _entries.push_back(entry);
        }
    }
    _received_modes.clear();
    ensure_unique_names(_entries);

    publish_available_modes();
}

void ModesImpl::use_fallback()
{
    _source = Source::Fallback;
    _entries = modes_fallback(_system_impl->autopilot(), _system_impl->get_vehicle_type());
    publish_available_modes();
}

void ModesImpl::publish_available_modes()
{
    _available_modes_callbacks.queue(
        modes(), [this](const auto& func) { _system_impl->call_user_callback(func); });

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
    update_current_mode(current_mode.custom_mode, current_mode.standard_mode);
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
    uint32_t custom_mode, std::optional<uint8_t> maybe_standard_mode)
{
    _maybe_custom_mode = custom_mode;
    _maybe_standard_mode = maybe_standard_mode;

    Modes::Mode new_mode{};
    const auto it = std::find_if(_entries.begin(), _entries.end(), [&](const ModeEntry& entry) {
        return entry.mode.custom_mode == custom_mode;
    });
    if (it != _entries.end()) {
        new_mode = it->mode;
    } else {
        // Not in the list (yet), so all we know is what we were told.
        new_mode.custom_mode = custom_mode;
        new_mode.name = standard_mode_name(maybe_standard_mode.value_or(0));
        new_mode.is_standard = !new_mode.name.empty();
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

void ModesImpl::ensure_unique_names(std::vector<ModeEntry>& entries)
{
    // Names are what users select modes by, so they need to be unique. Same scheme as
    // QGroundControl: append " (1)", " (2)", ... to later duplicates.
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        unsigned duplicate_index = 0;
        for (auto it2 = std::next(it); it2 != entries.end(); ++it2) {
            if (it2->mode.name == it->mode.name) {
                it2->mode.name += " (" + std::to_string(++duplicate_index) + ")";
            }
        }
    }
}

} // namespace mavsdk
