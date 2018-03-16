#include "system.h"
#include "global_include.h"
#include "dronecore_impl.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <functional>
#include <algorithm>
#include "px4_custom_mode.h"

// Set to 1 to log incoming/outgoing mavlink messages.
#define MESSAGE_DEBUGGING 0

namespace dronecore {

using namespace std::placeholders; // for `_1`

System::System(DroneCoreImpl &parent,
               uint8_t system_id) :
    _system_id(system_id),
    _parent(parent),
    _params(*this),
    _commands(*this),
    _timeout_handler(_time),
    _call_every_handler(_time)
{
    _system_thread = new std::thread(system_thread, this);

    register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&System::process_heartbeat, this, _1), this);

    register_mavlink_message_handler(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&System::process_autopilot_version, this, _1), this);

    register_mavlink_message_handler(
        MAVLINK_MSG_ID_STATUSTEXT,
        std::bind(&System::process_statustext, this, _1), this);
}

System::~System()
{
    _should_exit = true;
    unregister_all_mavlink_message_handlers(this);

    unregister_timeout_handler(_autopilot_version_timed_out_cookie);
    unregister_timeout_handler(_heartbeat_timeout_cookie);

    if (_system_thread != nullptr) {
        _system_thread->join();
        delete _system_thread;
        _system_thread = nullptr;
    }
}

bool System::is_connected() const
{
    return _connected;
}

void System::register_mavlink_message_handler(uint16_t msg_id,
                                              mavlink_message_handler_t callback,
                                              const void *cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_handler_table_mutex);

    MavlinkHandlerTableEntry entry = {msg_id, callback, cookie};
    _mavlink_handler_table.push_back(entry);
}

void System::unregister_all_mavlink_message_handlers(const void *cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_handler_table_mutex);

    for (auto it = _mavlink_handler_table.begin();
         it != _mavlink_handler_table.end();
         /* no ++it */) {

        if (it->cookie == cookie) {
            it = _mavlink_handler_table.erase(it);
        } else {
            ++it;
        }
    }
    _mavlink_handler_table.clear();
}

void System::register_timeout_handler(std::function<void()> callback,
                                      double duration_s,
                                      void **cookie)
{
    _timeout_handler.add(callback, duration_s, cookie);
}

void System::refresh_timeout_handler(const void *cookie)
{
    _timeout_handler.refresh(cookie);
}

void System::unregister_timeout_handler(const void *cookie)
{
    _timeout_handler.remove(cookie);
}

void System::process_mavlink_message(const mavlink_message_t &message)
{
    if (_communication_locked) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mavlink_handler_table_mutex);

#if MESSAGE_DEBUGGING==1
    bool forwarded = false;
#endif
    for (auto it = _mavlink_handler_table.begin(); it != _mavlink_handler_table.end(); ++it) {
        if (it->msg_id == message.msgid) {
#if MESSAGE_DEBUGGING==1
            LogDebug() << "Forwarding msg " << int(message.msgid) << " to " << size_t(it->cookie);
            forwarded = true;
#endif
            it->callback(message);
        }
    }
#if MESSAGE_DEBUGGING==1
    if (!forwarded) {
        LogDebug() << "Ignoring msg " << int(message.msgid);
    }
#endif
}

void System::add_call_every(std::function<void()> callback, float interval_s, void **cookie)
{
    _call_every_handler.add(callback, interval_s, cookie);
}

void System::change_call_every(float interval_s, const void *cookie)
{
    _call_every_handler.change(interval_s, cookie);
}

void System::reset_call_every(const void *cookie)
{
    _call_every_handler.reset(cookie);
}

void System::remove_call_every(const void *cookie)
{
    _call_every_handler.remove(cookie);
}

