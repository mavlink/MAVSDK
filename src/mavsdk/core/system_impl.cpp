#include "system.h"
#include "mavsdk_impl.h"
#include "mavlink_include.h"
#include "system_impl.h"
#include "plugin_impl_base.h"
#include "px4_custom_mode.h"
#include "ardupilot_custom_mode.h"
#include "request_message.h"
#include <cstdlib>
#include <functional>
#include <future>
#include <utility>

namespace mavsdk {

SystemImpl::SystemImpl(MavsdkImpl& parent) :
    Sender(),
    _parent(parent),
    _params(
        *this,
        _parent.mavlink_message_handler,
        _parent.timeout_handler,
        [this]() { return timeout_s(); },
        false),
    _command_sender(*this),
    //_request_message_handler(
    //    *this,
    //    _command_sender,
    //    _parent.mavlink_message_handler,
    //    _parent.timeout_handler),
    _timesync(*this),
    _ping(*this),
    _mission_transfer(
        *this,
        _parent.mavlink_message_handler,
        _parent.timeout_handler,
        [this]() { return timeout_s(); }),
    _request_message(
        *this, _command_sender, _parent.mavlink_message_handler, _parent.timeout_handler),
    _mavlink_ftp(*this)
{
    _system_thread = new std::thread(&SystemImpl::system_thread, this);
}

SystemImpl::~SystemImpl()
{
    _should_exit = true;
    _parent.mavlink_message_handler.unregister_all(this);

    if (!_always_connected) {
        unregister_timeout_handler(_heartbeat_timeout_cookie);
    }

    if (_system_thread != nullptr) {
        _system_thread->join();
        delete _system_thread;
        _system_thread = nullptr;
    }
}

void SystemImpl::init(uint8_t system_id, uint8_t comp_id, bool connected)
{
    _target_address.system_id = system_id;
    // FIXME: for now use this as a default.
    _target_address.component_id = MAV_COMP_ID_AUTOPILOT1;

    if (connected) {
        _always_connected = true;
        set_connected();
    }

    _parent.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) { process_heartbeat(message); },
        this);

    _parent.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_STATUSTEXT,
        [this](const mavlink_message_t& message) { process_statustext(message); },
        this);

    _parent.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        [this](const mavlink_message_t& message) { process_autopilot_version(message); },
        this);

    // register_mavlink_command_handler(
    //    MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
    //    [this](const MavlinkCommandReceiver::CommandLong& command) {
    //        return process_autopilot_version_request(command);
    //    },
    //    this);

    //// TO-DO!
    // register_mavlink_command_handler(
    //    MAV_CMD_REQUEST_MESSAGE,
    //    [this](const MavlinkCommandReceiver::CommandLong& command) {
    //        return make_command_ack_message(command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    //    },
    //    this);

    add_new_component(comp_id);
}

bool SystemImpl::is_connected() const
{
    return _connected;
}

void SystemImpl::register_mavlink_message_handler(
    uint16_t msg_id, const MavlinkMessageHandler& callback, const void* cookie)
{
    _parent.mavlink_message_handler.register_one(msg_id, callback, cookie);
}

void SystemImpl::register_mavlink_message_handler(
    uint16_t msg_id, uint8_t cmp_id, const MavlinkMessageHandler& callback, const void* cookie)
{
    _parent.mavlink_message_handler.register_one(msg_id, cmp_id, callback, cookie);
}

void SystemImpl::unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie)
{
    _parent.mavlink_message_handler.unregister_one(msg_id, cookie);
}

void SystemImpl::unregister_all_mavlink_message_handlers(const void* cookie)
{
    _parent.mavlink_message_handler.unregister_all(cookie);
}

void SystemImpl::update_componentid_messages_handler(
    uint16_t msg_id, uint8_t cmp_id, const void* cookie)
{
    _parent.mavlink_message_handler.update_component_id(msg_id, cmp_id, cookie);
}

void SystemImpl::register_timeout_handler(
    const std::function<void()>& callback, double duration_s, void** cookie)
{
    _parent.timeout_handler.add(callback, duration_s, cookie);
}

void SystemImpl::refresh_timeout_handler(const void* cookie)
{
    _parent.timeout_handler.refresh(cookie);
}

void SystemImpl::unregister_timeout_handler(const void* cookie)
{
    _parent.timeout_handler.remove(cookie);
}

double SystemImpl::timeout_s() const
{
    return _parent.timeout_s();
}

void SystemImpl::enable_timesync()
{
    _timesync.enable();
}

