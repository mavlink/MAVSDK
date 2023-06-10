#include "system.h"
#include "mavsdk_impl.h"
#include "mavlink_include.h"
#include "system_impl.h"
#include "plugin_impl_base.h"
#include "px4_custom_mode.h"
#include "ardupilot_custom_mode.h"
#include "request_message.h"
#include "callback_list.tpp"
#include "unused.h"
#include <cassert>
#include <cstdlib>
#include <functional>
#include <future>
#include <utility>

namespace mavsdk {

template class CallbackList<bool>;
template class CallbackList<System::ComponentType>;
template class CallbackList<System::ComponentType, uint8_t>;

SystemImpl::SystemImpl(MavsdkImpl& parent) :
    Sender(),
    _mavsdk_impl(parent),
    _command_sender(*this),
    _timesync(*this),
    _ping(*this),
    _mission_transfer(
        *this,
        _mavsdk_impl.mavlink_message_handler,
        _mavsdk_impl.timeout_handler,
        [this]() { return timeout_s(); }),
    _request_message(
        *this, _command_sender, _mavsdk_impl.mavlink_message_handler, _mavsdk_impl.timeout_handler),
    _mavlink_ftp(*this)
{
    _system_thread = new std::thread(&SystemImpl::system_thread, this);
}

SystemImpl::~SystemImpl()
{
    _should_exit = true;
    _mavsdk_impl.mavlink_message_handler.unregister_all(this);

    unregister_timeout_handler(_heartbeat_timeout_cookie);

    if (_system_thread != nullptr) {
        _system_thread->join();
        delete _system_thread;
        _system_thread = nullptr;
    }
}

void SystemImpl::init(uint8_t system_id, uint8_t comp_id)
{
    _target_address.system_id = system_id;
    // We use this as a default.
    _target_address.component_id = MAV_COMP_ID_AUTOPILOT1;

    _mavsdk_impl.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) { process_heartbeat(message); },
        this);

    _mavsdk_impl.mavlink_message_handler.register_one(
        MAVLINK_MSG_ID_STATUSTEXT,
        [this](const mavlink_message_t& message) { process_statustext(message); },
        this);

    _mavsdk_impl.mavlink_message_handler.register_one(
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
    _mavsdk_impl.mavlink_message_handler.register_one(msg_id, callback, cookie);
}

void SystemImpl::register_mavlink_message_handler(
    uint16_t msg_id, uint8_t cmp_id, const MavlinkMessageHandler& callback, const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.register_one(msg_id, cmp_id, callback, cookie);
}

void SystemImpl::unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.unregister_one(msg_id, cookie);
}

void SystemImpl::unregister_all_mavlink_message_handlers(const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.unregister_all(cookie);
}

void SystemImpl::update_componentid_messages_handler(
    uint16_t msg_id, uint8_t cmp_id, const void* cookie)
{
    _mavsdk_impl.mavlink_message_handler.update_component_id(msg_id, cmp_id, cookie);
}

void SystemImpl::register_timeout_handler(
    const std::function<void()>& callback, double duration_s, void** cookie)
{
    _mavsdk_impl.timeout_handler.add(callback, duration_s, cookie);
}

void SystemImpl::refresh_timeout_handler(const void* cookie)
{
    _mavsdk_impl.timeout_handler.refresh(cookie);
}

void SystemImpl::unregister_timeout_handler(const void* cookie)
{
    _mavsdk_impl.timeout_handler.remove(cookie);
}

double SystemImpl::timeout_s() const
{
    return _mavsdk_impl.timeout_s();
}

void SystemImpl::enable_timesync()
{
    _timesync.enable();
}

System::IsConnectedHandle
SystemImpl::subscribe_is_connected(const System::IsConnectedCallback& callback)
{
    std::lock_guard<std::mutex> lock(_connection_mutex);
    return _is_connected_callbacks.subscribe(callback);
}

void SystemImpl::unsubscribe_is_connected(System::IsConnectedHandle handle)
{
    _is_connected_callbacks.unsubscribe(handle);
}

void SystemImpl::add_call_every(std::function<void()> callback, float interval_s, void** cookie)
{
    _mavsdk_impl.call_every_handler.add(
        std::move(callback), static_cast<double>(interval_s), cookie);
}

