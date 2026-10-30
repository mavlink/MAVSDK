#include "modes_server_impl.hpp"
#include "standard_modes.hpp"

#include <algorithm>
#include <limits>

namespace mavsdk {

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

    // We only describe the modes, changing them is up to ActionServer. Rejecting the standard
    // mode command makes clients fall back to MAV_CMD_DO_SET_MODE.
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_DO_SET_STANDARD_MODE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return _server_component_impl->make_command_ack_message(
                command, MAV_RESULT_UNSUPPORTED);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_DO_SET_STANDARD_MODE,
        [this](const MavlinkCommandReceiver::CommandInt& command) {
            return _server_component_impl->make_command_ack_message(
                command, MAV_RESULT_UNSUPPORTED);
        },
        this);

    _call_every_cookie = _server_component_impl->add_call_every(
        [this]() { check_current_mode(); }, check_interval_s);
}

void ModesServerImpl::deinit()
{
    _server_component_impl->remove_call_every_blocking(_call_every_cookie);
    _server_component_impl->unregister_all_mavlink_command_handlers(this);
    _server_component_impl->mavlink_request_message_handler().unregister_all_handlers(this);
}

ModesServer::Result
ModesServerImpl::set_available_modes(const std::vector<ModesServer::Mode>& modes)
{
    if (modes.size() > std::numeric_limits<uint8_t>::max()) {
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
        // Report the current mode again, its standard mode might have changed.
        _last_custom_mode.reset();
    }

    send_available_modes_monitor();

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

void ModesServerImpl::check_current_mode()
{
    const uint32_t custom_mode = _server_component_impl->get_custom_mode();

    bool send_current = false;
    bool send_monitor = false;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_modes_set) {
            return;
        }

        if (++_ticks >= ticks_per_periodic_send) {
            _ticks = 0;
            send_current = true;
            send_monitor = true;
        }

        if (_last_custom_mode != custom_mode) {
            _last_custom_mode = custom_mode;
            send_current = true;
        }
    }

    if (send_current) {
        send_current_mode(custom_mode);
    }
    if (send_monitor) {
        send_available_modes_monitor();
    }
}

void ModesServerImpl::send_available_mode(size_t index)
{
    // Called with _mutex held.
    const auto& entry = _entries[index];

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
            entry.mode.name.c_str(),
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

void ModesServerImpl::send_current_mode(uint32_t custom_mode)
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
            custom_mode);
        return message;
    });
}

} // namespace mavsdk