void SystemImpl::subscribe_is_connected(System::IsConnectedCallback callback)
{
    std::lock_guard<std::mutex> lock(_connection_mutex);
    _is_connected_callback = std::move(callback);
}

// FIXME: fix intercept
// void SystemImpl::process_mavlink_message(mavlink_message_t& message)
//{
//    // This is a low level interface where incoming messages can be tampered
//    // with or even dropped.
//    if (_incoming_messages_intercept_callback) {
//        const bool keep = _incoming_messages_intercept_callback(message);
//        if (!keep) {
//            LogDebug() << "Dropped incoming message: " << int(message.msgid);
//            return;
//        }
//    }
//
//    //_mavlink_message_handler.process_message(message);
//}

void SystemImpl::add_call_every(std::function<void()> callback, float interval_s, void** cookie)
{
    _parent.call_every_handler.add(std::move(callback), static_cast<double>(interval_s), cookie);
}

void SystemImpl::change_call_every(float interval_s, const void* cookie)
{
    _parent.call_every_handler.change(static_cast<double>(interval_s), cookie);
}

void SystemImpl::reset_call_every(const void* cookie)
{
    _parent.call_every_handler.reset(cookie);
}

void SystemImpl::remove_call_every(const void* cookie)
{
    _parent.call_every_handler.remove(cookie);
}

void SystemImpl::register_statustext_handler(
    std::function<void(const MavlinkStatustextHandler::Statustext&)> callback, void* cookie)
{
    std::lock_guard<std::mutex> lock(_statustext_handler_callbacks_mutex);
    _statustext_handler_callbacks.push_back(StatustextCallback{std::move(callback), cookie});
}

void SystemImpl::unregister_statustext_handler(void* cookie)
{
    std::lock_guard<std::mutex> lock(_statustext_handler_callbacks_mutex);
    _statustext_handler_callbacks.erase(std::remove_if(
        _statustext_handler_callbacks.begin(),
        _statustext_handler_callbacks.end(),
        [&](const auto& entry) { return entry.cookie == cookie; }));
}