void SystemImpl::change_call_every(float interval_s, const void* cookie)
{
    _mavsdk_impl.call_every_handler.change(static_cast<double>(interval_s), cookie);
}

void SystemImpl::reset_call_every(const void* cookie)
{
    _mavsdk_impl.call_every_handler.reset(cookie);
}

void SystemImpl::remove_call_every(const void* cookie)
{
    _mavsdk_impl.call_every_handler.remove(cookie);
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
            new_vehicle_type != MAV_TYPE_GENERIC) {
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
    SteadyTimePoint last_ping_time{};

    while (!_should_exit) {
        {
            std::lock_guard<std::mutex> lock(_mavlink_parameter_clients_mutex);
            for (auto& entry : _mavlink_parameter_clients) {
                entry.parameter_client->do_work();
            }
        }
        _command_sender.do_work();
        _timesync.do_work();
        _mission_transfer.do_work();

        if (_mavsdk_impl.time.elapsed_since_s(last_ping_time) >= SystemImpl::_ping_interval_s) {
            if (_connected) {
                _ping.run_once();
            }
            last_ping_time = _mavsdk_impl.time.steady_time();
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
        case MAV_COMP_ID_WINCH:
            return "Winch";
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
        _component_discovered_callbacks.queue(
            component_type(component_id), [this](const auto& func) { call_user_callback(func); });
        _component_discovered_id_callbacks.queue(
            component_type(component_id), component_id, [this](const auto& func) {
                call_user_callback(func);
            });
        LogDebug() << "Component " << component_name(component_id) << " (" << int(component_id)
                   << ") added.";
    }
}

size_t SystemImpl::total_components() const
{
    return _components.size();
}

System::ComponentDiscoveredHandle
SystemImpl::subscribe_component_discovered(const System::ComponentDiscoveredCallback& callback)
{
    std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
    const auto handle = _component_discovered_callbacks.subscribe(callback);

    if (total_components() > 0) {
        for (const auto& elem : _components) {
            _component_discovered_callbacks.queue(
                component_type(elem), [this](const auto& func) { call_user_callback(func); });
        }
    }
    return handle;
}

void SystemImpl::unsubscribe_component_discovered(System::ComponentDiscoveredHandle handle)
{
    std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
    _component_discovered_callbacks.unsubscribe(handle);
}

System::ComponentDiscoveredIdHandle
SystemImpl::subscribe_component_discovered_id(const System::ComponentDiscoveredIdCallback& callback)
{
    std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
    const auto handle = _component_discovered_id_callbacks.subscribe(callback);

    if (total_components() > 0) {
        for (const auto& elem : _components) {
            _component_discovered_id_callbacks.queue(
                component_type(elem), elem, [this](const auto& func) { call_user_callback(func); });
        }
    }
    return handle;
}

void SystemImpl::unsubscribe_component_discovered_id(System::ComponentDiscoveredIdHandle handle)
{
    std::lock_guard<std::mutex> lock(_component_discovered_callback_mutex);
    _component_discovered_id_callbacks.unsubscribe(handle);
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
    return _mavsdk_impl.send_message(message);
}

void SystemImpl::send_autopilot_version_request()
{
    auto prom = std::promise<MavlinkCommandSender::Result>();
    auto fut = prom.get_future();

    send_autopilot_version_request_async(
        [&prom](MavlinkCommandSender::Result result, float) { prom.set_value(result); });

    if (fut.get() == MavlinkCommandSender::Result::Unsupported) {
        _old_message_520_supported = false;
        LogWarn() << "Trying alternative command (512).";
        send_autopilot_version_request();
    }
}

void SystemImpl::send_autopilot_version_request_async(
    const MavlinkCommandSender::CommandResultCallback& callback)
{
    MavlinkCommandSender::CommandLong command{};
    command.target_component_id = get_autopilot_id();

    if (_old_message_520_supported) {
        // Note: This MAVLINK message is deprecated and would be removed from MAVSDK in a future
        // release.
        command.command = MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES;
        command.params.maybe_param1 = 1.0f;
    } else {
        command.command = MAV_CMD_REQUEST_MESSAGE;
        command.params.maybe_param1 = {static_cast<float>(MAVLINK_MSG_ID_AUTOPILOT_VERSION)};
    }

    send_command_async(command, callback);
}

void SystemImpl::send_flight_information_request()
{
    auto prom = std::promise<MavlinkCommandSender::Result>();
    auto fut = prom.get_future();

    MavlinkCommandSender::CommandLong command{};
    command.target_component_id = get_autopilot_id();

    if (_old_message_528_supported) {
        // Note: This MAVLINK message is deprecated and would be removed from MAVSDK in a future
        // release.
        command.command = MAV_CMD_REQUEST_FLIGHT_INFORMATION;
        command.params.maybe_param1 = 1.0f;
    } else {
        command.command = MAV_CMD_REQUEST_MESSAGE;
        command.params.maybe_param1 = {static_cast<float>(MAVLINK_MSG_ID_FLIGHT_INFORMATION)};
    }

    send_command_async(
        command, [&prom](MavlinkCommandSender::Result result, float) { prom.set_value(result); });
    if (fut.get() == MavlinkCommandSender::Result::Unsupported) {
        _old_message_528_supported = false;
        LogWarn() << "Trying alternative command (512)..";
        send_flight_information_request();
    }
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
                _mavsdk_impl.notify_on_discover();
            }

            // We call this later to avoid deadlocks on creating the server components.
            _mavsdk_impl.call_user_callback([this]() {
                // Send a heartbeat back immediately.
                _mavsdk_impl.start_sending_heartbeats();
            });

            register_timeout_handler(
                [this] { heartbeats_timed_out(); },
                HEARTBEAT_TIMEOUT_S,
                &_heartbeat_timeout_cookie);

            enable_needed = true;

            _is_connected_callbacks.queue(
                true, [this](const auto& func) { _mavsdk_impl.call_user_callback(func); });

        } else if (_connected) {
            refresh_timeout_handler(_heartbeat_timeout_cookie);
        }
        // If not yet connected there is nothing to do/
    }
    if (enable_needed) {
        if (has_autopilot()) {
            send_autopilot_version_request_async(nullptr);
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
        _mavsdk_impl.notify_on_timeout();
        _is_connected_callbacks.queue(
            false, [this](const auto& func) { _mavsdk_impl.call_user_callback(func); });
    }

    _mavsdk_impl.stop_sending_heartbeats();

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
    return _mavsdk_impl.get_own_system_id();
}

