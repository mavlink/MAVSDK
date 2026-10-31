#include "modes_server_impl.hpp"
#include "callback_list.tpp"
#include "px4_custom_mode.hpp"
#include "standard_modes.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace mavsdk {

template class CallbackList<ModesServer::Mode>;

namespace {

// The check runs at 10 Hz so that mode changes are reported quickly, the periodic messages at
// 1 Hz.
constexpr float check_interval_s = 0.1f;
constexpr unsigned ticks_per_periodic_send = 10;

uint32_t mode_properties(const ModesServer::Mode& mode)
{
    uint32_t properties = 0;
    if (mode.is_advanced) {
        properties |= MAV_MODE_PROPERTY_ADVANCED;
    }
    if (!mode.is_user_selectable) {
        properties |= MAV_MODE_PROPERTY_NOT_USER_SELECTABLE;
    }
    if (mode.is_auto_mode) {
        properties |= MAV_MODE_PROPERTY_AUTO_MODE;
    }
    return properties;
}

} // namespace

ModesServerImpl::ModesServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
}

ModesServerImpl::~ModesServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void ModesServerImpl::init()
{
    _server_component_impl->mavlink_request_message_handler().register_handler(
        MAVLINK_MSG_ID_AVAILABLE_MODES,
        [this](uint32_t, uint8_t, const MavlinkRequestMessageHandler::Params& params) {
            return process_available_modes_request(params);
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_DO_SET_STANDARD_MODE,
        [this](const MavlinkCommandReceiver::CommandLong& command)
            -> std::optional<mavlink_command_ack_t> {
            const auto result = process_set_standard_mode(command.params.param1);
            return _server_component_impl->make_command_ack_message(
                command, result.value_or(MAV_RESULT_UNSUPPORTED));
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_DO_SET_STANDARD_MODE,
        [this](const MavlinkCommandReceiver::CommandInt& command)
            -> std::optional<mavlink_command_ack_t> {
            const auto result = process_set_standard_mode(command.params.param1);
            return _server_component_impl->make_command_ack_message(
                command, result.value_or(MAV_RESULT_UNSUPPORTED));
        },
        this);

    // ActionServer handles these as well, but only until we have modes. Without modes we
    // return no ack and leave them to it.
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_DO_SET_MODE,
        [this](const MavlinkCommandReceiver::CommandLong& command)
            -> std::optional<mavlink_command_ack_t> {
            const auto result = process_set_mode(
                static_cast<uint8_t>(command.params.param1), custom_mode_from_command(command));
            if (!result) {
                return std::nullopt;
            }
            return _server_component_impl->make_command_ack_message(command, result.value());
        },
        this);
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SET_MODE,
        [this](const mavlink_message_t& message) { process_set_mode_message(message); },
        this);

    _call_every_cookie = _server_component_impl->add_call_every(
        [this]() { check_current_mode(); }, check_interval_s);
}

void ModesServerImpl::deinit()
{
    _server_component_impl->set_modes_server_handles_mode_changes(false);
    _server_component_impl->remove_call_every_blocking(_call_every_cookie);
    _server_component_impl->unregister_all_mavlink_message_handlers_blocking(this);
    _server_component_impl->unregister_all_mavlink_command_handlers(this);
    _server_component_impl->mavlink_request_message_handler().unregister_all_handlers(this);
}