void System::process_heartbeat(const mavlink_message_t &message)
{
    LogDebug() << "Got HEARTBEAT from: " << component_name(message.compid);
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    if (message.compid == MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT) {
        _armed = ((heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false);
        _hitl_enabled = ((heartbeat.base_mode & MAV_MODE_FLAG_HIL_ENABLED) ? true : false);
    }

    // We do not call on_discovery here but wait with the notification until we know the UUID.

    /* If the component is an autopilot and
     * we don't know its UUID, then try to find out. */
    if (message.compid == MAV_COMP_ID_AUTOPILOT1
        && _uuid == 0 && !_uuid_initialized) {
        request_autopilot_version();
    }

    set_connected();
}

void System::process_autopilot_version(const mavlink_message_t &message)
{
    // Ignore if they don't come from the autopilot component
    if (message.compid != MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT) {
        return;
    }

    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    _supports_mission_int =
        ((autopilot_version.capabilities & MAV_PROTOCOL_CAPABILITY_MISSION_INT) ? true : false);

    if (_uuid == 0 && autopilot_version.uid != 0) {

        // This is the best case. The system has a UUID and we were able to get it.
        _uuid = autopilot_version.uid;

    } else if (_uuid == 0 && autopilot_version.uid == 0) {

        // This is not ideal because the system has no valid UUID.
        // In this case we use the mavlink system ID as the UUID.
        _uuid = _system_id;

    } else if (_uuid != autopilot_version.uid) {

        // TODO: this is bad, we should raise a flag to invalidate system.
        LogErr() << "Error: UUID changed";
    }

    _uuid_initialized = true;
    set_connected();

    _autopilot_version_pending = false;
    unregister_timeout_handler(_autopilot_version_timed_out_cookie);
}

void System::process_statustext(const mavlink_message_t &message)
{
    mavlink_statustext_t statustext;
    mavlink_msg_statustext_decode(&message, &statustext);

    std::string debug_str = "MAVLink: ";

    switch (statustext.severity) {
        case MAV_SEVERITY_EMERGENCY:
            debug_str += "emergency";
            break;
        case MAV_SEVERITY_ALERT:
            debug_str += "alert";
            break;
        case MAV_SEVERITY_CRITICAL:
            debug_str += "critical";
            break;
        case MAV_SEVERITY_ERROR:
            debug_str += "error";
            break;
        case MAV_SEVERITY_WARNING:
            debug_str += "warning";
            break;
        case MAV_SEVERITY_NOTICE:
            debug_str += "notice";
            break;
        case MAV_SEVERITY_INFO:
            debug_str += "info";
            break;
        case MAV_SEVERITY_DEBUG:
            debug_str += "debug";
            break;
        default:
            break;
    }

    // statustext.text is not null terminated, therefore we copy it first to
    // an array big enough that is zeroed.
    char text_with_null[sizeof(statustext.text) + 1] {};
    memcpy(text_with_null, statustext.text, sizeof(statustext.text));

    LogDebug() << debug_str << ": " << text_with_null;
}

void System::heartbeats_timed_out()
{
    LogInfo() << "heartbeats timed out";
    set_disconnected();
}

void System::system_thread(System *self)
{
    dl_time_t last_time {};

    while (!self->_should_exit) {

        if (self->_time.elapsed_since_s(last_time) >= System::_HEARTBEAT_SEND_INTERVAL_S) {
            send_heartbeat(*self);
            last_time = self->_time.steady_time();
        }

        self->_call_every_handler.run_once();
        self->_timeout_handler.run_once();
        self->_params.do_work();
        self->_commands.do_work();

        if (self->_connected) {
            // Work fairly fast if we're connected.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            // Be less aggressive when unconnected.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

std::string System::component_name(uint8_t component_id)
{
    switch (component_id) {
        case MAV_COMP_ID_AUTOPILOT1:
            return "Autopilot";
        case MAV_COMP_ID_CAMERA...MAV_COMP_ID_CAMERA6:
            return "Camera";
        case MAV_COMP_ID_GIMBAL:
            return "Gimbal";
        default:
            return "Unsupported component";
    }
}

void System::add_new_component(uint8_t component_id)
{
    auto res_pair = _components.insert(component_id);
    if (res_pair.second) {
        LogDebug() << "Component " << component_name(component_id) << " added.";
    }
}

size_t System::total_components() const
{
    return _components.size();
}

bool System::is_standalone() const
{
    return !is_autopilot();
}

bool System::is_autopilot() const
{
    return get_autopilot_id() != uint8_t(0);
}

bool System::has_camera(uint8_t camera_id) const
{
    auto camera_ids = get_camera_ids();

    switch (camera_id) {
        case 1:
            for (auto cam_id : camera_ids) {
                return (cam_id == MAV_COMP_ID_CAMERA);
            }
        case 2:
            for (auto cam_id : camera_ids) {
                return (cam_id == MAV_COMP_ID_CAMERA2);
            }
        case 3:
            for (auto cam_id : camera_ids) {
                return (cam_id == MAV_COMP_ID_CAMERA3);
            }
        case 4:
            for (auto cam_id : camera_ids) {
                return (cam_id == MAV_COMP_ID_CAMERA4);
            }
        case 5:
            for (auto cam_id : camera_ids) {
                return (cam_id == MAV_COMP_ID_CAMERA5);
            }
        case 6:
            for (auto cam_id : camera_ids) {
                return (cam_id == MAV_COMP_ID_CAMERA6);
            }
        default:
            return false;
    }
}

bool System::has_gimbal() const
{
    return get_gimbal_id() == MAV_COMP_ID_GIMBAL;
}

void System::send_heartbeat(System &self)
{
    mavlink_message_t message;
    // Self is GCS (its not autopilot!); hence MAV_AUTOPILOT_INVALID.
    mavlink_msg_heartbeat_pack(_own_system_id, _own_component_id, &message,
                               MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, 0);
    self.send_message(message);
}

bool System::send_message(const mavlink_message_t &message)
{
    if (_communication_locked) {
        return false;
    }

#if MESSAGE_DEBUGGING==1
    LogDebug() << "Sending msg " << size_t(message.msgid);
#endif
    return _parent.send_message(message);
}

void System::request_autopilot_version()
{
    if (_uuid_initialized) {
        // Already initialized, we can exit.
        return;
    }

    if (!_autopilot_version_pending && _uuid_retries >= 3) {
        // We give up getting a UUID and use the system ID.

        LogWarn() << "No UUID received, using system ID instead.";
        _uuid = _system_id;
        _uuid_initialized = true;
        set_connected();
        return;
    }

    _autopilot_version_pending = true;

    // We don't care about an answer, we mostly care about receiving AUTOPILOT_VERSION.
    send_command_with_ack_async(
        MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
        MavlinkCommands::Params {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
        nullptr,
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT
    );
    ++_uuid_retries;

    // We set a timeout to stay "pending" for half a second. This way, we don't give up too
    // early e.g. because multiple components send heartbeats and we receive them all at once
    // and run out of retries.

    // We create a temp reference, so we don't need to capture `this`.
    auto &pending_tmp = _autopilot_version_pending;
    register_timeout_handler(
    [&pending_tmp]() {
        pending_tmp = false;
    },
    0.5,
    &_autopilot_version_timed_out_cookie);
}

void System::set_connected()
{
    bool enable_needed = false;
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);

        if (!_connected && _uuid_initialized) {

            LogDebug() << "# of components: " << _components.size();

            _parent.notify_on_discover(_uuid);
            _connected = true;

            register_timeout_handler(std::bind(&System::heartbeats_timed_out, this),
                                     _HEARTBEAT_TIMEOUT_S,
                                     &_heartbeat_timeout_cookie);
            enable_needed = true;

        } else if (_connected) {
            refresh_timeout_handler(_heartbeat_timeout_cookie);
        }
        // If not yet connected there is nothing to do/
    }
    if (enable_needed) {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        for (auto plugin_impl : _plugin_impls) {
            plugin_impl->enable();
        }
    }
}

void System::set_disconnected()
{
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);

        // This might not be needed because this is probably called from the triggered
        // timeout anyway but it should also do no harm.
        //unregister_timeout_handler(_heartbeat_timeout_cookie);
        //_heartbeat_timeout_cookie = nullptr;

        _connected = false;
        _parent.notify_on_timeout(_uuid);
    }

    {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        for (auto plugin_impl : _plugin_impls) {
            plugin_impl->disable();
        }
    }
}

uint64_t System::get_uuid() const
{
    // We want to support UUIDs if the autopilot tells us.
    return _uuid;
}

uint8_t System::get_system_id() const
{
    return _system_id;
}

#if 0
uint8_t System::get_component_id() const
{
    return _component_id;
}
#endif

void System::set_system_id(uint8_t system_id)
{
    _system_id = system_id;
}

void System::set_param_float_async(const std::string &name, float value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_float(value);
    _params.set_param_async(name, param_value, callback);
}

void System::set_param_int_async(const std::string &name, int32_t value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_int(value);
    _params.set_param_async(name, param_value, callback);
}

void System::set_param_ext_float_async(const std::string &name, float value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_float(value);
    _params.set_param_async(name, param_value, callback, true);
}

void System::set_param_ext_int_async(const std::string &name, int32_t value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_int(value);
    _params.set_param_async(name, param_value, callback, true);
}

void System::get_param_float_async(const std::string &name,
                                   get_param_float_callback_t callback)
{
    _params.get_param_async(name, std::bind(&System::receive_float_param, _1, _2,
                                            callback));
}

MavlinkCommands::Result System::set_flight_mode(FlightMode system_mode)
{
    const uint8_t flag_safety_armed = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;

    const uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED
                         | flag_safety_armed
                         | flag_hitl_enabled;

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = 0;

    switch (system_mode) {
        case FlightMode::HOLD:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case FlightMode::RETURN_TO_LAUNCH:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case FlightMode::TAKEOFF:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case FlightMode::LAND:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case FlightMode::MISSION:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case FlightMode::FOLLOW_ME:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case FlightMode::OFFBOARD:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            break;
        default :
            LogErr() << "Unknown Flight mode.";
            return MavlinkCommands::Result::ERROR;

    }

    MavlinkCommands::Result ret = send_command_with_ack(
                                      MAV_CMD_DO_SET_MODE,
                                      MavlinkCommands::Params {float(mode),
                                                               float(custom_mode),
                                                               float(custom_sub_mode),
                                                               NAN, NAN, NAN, NAN},
                                      MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);

    return ret;
}

void System::set_flight_mode_async(FlightMode system_mode, command_result_callback_t callback)
{
    const uint8_t flag_safety_armed = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;

    const uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED
                         | flag_safety_armed
                         | flag_hitl_enabled;

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = 0;

    switch (system_mode) {
        case FlightMode::HOLD:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case FlightMode::RETURN_TO_LAUNCH:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case FlightMode::TAKEOFF:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case FlightMode::LAND:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case FlightMode::MISSION:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case FlightMode::FOLLOW_ME:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case FlightMode::OFFBOARD:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            break;
        default :
            LogErr() << "Unknown flight mode.";
            if (callback) {
                callback(MavlinkCommands::Result::ERROR, NAN);
            }
            return ;
    }


    send_command_with_ack_async(MAV_CMD_DO_SET_MODE,
                                MavlinkCommands::Params {float(mode),
                                                         float(custom_mode),
                                                         float(custom_sub_mode),
                                                         NAN, NAN, NAN, NAN},
                                callback,
                                MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void System::get_param_int_async(const std::string &name,
                                 get_param_int_callback_t callback)
{
    _params.get_param_async(name, std::bind(&System::receive_int_param, _1, _2,
                                            callback));
}

void System::get_param_ext_float_async(const std::string &name,
                                       get_param_float_callback_t callback)
{
    _params.get_param_async(name, std::bind(&System::receive_float_param, _1, _2,
                                            callback), true);
}

void System::get_param_ext_int_async(const std::string &name,
                                     get_param_int_callback_t callback)
{
    _params.get_param_async(name, std::bind(&System::receive_int_param, _1, _2,
                                            callback), true);
}

void System::receive_float_param(bool success, MavlinkParameters::ParamValue value,
                                 get_param_float_callback_t callback)
{
    if (callback) {
        callback(success, value.get_float());
    }
}

void System::receive_int_param(bool success, MavlinkParameters::ParamValue value,
                               get_param_int_callback_t callback)
{
    if (callback) {
        callback(success, value.get_int());
    }
}

uint8_t System::get_autopilot_id() const
{
    for (auto compid : _components)
        if (compid == MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT) {
            return compid;
        }
    // FIXME: Not sure what should be returned if autopilot is not found
    return uint8_t(0);
}

std::vector<uint8_t> System::get_camera_ids() const
{
    std::vector<uint8_t> camera_ids;
    camera_ids.clear();

    for (auto compid : _components)
        if (compid >= MAV_COMP_ID_CAMERA && compid <= MAV_COMP_ID_CAMERA6) {
            camera_ids.push_back(compid);
        }
    return camera_ids;
}

uint8_t System::get_gimbal_id() const
{
    for (auto compid : _components)
        if (compid == MAV_COMP_ID_GIMBAL) {
            return compid;
        }
    return uint8_t(0);
}

MavlinkCommands::Result System::send_command_with_ack(
    uint16_t command, const MavlinkCommands::Params &params, uint8_t component_id)
{
    if (_system_id == 0 && _components.size() == 0) {
        return MavlinkCommands::Result::NO_SYSTEM;
    }

    const uint8_t component_id_to_use =
        ((component_id != 0) ? component_id : get_autopilot_id());

    return _commands.send_command(command, params, _system_id, component_id_to_use);
}

void System::send_command_with_ack_async(uint16_t command,
                                         const MavlinkCommands::Params &params,
                                         command_result_callback_t callback,
                                         uint8_t component_id)
{
    if (_system_id == 0 && _components.size() == 0) {
        if (callback) {
            callback(MavlinkCommands::Result::NO_SYSTEM, NAN);
        }
        return;
    }

    const uint8_t component_id_to_use =
        ((component_id != 0) ? component_id : _components.size() == 0);

    _commands.queue_command_async(command, params, _system_id, component_id_to_use,
                                  callback);
}

MavlinkCommands::Result System::set_msg_rate(uint16_t message_id, double rate_hz,
                                             uint8_t component_id)
{
    // If left at -1 it will stop the message stream.
    float interval_us = -1.0f;
    if (rate_hz > 0) {
        interval_us = 1e6f / static_cast<float>(rate_hz);
    }

    if (component_id != 0) {
        return send_command_with_ack(
                   MAV_CMD_SET_MESSAGE_INTERVAL,
                   MavlinkCommands::Params {float(message_id), interval_us, NAN, NAN, NAN, NAN, NAN},
                   component_id);
    } else {
        return send_command_with_ack(
                   MAV_CMD_SET_MESSAGE_INTERVAL,
                   MavlinkCommands::Params {float(message_id), interval_us, NAN, NAN, NAN, NAN, NAN});
    }
}

void System::set_msg_rate_async(uint16_t message_id, double rate_hz,
                                command_result_callback_t callback, uint8_t component_id)
{
    // If left at -1 it will stop the message stream.
    float interval_us = -1.0f;
    if (rate_hz > 0) {
        interval_us = 1e6f / static_cast<float>(rate_hz);
    }

    if (component_id != 0) {
        send_command_with_ack_async(
            MAV_CMD_SET_MESSAGE_INTERVAL,
            MavlinkCommands::Params {float(message_id), interval_us, NAN, NAN, NAN, NAN, NAN},
            callback,
            component_id);
    } else {
        send_command_with_ack_async(
            MAV_CMD_SET_MESSAGE_INTERVAL,
            MavlinkCommands::Params {float(message_id), interval_us, NAN, NAN, NAN, NAN, NAN},
            callback);
    }
}

void System::register_plugin(PluginImplBase *plugin_impl)
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

void System::unregister_plugin(PluginImplBase *plugin_impl)
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

void System::lock_communication()
{
    _communication_locked = true;
}

void System::unlock_communication()
{
    _communication_locked = false;
}


} // namespace dronecore