void SystemImpl::process_heartbeat(const mavlink_message_t& message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    if (heartbeat.autopilot == MAV_AUTOPILOT_PX4) {
        _autopilot = Autopilot::Px4;
    } else if (heartbeat.autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA) {
        _autopilot = Autopilot::ArduPilot;
    }

    // Only set the vehicle type if the heartbeat is from an autopilot component
    // This check only works if the MAV_TYPE::MAV_TYPE_ENUM_END is actually the
    // last enumerator.
    if (MAV_TYPE::MAV_TYPE_ENUM_END < heartbeat.type) {
        LogErr() << "type received in HEARTBEAT was not recognized";
    } else {
        const auto new_vehicle_type = static_cast<MAV_TYPE>(heartbeat.type);
        if (heartbeat.autopilot != MAV_AUTOPILOT_INVALID && _vehicle_type != new_vehicle_type &&
            _vehicle_type != MAV_TYPE_GENERIC) {
            LogWarn() << "Vehicle type changed (new type: " << static_cast<unsigned>(heartbeat.type)
                      << ", old type: " << static_cast<unsigned>(_vehicle_type) << ")";
            _vehicle_type = new_vehicle_type;
        }
    }

    if (message.compid == MavlinkCommandSender::DEFAULT_COMPONENT_ID_AUTOPILOT) {
        _armed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;
        _hitl_enabled = (heartbeat.base_mode & MAV_MODE_FLAG_HIL_ENABLED) != 0;
    }
    if (heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {
        _flight_mode =
            to_flight_mode_from_custom_mode(_autopilot, _vehicle_type, heartbeat.custom_mode);
    }

    set_connected();
}

void SystemImpl::process_statustext(const mavlink_message_t& message)
{
    mavlink_statustext_t statustext;
    mavlink_msg_statustext_decode(&message, &statustext);

    const auto maybe_result = _statustext_handler.process(statustext);

    if (maybe_result.has_value()) {
        LogDebug() << "MAVLink: "
                   << MavlinkStatustextHandler::severity_str(maybe_result.value().severity) << ": "
                   << maybe_result.value().text;

        std::lock_guard<std::mutex> lock(_statustext_handler_callbacks_mutex);
        for (const auto& entry : _statustext_handler_callbacks) {
            entry.callback(maybe_result.value());
        }
    }
}

void SystemImpl::process_autopilot_version(const mavlink_message_t& message)
{
    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    _mission_transfer.set_int_messages_supported(
        autopilot_version.capabilities & MAV_PROTOCOL_CAPABILITY_MISSION_INT);
}

void SystemImpl::heartbeats_timed_out()
{
    LogInfo() << "heartbeats timed out";
    set_disconnected();
}

void SystemImpl::system_thread()
{
    dl_time_t last_ping_time{};

    while (!_should_exit) {
        _params.do_work();
        _command_sender.do_work();
        _timesync.do_work();
        _mission_transfer.do_work();

        if (_parent.time.elapsed_since_s(last_ping_time) >= SystemImpl::_ping_interval_s) {
            if (_connected) {
                _ping.run_once();
            }
            last_ping_time = _parent.time.steady_time();
        }

        if (_connected) {
            // Work fairly fast if we're connected.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            // Be less aggressive when unconnected.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

// std::optional<mavlink_message_t>
// SystemImpl::process_autopilot_version_request(const MavlinkCommandReceiver::CommandLong& command)
//{
//    if (_should_send_autopilot_version) {
//        send_autopilot_version();
//        return make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
//    }
//
//    return {};
//}

std::string SystemImpl::component_name(uint8_t component_id)
{
    switch (component_id) {
        case MAV_COMP_ID_AUTOPILOT1:
            return "Autopilot";
        case MAV_COMP_ID_CAMERA:
            return "Camera 1";
        case MAV_COMP_ID_CAMERA2:
            return "Camera 2";
        case MAV_COMP_ID_CAMERA3:
            return "Camera 3";
        case MAV_COMP_ID_CAMERA4:
            return "Camera 4";
        case MAV_COMP_ID_CAMERA5:
            return "Camera 5";
        case MAV_COMP_ID_CAMERA6:
            return "Camera 6";
        case MAV_COMP_ID_GIMBAL:
            return "Gimbal";
        case MAV_COMP_ID_MISSIONPLANNER:
            return "Ground station";
        default:
            return "Unsupported component";
    }
}

System::ComponentType SystemImpl::component_type(uint8_t component_id)
{
    switch (component_id) {
        case MAV_COMP_ID_AUTOPILOT1:
            return System::ComponentType::AUTOPILOT;
        case MAV_COMP_ID_CAMERA:
        case MAV_COMP_ID_CAMERA2:
        case MAV_COMP_ID_CAMERA3:
        case MAV_COMP_ID_CAMERA4:
        case MAV_COMP_ID_CAMERA5:
        case MAV_COMP_ID_CAMERA6:
            return System::ComponentType::CAMERA;
        case MAV_COMP_ID_GIMBAL:
            return System::ComponentType::GIMBAL;
        default:
            return System::ComponentType::UNKNOWN;
    }
}

void SystemImpl::add_new_component(uint8_t component_id)
{
    if (component_id == 0) {
        return;
    }

    auto res_pair = _components.insert(component_id);
    if (res_pair.second) {
        std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
        if (_component_discovered_callback != nullptr) {
            const System::ComponentType type = component_type(component_id);
            auto temp_callback = _component_discovered_callback;
            call_user_callback([temp_callback, type]() { temp_callback(type); });
        }
        if (_component_discovered_id_callback != nullptr) {
            const System::ComponentType type = component_type(component_id);
            auto temp_callback = _component_discovered_id_callback;
            call_user_callback(
                [temp_callback, type, component_id]() { temp_callback(type, component_id); });
        }
        LogDebug() << "Component " << component_name(component_id) << " (" << int(component_id)
                   << ") added.";
    }
}

size_t SystemImpl::total_components() const
{
    return _components.size();
}

void SystemImpl::register_component_discovered_callback(System::DiscoverCallback callback)
{
    std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
    _component_discovered_callback = std::move(callback);

    if (total_components() > 0) {
        for (const auto& elem : _components) {
            const System::ComponentType type = component_type(elem);
            if (_component_discovered_callback) {
                auto temp_callback = _component_discovered_callback;
                call_user_callback([temp_callback, type]() { temp_callback(type); });
            }
        }
    }
}

void SystemImpl::register_component_discovered_id_callback(System::DiscoverIdCallback callback)
{
    std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
    _component_discovered_id_callback = std::move(callback);

    if (total_components() > 0) {
        for (const auto& elem : _components) {
            const System::ComponentType type = component_type(elem);
            if (_component_discovered_id_callback) {
                auto temp_callback = _component_discovered_id_callback;
                call_user_callback([temp_callback, type, elem]() { temp_callback(type, elem); });
            }
        }
    }
}

bool SystemImpl::is_standalone() const
{
    return !has_autopilot();
}

bool SystemImpl::has_autopilot() const
{
    return get_autopilot_id() != uint8_t(0);
}

bool SystemImpl::is_autopilot(uint8_t comp_id)
{
    return comp_id == MAV_COMP_ID_AUTOPILOT1;
}

bool SystemImpl::is_camera(uint8_t comp_id)
{
    return (comp_id >= MAV_COMP_ID_CAMERA) && (comp_id <= MAV_COMP_ID_CAMERA6);
}

bool SystemImpl::has_camera(int camera_id) const
{
    int camera_comp_id = (camera_id == -1) ? camera_id : (MAV_COMP_ID_CAMERA + camera_id);

    if (camera_comp_id == -1) { // Check whether the system has any camera.
        if (std::any_of(_components.begin(), _components.end(), is_camera)) {
            return true;
        }
    } else { // Look for the camera whose id is `camera_id`.
        for (auto compid : _components) {
            if (compid == camera_comp_id) {
                return true;
            }
        }
    }
    return false;
}

bool SystemImpl::has_gimbal() const
{
    return get_gimbal_id() == MAV_COMP_ID_GIMBAL;
}

bool SystemImpl::send_message(mavlink_message_t& message)
{
    // This is a low level interface where incoming messages can be tampered
    // with or even dropped.
    if (_outgoing_messages_intercept_callback) {
        const bool keep = _outgoing_messages_intercept_callback(message);
        if (!keep) {
            // We fake that everything was sent as instructed because
            // a potential loss would happen later, and we would not be informed
            // about it.
            LogDebug() << "Dropped outgoing message: " << int(message.msgid);
            return true;
        }
    }

    return _parent.send_message(message);
}

void SystemImpl::send_autopilot_version_request()
{
    // We don't care about an answer, we mostly care about receiving AUTOPILOT_VERSION.
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES;
    command.params.maybe_param1 = 1.0f;
    command.target_component_id = get_autopilot_id();

    send_command_async(command, nullptr);
}

void SystemImpl::send_flight_information_request()
{
    // We don't care about an answer, we mostly care about receiving FLIGHT_INFORMATION.
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_REQUEST_FLIGHT_INFORMATION;
    command.params.maybe_param1 = 1.0f;
    command.target_component_id = get_autopilot_id();

    send_command_async(command, nullptr);
}

void SystemImpl::set_connected()
{
    bool enable_needed = false;
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);

        if (!_connected) {
            if (!_components.empty()) {
                LogDebug() << "Discovered " << _components.size() << " component(s)";
            }

            _connected = true;

            // System with sysid 0 is a bit special: it is a placeholder for a connection initiated
            // by MAVSDK. As such, it should not be advertised as a newly discovered system.
            if (static_cast<int>(get_system_id()) != 0) {
                _parent.notify_on_discover();
            }

            // We call this later to avoid deadlocks on creating the server components.
            _parent.call_user_callback([this]() {
                // Send a heartbeat back immediately.
                _parent.start_sending_heartbeats();
            });

            if (!_always_connected) {
                register_timeout_handler(
                    [this] { heartbeats_timed_out(); },
                    HEARTBEAT_TIMEOUT_S,
                    &_heartbeat_timeout_cookie);
            }
            enable_needed = true;

            if (_is_connected_callback) {
                const auto temp_callback = _is_connected_callback;
                _parent.call_user_callback([temp_callback]() { temp_callback(true); });
            }

        } else if (_connected && !_always_connected) {
            refresh_timeout_handler(_heartbeat_timeout_cookie);
        }
        // If not yet connected there is nothing to do/
    }
    if (enable_needed) {
        if (has_autopilot()) {
            send_autopilot_version_request();
        }

        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        for (auto plugin_impl : _plugin_impls) {
            plugin_impl->enable();
        }
    }
}

void SystemImpl::set_disconnected()
{
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);

        // This might not be needed because this is probably called from the triggered
        // timeout anyway, but it should also do no harm.
        // unregister_timeout_handler(_heartbeat_timeout_cookie);
        //_heartbeat_timeout_cookie = nullptr;

        _connected = false;
        _parent.notify_on_timeout();
        if (_is_connected_callback) {
            const auto temp_callback = _is_connected_callback;
            _parent.call_user_callback([temp_callback]() { temp_callback(false); });
        }
    }

    _parent.stop_sending_heartbeats();

    {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        for (auto plugin_impl : _plugin_impls) {
            plugin_impl->disable();
        }
    }
}