ModesServer::Result
ModesServerImpl::set_available_modes(const std::vector<ModesServer::Mode>& modes)
{
    if (modes.empty() || modes.size() > std::numeric_limits<uint8_t>::max()) {
        return ModesServer::Result::InvalidModes;
    }

    std::vector<Entry> entries;
    entries.reserve(modes.size());
    for (const auto& mode : modes) {
        if (mode.name.empty() ||
            mode.name.size() > MAVLINK_MSG_AVAILABLE_MODES_FIELD_MODE_NAME_LEN) {
            return ModesServer::Result::InvalidModes;
        }

        const uint8_t standard_mode = mode.is_standard ?
                                          standard_mode_from_name(mode.name) :
                                          static_cast<uint8_t>(MAV_STANDARD_MODE_NON_STANDARD);
        if (mode.is_standard && standard_mode == MAV_STANDARD_MODE_NON_STANDARD) {
            return ModesServer::Result::InvalidModes;
        }

        const bool duplicate = std::any_of(entries.begin(), entries.end(), [&](const Entry& entry) {
            return entry.mode.name == mode.name || entry.mode.custom_mode == mode.custom_mode;
        });
        if (duplicate) {
            return ModesServer::Result::InvalidModes;
        }

        entries.push_back({mode, standard_mode});
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _entries = std::move(entries);
        _modes_set = true;
        ++_seq;
        // A pending request might be for a mode that is gone.
        _intended_custom_mode.reset();
        // Report the current mode again, its standard mode might have changed.
        _last_sent.reset();
    }

    _server_component_impl->set_modes_server_handles_mode_changes(true);

    send_available_modes_monitor();

    return ModesServer::Result::Success;
}

ModesServer::ModeRequestedHandle
ModesServerImpl::subscribe_mode_requested(const ModesServer::ModeRequestedCallback& callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _mode_requested_callbacks.subscribe(callback);
}

void ModesServerImpl::unsubscribe_mode_requested(ModesServer::ModeRequestedHandle handle)
{
    // No lock: unsubscribe() waits, and the callback may want it. See THREADING.md.
    _mode_requested_callbacks.unsubscribe(handle);
}

ModesServer::Result ModesServerImpl::set_current_mode(const std::string& name)
{
    uint32_t custom_mode;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
            return entry.mode.name == name;
        });
        if (it == _entries.end()) {
            return ModesServer::Result::ModeNotFound;
        }
        custom_mode = it->mode.custom_mode;
        _intended_custom_mode.reset();
        _last_sent = {custom_mode, custom_mode};
    }

    _server_component_impl->set_base_mode(
        _server_component_impl->get_base_mode() | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED);
    _server_component_impl->set_custom_mode(custom_mode);

    // Don't wait for the next check.
    send_current_mode(custom_mode, custom_mode);

    return ModesServer::Result::Success;
}

std::optional<MAV_RESULT>
ModesServerImpl::process_available_modes_request(const MavlinkRequestMessageHandler::Params& params)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_modes_set) {
        return MAV_RESULT_TEMPORARILY_REJECTED;
    }

    // param2 is the mode index: 0 for all modes, otherwise 1-based.
    const auto index = static_cast<int>(params[0]);
    if (index == 0) {
        for (size_t i = 0; i < _entries.size(); ++i) {
            send_available_mode(i);
        }
        return MAV_RESULT_ACCEPTED;
    }

    if (index < 0 || static_cast<size_t>(index) > _entries.size()) {
        return MAV_RESULT_DENIED;
    }

    send_available_mode(static_cast<size_t>(index) - 1);
    return MAV_RESULT_ACCEPTED;
}

std::optional<MAV_RESULT> ModesServerImpl::process_set_standard_mode(float param1)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_modes_set) {
        // Unsupported makes clients fall back to MAV_CMD_DO_SET_MODE.
        return std::nullopt;
    }

    const auto standard_mode = static_cast<uint8_t>(param1);
    const auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.standard_mode != MAV_STANDARD_MODE_NON_STANDARD &&
               entry.standard_mode == standard_mode;
    });
    if (it == _entries.end()) {
        return MAV_RESULT_DENIED;
    }
    _intended_custom_mode = it->mode.custom_mode;
    const ModesServer::Mode mode = it->mode;
    lock.unlock();

    report_mode_requested(mode);
    return MAV_RESULT_ACCEPTED;
}

std::optional<MAV_RESULT> ModesServerImpl::process_set_mode(uint8_t base_mode, uint32_t custom_mode)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_modes_set) {
        return std::nullopt;
    }

    if ((base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) == 0) {
        return MAV_RESULT_UNSUPPORTED;
    }

    const auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.mode.custom_mode == custom_mode;
    });
    if (it == _entries.end()) {
        return MAV_RESULT_DENIED;
    }
    _intended_custom_mode = it->mode.custom_mode;
    const ModesServer::Mode mode = it->mode;
    lock.unlock();

    report_mode_requested(mode);
    return MAV_RESULT_ACCEPTED;
}