uint8_t SystemImpl::get_own_component_id() const
{
    return _mavsdk_impl.get_own_component_id();
}

MAV_TYPE SystemImpl::get_vehicle_type() const
{
    return _vehicle_type;
}

uint8_t SystemImpl::get_own_mav_type() const
{
    return _mavsdk_impl.get_mav_type();
}

MavlinkParameterClient::Result SystemImpl::set_param(
    const std::string& name,
    ParamValue value,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->set_param(name, value);
}

MavlinkParameterClient::Result SystemImpl::set_param_float(
    const std::string& name, float value, std::optional<uint8_t> maybe_component_id, bool extended)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->set_param_float(name, value);
}

MavlinkParameterClient::Result SystemImpl::set_param_int(
    const std::string& name,
    int32_t value,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->set_param_int(name, value);
}

MavlinkParameterClient::Result SystemImpl::set_param_custom(
    const std::string& name, const std::string& value, std::optional<uint8_t> maybe_component_id)
{
    return param_sender(maybe_component_id.has_value() ? maybe_component_id.value() : 1, true)
        ->set_param_custom(name, value);
}

std::pair<MavlinkParameterClient::Result, std::map<std::string, ParamValue>>
SystemImpl::get_all_params(std::optional<uint8_t> maybe_component_id, bool extended)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->get_all_params();
}

void SystemImpl::set_param_async(
    const std::string& name,
    ParamValue value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->set_param_async(name, value, callback, cookie);
}

void SystemImpl::set_param_float_async(
    const std::string& name,
    float value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->set_param_float_async(name, value, callback, cookie);
}

void SystemImpl::set_param_int_async(
    const std::string& name,
    int32_t value,
    const SetParamCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->set_param_int_async(name, value, callback, cookie);
}

std::pair<MavlinkParameterClient::Result, float> SystemImpl::get_param_float(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->get_param_float(name);
}

std::pair<MavlinkParameterClient::Result, int> SystemImpl::get_param_int(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->get_param_int(name);
}