uint8_t SystemImpl::get_system_id() const
{
    return _target_address.system_id;
}

std::vector<uint8_t> SystemImpl::component_ids() const
{
    return std::vector<uint8_t>{_components.begin(), _components.end()};
}

void SystemImpl::set_system_id(uint8_t system_id)
{
    _target_address.system_id = system_id;
}

uint8_t SystemImpl::get_own_system_id() const
{
    return _parent.get_own_system_id();
}

uint8_t SystemImpl::get_own_component_id() const
{
    return _parent.get_own_component_id();
}

MAV_TYPE SystemImpl::get_vehicle_type() const
{
    return _vehicle_type;
}

uint8_t SystemImpl::get_own_mav_type() const
{
    return _parent.get_mav_type();
}

MAVLinkParameters::Result SystemImpl::set_param(
    const std::string& name,
    MAVLinkParameters::ParamValue value,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    return _params.set_param(name, value, maybe_component_id, extended);
}

MAVLinkParameters::Result SystemImpl::set_param_float(
    const std::string& name, float value, std::optional<uint8_t> maybe_component_id, bool extended)
{
    return _params.set_param_float(name, value, maybe_component_id, extended);
}

MAVLinkParameters::Result SystemImpl::set_param_int(
    const std::string& name,
    int32_t value,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    return _params.set_param_int(name, value, maybe_component_id, extended);
}