void ModesServerImpl::process_set_mode_message(const mavlink_message_t& message)
{
    mavlink_set_mode_t set_mode;
    mavlink_msg_set_mode_decode(&message, &set_mode);

    const auto result = process_set_mode(set_mode.base_mode, set_mode.custom_mode);
    if (!result) {
        return;
    }

    // SET_MODE is a message, not a command, but is acked like one.
    mavlink_command_ack_t command_ack{};
    command_ack.command = MAVLINK_MSG_ID_SET_MODE;
    command_ack.result = result.value();
    command_ack.progress = std::numeric_limits<uint8_t>::max();
    command_ack.target_system = mavlink_msg_target_field(message.sysid);
    command_ack.target_component = message.compid;
    _server_component_impl->send_command_ack(command_ack, message.sysid);
}

void ModesServerImpl::report_mode_requested(const ModesServer::Mode& mode)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _mode_requested_callbacks.queue(
        mode, [this](const auto& func) { _server_component_impl->call_user_callback(func); });
}

uint32_t
ModesServerImpl::custom_mode_from_command(const MavlinkCommandReceiver::CommandLong& command) const
{
    if (_server_component_impl->get_mav_autopilot() == MAV_AUTOPILOT_PX4) {
        // PX4 splits its custom mode into main and sub mode.
        px4::px4_custom_mode px4_mode{};
        px4_mode.main_mode = static_cast<uint8_t>(command.params.param2);
        px4_mode.sub_mode = static_cast<uint8_t>(command.params.param3);
        return px4_mode.data;
    }
    return static_cast<uint32_t>(command.params.param2);
}

void ModesServerImpl::check_current_mode()
{
    const uint32_t custom_mode = _server_component_impl->get_custom_mode();

    bool send_current = false;
    bool send_monitor = false;
    uint32_t intended_custom_mode;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_modes_set) {
            return;
        }

        intended_custom_mode = _intended_custom_mode.value_or(custom_mode);

        if (++_ticks >= ticks_per_periodic_send) {
            _ticks = 0;
            send_current = true;
            send_monitor = true;
        }

        const auto current = std::make_pair(custom_mode, intended_custom_mode);
        if (_last_sent != current) {
            _last_sent = current;
            send_current = true;
        }
    }

    if (send_current) {
        send_current_mode(custom_mode, intended_custom_mode);
    }
    if (send_monitor) {
        send_available_modes_monitor();
    }
}

void ModesServerImpl::send_available_mode(size_t index)
{
    // Called with _mutex held.
    const auto& entry = _entries[index];

    // The packer copies the full field width, so hand it a zero-padded buffer.
    char name[sizeof(mavlink_available_modes_t::mode_name)]{};
    std::memcpy(name, entry.mode.name.data(), std::min(entry.mode.name.size(), sizeof(name)));

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_available_modes_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint8_t>(_entries.size()),
            static_cast<uint8_t>(index + 1),
            entry.standard_mode,
            entry.mode.custom_mode,
            mode_properties(entry.mode),
            name,
            _seq);
        return message;
    });
}

void ModesServerImpl::send_available_modes_monitor()
{
    uint8_t seq;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        seq = _seq;
    }

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_available_modes_monitor_pack_chan(
            mavlink_address.system_id, mavlink_address.component_id, channel, &message, seq);
        return message;
    });
}

void ModesServerImpl::send_current_mode(uint32_t custom_mode, uint32_t intended_custom_mode)
{
    uint8_t standard_mode = MAV_STANDARD_MODE_NON_STANDARD;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (const auto& entry : _entries) {
            if (entry.mode.custom_mode == custom_mode) {
                standard_mode = entry.standard_mode;
                break;
            }
        }
    }

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_current_mode_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            standard_mode,
            custom_mode,
            intended_custom_mode);
        return message;
    });
}

} // namespace mavsdk