std::pair<MavlinkParameterClient::Result, std::string>
SystemImpl::get_param_custom(const std::string& name, std::optional<uint8_t> maybe_component_id)
{
    return param_sender(maybe_component_id ? maybe_component_id.value() : 1, true)
        ->get_param_custom(name);
}

void SystemImpl::get_param_async(
    const std::string& name,
    ParamValue value,
    const GetParamAnyCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->get_param_async(name, value, callback, cookie);
}

void SystemImpl::get_param_float_async(
    const std::string& name,
    const GetParamFloatCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->get_param_float_async(name, callback, cookie);
}

void SystemImpl::get_param_int_async(
    const std::string& name,
    const GetParamIntCallback& callback,
    const void* cookie,
    std::optional<uint8_t> maybe_component_id,
    bool extended)
{
    param_sender(maybe_component_id ? maybe_component_id.value() : 1, extended)
        ->get_param_int_async(name, callback, cookie);
}

void SystemImpl::get_param_custom_async(
    const std::string& name, const GetParamCustomCallback& callback, const void* cookie)
{
    param_sender(1, false)->get_param_custom_async(name, callback, cookie);
}

void SystemImpl::cancel_all_param(const void* cookie)
{
    UNUSED(cookie);
    // FIXME: this currently crashes on destruction
    // param_sender(1, false)->cancel_all_param(cookie);
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
        case MAV_TYPE::MAV_TYPE_GROUND_ROVER: {
            const auto new_mode = flight_mode_to_ardupilot_rover_mode(flight_mode);
            if (new_mode == ardupilot::RoverMode::Unknown) {
                LogErr() << "Cannot translate flight mode to ArduPilot Rover mode.";
                MavlinkCommandSender::CommandLong empty_command{};
                return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
            } else {
                command.params.maybe_param2 = static_cast<float>(new_mode);
            }
            break;
        }
        case MAV_TYPE::MAV_TYPE_FIXED_WING:
        case MAV_TYPE::MAV_TYPE_VTOL_TAILSITTER_DUOROTOR:
        case MAV_TYPE::MAV_TYPE_VTOL_TAILSITTER_QUADROTOR:
        case MAV_TYPE::MAV_TYPE_VTOL_TILTROTOR:
        case MAV_TYPE::MAV_TYPE_VTOL_FIXEDROTOR:
        case MAV_TYPE::MAV_TYPE_VTOL_TAILSITTER:
        case MAV_TYPE::MAV_TYPE_VTOL_TILTWING: {
            const auto new_mode = flight_mode_to_ardupilot_plane_mode(flight_mode);
            if (new_mode == ardupilot::PlaneMode::Unknown) {
                LogErr() << "Cannot translate flight mode to ArduPilot Plane mode.";
                MavlinkCommandSender::CommandLong empty_command{};
                return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
            } else {
                command.params.maybe_param2 = static_cast<float>(new_mode);
            }
            break;
        }

        case MAV_TYPE::MAV_TYPE_QUADROTOR:
        case MAV_TYPE::MAV_TYPE_COAXIAL:
        case MAV_TYPE::MAV_TYPE_HELICOPTER:
        case MAV_TYPE::MAV_TYPE_HEXAROTOR:
        case MAV_TYPE::MAV_TYPE_OCTOROTOR:
        case MAV_TYPE::MAV_TYPE_TRICOPTER:
        case MAV_TYPE::MAV_TYPE_DODECAROTOR:
        case MAV_TYPE::MAV_TYPE_DECAROTOR: {
            const auto new_mode = flight_mode_to_ardupilot_copter_mode(flight_mode);
            if (new_mode == ardupilot::CopterMode::Unknown) {
                LogErr() << "Cannot translate flight mode to ArduPilot Copter mode.";
                MavlinkCommandSender::CommandLong empty_command{};
                return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
            } else {
                command.params.maybe_param2 = static_cast<float>(new_mode);
            }
            break;
        }

        default:
            LogErr() << "Cannot translate flight mode to ArduPilot mode, for MAV_TYPE: "
                     << _vehicle_type;
            MavlinkCommandSender::CommandLong empty_command{};
            return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
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
        case FlightMode::Offboard:
            return ardupilot::RoverMode::Guided;
        case FlightMode::Unknown:
        case FlightMode::Ready:
        case FlightMode::Takeoff:
        case FlightMode::Land:
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
            return ardupilot::CopterMode::Loiter;
        case FlightMode::ReturnToLaunch:
            return ardupilot::CopterMode::Rtl;
        case FlightMode::Land:
            return ardupilot::CopterMode::Land;
        case FlightMode::Manual:
            return ardupilot::CopterMode::Stabilize;
        case FlightMode::FollowMe:
            return ardupilot::CopterMode::Follow;
        case FlightMode::Offboard:
            return ardupilot::CopterMode::Guided;
        case FlightMode::Altctl:
            return ardupilot::CopterMode::AltHold;
        case FlightMode::Posctl:
            return ardupilot::CopterMode::PosHold;
        case FlightMode::Stabilized:
            return ardupilot::CopterMode::Stabilize;
        case FlightMode::Unknown:
        case FlightMode::Ready:
        case FlightMode::Takeoff:
        case FlightMode::Rattitude:
        default:
            return ardupilot::CopterMode::Unknown;
    }
}
ardupilot::PlaneMode SystemImpl::flight_mode_to_ardupilot_plane_mode(FlightMode flight_mode)
{
    switch (flight_mode) {
        case FlightMode::Mission:
            return ardupilot::PlaneMode::Auto;
        case FlightMode::Acro:
            return ardupilot::PlaneMode::Acro;
        case FlightMode::Hold:
            return ardupilot::PlaneMode::Loiter;
        case FlightMode::ReturnToLaunch:
            return ardupilot::PlaneMode::Rtl;
        case FlightMode::Manual:
            return ardupilot::PlaneMode::Manual;
        case FlightMode::FBWA:
            return ardupilot::PlaneMode::Fbwa;
        case FlightMode::Stabilized:
            return ardupilot::PlaneMode::Stabilize;
        case FlightMode::Unknown:
            return ardupilot::PlaneMode::Unknown;
        default:
            return ardupilot::PlaneMode::Unknown;
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
    MavlinkParameterClient::Result result, ParamValue value, const GetParamFloatCallback& callback)
{
    if (callback) {
        if (result == MavlinkParameterClient::Result::Success) {
            callback(result, value.get<float>());
        } else {
            callback(result, NAN);
        }
    }
}

void SystemImpl::receive_int_param(
    MavlinkParameterClient::Result result, ParamValue value, const GetParamIntCallback& callback)
{
    if (callback) {
        if (result == MavlinkParameterClient::Result::Success) {
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
    _mavsdk_impl.call_user_callback_located(filename, linenumber, func);
}

void SystemImpl::param_changed(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_param_changed_callbacks_mutex);

    for (auto& callback : _param_changed_callbacks) {
        callback.second(name);
    }
}

void SystemImpl::register_param_changed_handler(
    const ParamChangedCallback& callback, const void* cookie)
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

Time& SystemImpl::get_time()
{
    return _mavsdk_impl.time;
}

void SystemImpl::subscribe_param_float(
    const std::string& name,
    const MavlinkParameterClient::ParamFloatChangedCallback& callback,
    const void* cookie)
{
    param_sender(1, false)->subscribe_param_float_changed(name, callback, cookie);
}

void SystemImpl::subscribe_param_int(
    const std::string& name,
    const MavlinkParameterClient::ParamIntChangedCallback& callback,
    const void* cookie)
{
    param_sender(1, false)->subscribe_param_int_changed(name, callback, cookie);
}
void SystemImpl::subscribe_param_custom(
    const std::string& name,
    const MavlinkParameterClient::ParamCustomChangedCallback& callback,
    const void* cookie)
{
    param_sender(1, false)->subscribe_param_custom_changed(name, callback, cookie);
}

MavlinkParameterClient* SystemImpl::param_sender(uint8_t component_id, bool extended)
{
    std::lock_guard<std::mutex> lock(_mavlink_parameter_clients_mutex);

    for (auto& entry : _mavlink_parameter_clients) {
        if (entry.component_id == component_id && entry.extended == extended) {
            return entry.parameter_client.get();
        }
    }

    _mavlink_parameter_clients.push_back(
        {std::make_unique<MavlinkParameterClient>(
             *this,
             _mavsdk_impl.mavlink_message_handler,
             _mavsdk_impl.timeout_handler,
             [this]() { return timeout_s(); },
             component_id,
             extended),
         component_id,
         extended});

    return _mavlink_parameter_clients.back().parameter_client.get();
}

} // namespace mavsdk