MAVLinkParameters::Result
SystemImpl::set_param_custom(const std::string& name, const std::string& value)
{
    return _params.set_param_custom(name, value);
}

std::map<std::string, MAVLinkParameters::ParamValue> SystemImpl::get_all_params()
{
    return _params.get_all_params();
}

void SystemImpl::set_param_async(
    const std::string& name,
    MAVLinkParameters::ParamValue value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    _params.set_param_async(name, value, callback, cookie, maybe_component_id, extended);
}

void SystemImpl::set_param_float_async(
    const std::string& name,
    float value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    _params.set_param_float_async(name, value, callback, cookie, maybe_component_id, extended);
}

MAVLinkParameters::Result
SystemImpl::provide_server_param_int(const std::string& name, int32_t value)
{
    return _params.provide_server_param_int(name, value);
}

MAVLinkParameters::Result
SystemImpl::provide_server_param_float(const std::string& name, float value)
{
    return _params.provide_server_param_float(name, value);
}

MAVLinkParameters::Result
SystemImpl::provide_server_param_custom(const std::string& name, const std::string& value)
{
    return _params.provide_server_param_custom(name, value);
}

void SystemImpl::set_param_int_async(
    const std::string& name,
    int32_t value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    _params.set_param_int_async(name, value, callback, cookie, maybe_component_id, extended);
}

std::pair<MAVLinkParameters::Result, int>
SystemImpl::retrieve_server_param_int(const std::string& name)
{
    return _params.retrieve_server_param_int(name);
}

std::pair<MAVLinkParameters::Result, float>
SystemImpl::retrieve_server_param_float(const std::string& name)
{
    return _params.retrieve_server_param_float(name);
}

std::pair<MAVLinkParameters::Result, std::string>
SystemImpl::retrieve_server_param_custom(const std::string& name)
{
    return _params.retrieve_server_param_custom(name);
}

std::map<std::string, MAVLinkParameters::ParamValue> SystemImpl::retrieve_all_server_params()
{
    return _params.retrieve_all_server_params();
}

std::pair<MAVLinkParameters::Result, float> SystemImpl::get_param_float(const std::string& name)
{
    return _params.get_param_float(name, {}, false);
}

std::pair<MAVLinkParameters::Result, int> SystemImpl::get_param_int(const std::string& name)
{
    return _params.get_param_int(name, {}, false);
}

std::pair<MAVLinkParameters::Result, std::string>
SystemImpl::get_param_custom(const std::string& name)
{
    return _params.get_param_custom(name);
}

void SystemImpl::get_param_async(
    const std::string& name,
    MAVLinkParameters::ParamValue value,
    const GetParamAnyCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    _params.get_param_async(name, value, callback, cookie, maybe_component_id, extended);
}

void SystemImpl::get_param_float_async(
    const std::string& name,
    const GetParamFloatCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    _params.get_param_float_async(name, callback, cookie, maybe_component_id, extended);
}

void SystemImpl::get_param_int_async(
    const std::string& name,
    const GetParamIntCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    _params.get_param_int_async(name, callback, cookie, maybe_component_id, extended);
}

void SystemImpl::get_param_custom_async(
    const std::string& name, const GetParamCustomCallback& callback, const void* cookie)
{
    _params.get_param_custom_async(name, callback, cookie);
}

void SystemImpl::cancel_all_param(const void* cookie)
{
    _params.cancel_all_param(cookie);
}

void SystemImpl::subscribe_param_int(
    const std::string& name, const SubscribeParamIntCallback& callback, const void* cookie)
{
    MAVLinkParameters::ParamValue value_type;
    value_type.set<int32_t>(0);

    _params.subscribe_param_changed(
        name,
        value_type,
        [callback](MAVLinkParameters::ParamValue value) { callback(value.get<int32_t>()); },
        cookie);
}

void SystemImpl::subscribe_param_float(
    const std::string& name, const SubscribeParamFloatCallback& callback, const void* cookie)
{
    MAVLinkParameters::ParamValue value_type;
    value_type.set<float>(0.0f);

    _params.subscribe_param_changed(
        name,
        value_type,
        [callback](MAVLinkParameters::ParamValue value) { callback(value.get<float>()); },
        cookie);
}

MavlinkCommandSender::Result
SystemImpl::set_flight_mode(FlightMode system_mode, uint8_t component_id)
{
    std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong> result =
        make_command_flight_mode(system_mode, component_id);

    if (result.first != MavlinkCommandSender::Result::Success) {
        return result.first;
    }

    return send_command(result.second);
}

void SystemImpl::set_flight_mode_async(
    FlightMode system_mode, const CommandResultCallback& callback, uint8_t component_id)
{
    std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong> result =
        make_command_flight_mode(system_mode, component_id);

    if (result.first != MavlinkCommandSender::Result::Success) {
        if (callback) {
            callback(result.first, NAN);
        }
        return;
    }

    send_command_async(result.second, callback);
}

std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
SystemImpl::make_command_flight_mode(FlightMode flight_mode, uint8_t component_id)
{
    if (_autopilot == Autopilot::ArduPilot) {
        return make_command_ardupilot_mode(flight_mode, component_id);
    } else {
        return make_command_px4_mode(flight_mode, component_id);
    }
}

std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
SystemImpl::make_command_ardupilot_mode(FlightMode flight_mode, uint8_t component_id)
{
    const uint8_t flag_safety_armed = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;
    const uint8_t mode_type =
        MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed | flag_hitl_enabled;

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_SET_MODE;
    command.params.maybe_param1 = static_cast<float>(mode_type);

    switch (_vehicle_type) {
        case MAV_TYPE::MAV_TYPE_SURFACE_BOAT:
        case MAV_TYPE::MAV_TYPE_GROUND_ROVER:
            if (flight_mode_to_ardupilot_rover_mode(flight_mode) == ardupilot::RoverMode::Unknown) {
                LogErr() << "Cannot translate flight mode to ardupilot rover mode.";
                MavlinkCommandSender::CommandLong empty_command{};
                return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
            } else {
                command.params.maybe_param2 =
                    static_cast<float>(flight_mode_to_ardupilot_rover_mode(flight_mode));
            }
            break;
        default:
            if (flight_mode_to_ardupilot_copter_mode(flight_mode) ==
                ardupilot::CopterMode::Unknown) {
                LogErr() << "Cannot translate flight mode to ardupilot copter mode.";
                MavlinkCommandSender::CommandLong empty_command{};
                return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
            } else {
                command.params.maybe_param2 =
                    static_cast<float>(flight_mode_to_ardupilot_copter_mode(flight_mode));
            }
            break;
    }
    command.target_component_id = component_id;

    return std::make_pair<>(MavlinkCommandSender::Result::Success, command);
}
ardupilot::RoverMode SystemImpl::flight_mode_to_ardupilot_rover_mode(FlightMode flight_mode)
{
    switch (flight_mode) {
        case FlightMode::Mission:
            return ardupilot::RoverMode::Auto;
        case FlightMode::Acro:
            return ardupilot::RoverMode::Acro;
        case FlightMode::Hold:
            return ardupilot::RoverMode::Hold;
        case FlightMode::ReturnToLaunch:
            return ardupilot::RoverMode::RTL;
        case FlightMode::Manual:
            return ardupilot::RoverMode::Manual;
        case FlightMode::FollowMe:
            return ardupilot::RoverMode::Follow;
        case FlightMode::Unknown:
        case FlightMode::Ready:
        case FlightMode::Takeoff:
        case FlightMode::Land:
        case FlightMode::Offboard:
        case FlightMode::Altctl:
        case FlightMode::Posctl:
        case FlightMode::Rattitude:
        case FlightMode::Stabilized:
        default:
            return ardupilot::RoverMode::Unknown;
    }
}
ardupilot::CopterMode SystemImpl::flight_mode_to_ardupilot_copter_mode(FlightMode flight_mode)
{
    switch (flight_mode) {
        case FlightMode::Mission:
            return ardupilot::CopterMode::Auto;
        case FlightMode::Acro:
            return ardupilot::CopterMode::Acro;
        case FlightMode::Hold:
            return ardupilot::CopterMode::Alt_Hold;
        case FlightMode::ReturnToLaunch:
            return ardupilot::CopterMode::RTL;
        case FlightMode::Land:
            return ardupilot::CopterMode::Land;
        case FlightMode::Manual:
        case FlightMode::FollowMe:
        case FlightMode::Unknown:
        case FlightMode::Ready:
        case FlightMode::Takeoff:
        case FlightMode::Offboard:
        case FlightMode::Altctl:
        case FlightMode::Posctl:
        case FlightMode::Rattitude:
        case FlightMode::Stabilized:
        default:
            return ardupilot::CopterMode::Unknown;
    }
}

std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
SystemImpl::make_command_px4_mode(FlightMode flight_mode, uint8_t component_id)
{
    const uint8_t flag_safety_armed = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;

    const uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed | flag_hitl_enabled;

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = 0;

    switch (flight_mode) {
        case FlightMode::Hold:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case FlightMode::ReturnToLaunch:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case FlightMode::Takeoff:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case FlightMode::Land:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case FlightMode::Mission:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case FlightMode::FollowMe:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case FlightMode::Offboard:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            break;
        case FlightMode::Manual:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_MANUAL;
            break;
        case FlightMode::Posctl:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_POSCTL;
            break;
        case FlightMode::Altctl:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_ALTCTL;
            break;
        case FlightMode::Rattitude:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE;
            break;
        case FlightMode::Acro:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_ACRO;
            break;
        case FlightMode::Stabilized:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_STABILIZED;
            break;
        default:
            LogErr() << "Unknown Flight mode.";
            MavlinkCommandSender::CommandLong empty_command{};
            return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
    }

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_SET_MODE;
    command.params.maybe_param1 = static_cast<float>(mode);
    command.params.maybe_param2 = static_cast<float>(custom_mode);
    command.params.maybe_param3 = static_cast<float>(custom_sub_mode);
    command.target_component_id = component_id;

    return std::make_pair<>(MavlinkCommandSender::Result::Success, command);
}

FlightMode SystemImpl::get_flight_mode() const
{
    return _flight_mode;
}

void SystemImpl::receive_float_param(
    MAVLinkParameters::Result result,
    MAVLinkParameters::ParamValue value,
    const GetParamFloatCallback& callback)
{
    if (callback) {
        if (result == MAVLinkParameters::Result::Success) {
            callback(result, value.get<float>());
        } else {
            callback(result, NAN);
        }
    }
}

void SystemImpl::receive_int_param(
    MAVLinkParameters::Result result,
    MAVLinkParameters::ParamValue value,
    const GetParamIntCallback& callback)
{
    if (callback) {
        if (result == MAVLinkParameters::Result::Success) {
            callback(result, value.get<int32_t>());
        } else {
            callback(result, 0);
        }
    }
}

uint8_t SystemImpl::get_autopilot_id() const
{
    for (auto compid : _components)
        if (compid == MavlinkCommandSender::DEFAULT_COMPONENT_ID_AUTOPILOT) {
            return compid;
        }
    // FIXME: Not sure what should be returned if autopilot is not found
    return uint8_t(0);
}

std::vector<uint8_t> SystemImpl::get_camera_ids() const
{
    std::vector<uint8_t> camera_ids{};

    for (auto compid : _components)
        if (compid >= MAV_COMP_ID_CAMERA && compid <= MAV_COMP_ID_CAMERA6) {
            camera_ids.push_back(compid);
        }
    return camera_ids;
}

uint8_t SystemImpl::get_gimbal_id() const
{
    for (auto compid : _components)
        if (compid == MAV_COMP_ID_GIMBAL) {
            return compid;
        }
    return uint8_t(0);
}

MavlinkCommandSender::Result SystemImpl::send_command(MavlinkCommandSender::CommandLong& command)
{
    if (_target_address.system_id == 0 && _components.empty()) {
        return MavlinkCommandSender::Result::NoSystem;
    }
    command.target_system_id = get_system_id();
    return _command_sender.send_command(command);
}

MavlinkCommandSender::Result SystemImpl::send_command(MavlinkCommandSender::CommandInt& command)
{
    if (_target_address.system_id == 0 && _components.empty()) {
        return MavlinkCommandSender::Result::NoSystem;
    }
    command.target_system_id = get_system_id();
    return _command_sender.send_command(command);
}

void SystemImpl::send_command_async(
    MavlinkCommandSender::CommandLong command, const CommandResultCallback& callback)
{
    if (_target_address.system_id == 0 && _components.empty()) {
        if (callback) {
            callback(MavlinkCommandSender::Result::NoSystem, NAN);
        }
        return;
    }
    command.target_system_id = get_system_id();

    _command_sender.queue_command_async(command, callback);
}

void SystemImpl::send_command_async(
    MavlinkCommandSender::CommandInt command, const CommandResultCallback& callback)
{
    if (_target_address.system_id == 0 && _components.empty()) {
        if (callback) {
            callback(MavlinkCommandSender::Result::NoSystem, NAN);
        }
        return;
    }
    command.target_system_id = get_system_id();

    _command_sender.queue_command_async(command, callback);
}

MavlinkCommandSender::Result
SystemImpl::set_msg_rate(uint16_t message_id, double rate_hz, uint8_t component_id)
{
    MavlinkCommandSender::CommandLong command =
        make_command_msg_rate(message_id, rate_hz, component_id);
    return send_command(command);
}

void SystemImpl::set_msg_rate_async(
    uint16_t message_id,
    double rate_hz,
    const CommandResultCallback& callback,
    uint8_t component_id)
{
    MavlinkCommandSender::CommandLong command =
        make_command_msg_rate(message_id, rate_hz, component_id);
    send_command_async(command, callback);
}

MavlinkCommandSender::CommandLong
SystemImpl::make_command_msg_rate(uint16_t message_id, double rate_hz, uint8_t component_id)
{
    MavlinkCommandSender::CommandLong command{};

    // 0 to request default rate, -1 to stop stream

    float interval_us = 0.0f;

    if (rate_hz > 0) {
        interval_us = 1e6f / static_cast<float>(rate_hz);
    } else if (rate_hz < 0) {
        interval_us = -1.0f;
    }

    command.command = MAV_CMD_SET_MESSAGE_INTERVAL;
    command.params.maybe_param1 = static_cast<float>(message_id);
    command.params.maybe_param2 = interval_us;
    command.target_component_id = component_id;

    return command;
}

void SystemImpl::register_plugin(PluginImplBase* plugin_impl)
{
    assert(plugin_impl);

    plugin_impl->init();

    {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        _plugin_impls.push_back(plugin_impl);
    }

    // If we're connected already, let's enable it straightaway.
    if (_connected) {
        plugin_impl->enable();
    }
}

void SystemImpl::unregister_plugin(PluginImplBase* plugin_impl)
{
    assert(plugin_impl);

    plugin_impl->disable();
    plugin_impl->deinit();

    // Remove first, so it won't get enabled/disabled anymore.
    {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        auto found = std::find(_plugin_impls.begin(), _plugin_impls.end(), plugin_impl);
        if (found != _plugin_impls.end()) {
            _plugin_impls.erase(found);
        }
    }
}

void SystemImpl::call_user_callback_located(
    const std::string& filename, const int linenumber, const std::function<void()>& func)
{
    _parent.call_user_callback_located(filename, linenumber, func);
}

void SystemImpl::param_changed(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_param_changed_callbacks_mutex);

    for (auto& callback : _param_changed_callbacks) {
        callback.second(name);
    }
}

void SystemImpl::register_param_changed_handler(
    const param_changed_callback_t& callback, const void* cookie)
{
    if (!callback) {
        LogErr() << "No callback for param_changed_handler supplied.";
        return;
    }

    if (!cookie) {
        LogErr() << "No callback for param_changed_handler supplied.";
        return;
    }

    std::lock_guard<std::mutex> lock(_param_changed_callbacks_mutex);

    _param_changed_callbacks[cookie] = callback;
}

void SystemImpl::unregister_param_changed_handler(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_param_changed_callbacks_mutex);

    auto it = _param_changed_callbacks.find(cookie);
    if (it == _param_changed_callbacks.end()) {
        LogWarn() << "param_changed_handler for cookie not found";
        return;
    }
    _param_changed_callbacks.erase(it);
}

void SystemImpl::intercept_incoming_messages(std::function<bool(mavlink_message_t&)> callback)
{
    _incoming_messages_intercept_callback = std::move(callback);
}

void SystemImpl::intercept_outgoing_messages(std::function<bool(mavlink_message_t&)> callback)
{
    _outgoing_messages_intercept_callback = std::move(callback);
}

Time& SystemImpl::get_time()
{
    return _parent.time;
};

} // namespace mavsdk
