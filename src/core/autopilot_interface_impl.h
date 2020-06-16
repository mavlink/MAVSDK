#pragma once

#include "global_include.h"
#include "autopilot_interface.h"
#include "interface_base_impl.h"
#include "node_impl.h"

namespace mavsdk {

class AutopilotInterfaceImpl : public InterfaceBaseImpl {
public:
    enum class FlightMode {
        Unknown,
        Ready,
        Takeoff,
        Hold,
        Mission,
        ReturnToLaunch,
        Land,
        Offboard,
        FollowMe,
        Manual,
        Altctl,
        Posctl,
        Acro,
        Rattitude,
        Stabilized,
    };

    AutopilotInterfaceImpl(std::shared_ptr<NodeImpl> node_impl);

    typedef std::function<void(const mavlink_message_t)> mavlink_message_handler_t;

    void register_mavlink_message_handler(
            uint16_t msg_id, mavlink_message_handler_t callback, const void* cookie);
    void unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie);
    void unregister_all_mavlink_message_handlers(const void* cookie);

    void register_timeout_handler(std::function<void()> callback, double duration_s, void** cookie);
    void refresh_timeout_handler(const void* cookie);
    void unregister_timeout_handler(const void* cookie);

    void add_call_every(std::function<void()> callback, float interval_s, void** cookie);
    void change_call_every(float interval_s, void *cookie);
    void reset_call_every(void *cookie);
    void remove_call_every(void *cookie);

    uint8_t get_system_id();
    uint8_t get_component_id();

    uint8_t get_own_system_id();
    uint8_t get_own_component_id();

    bool send_message(mavlink_message_t& message);

    typedef std::function<void(MAVLinkCommands::Result, float)> commandResultCallback;

    MAVLinkCommands::Result send_command(MAVLinkCommands::CommandLong& command);
    MAVLinkCommands::Result send_command(MAVLinkCommands::CommandInt& command);

    void send_command_async(
            MAVLinkCommands::CommandLong& command, const commandResultCallback callback);
    void send_command_async(
            MAVLinkCommands::CommandInt& command, const commandResultCallback callback);

    MAVLinkParameters::Result set_param_float(const std::string& name, float value);
    MAVLinkParameters::Result set_param_int(const std::string& name, int32_t value);
    MAVLinkParameters::Result set_param_ext_float(const std::string& name, float value);
    MAVLinkParameters::Result set_param_ext_int(const std::string& name, int32_t value);

    typedef std::function<void(MAVLinkParameters::Result result)> success_t;
    void set_param_float_async(
            const std::string& name, float value, success_t callback, const void* cookie);
    void set_param_int_async(
            const std::string& name, int32_t value, success_t callback, const void* cookie);
    void set_param_ext_float_async(
            const std::string& name, float value, success_t callback, const void* cookie);
    void set_param_ext_int_async(
            const std::string& name, int32_t value, success_t callback, const void* cookie);

    typedef std::function<void(MAVLinkParameters::Result result, float value)>
        get_param_float_callback_t;
    typedef std::function<void(MAVLinkParameters::Result result, int32_t value)>
        get_param_int_callback_t;

    std::pair<MAVLinkParameters::Result, float> get_param_float(const std::string& name);
    std::pair<MAVLinkParameters::Result, int> get_param_int(const std::string& name);
    std::pair<MAVLinkParameters::Result, float> get_param_ext_float(const std::string& name);
    std::pair<MAVLinkParameters::Result, int> get_param_ext_int(const std::string& name);

    // These methods can be used to cache a parameter when an autopilot connects. For that
    // the callback can just be set to nullptr
    void get_param_float_async(
            const std::string& name, get_param_float_callback_t callback, const void* cookie);
    void get_param_int_async(
            const std::string& name, get_param_int_callback_t callback, const void* cookie);
    void get_param_ext_float_async(
            const std::string& name, get_param_float_callback_t callback, const void* cookie);
    void get_param_ext_int_async(
            const std::string& name, get_param_int_callback_t callback, const void* cookie);

    MAVLinkCommands::Result set_msg_rate(
            uint16_t message_id, double rate_hz);

    void set_msg_rate_async(
            uint16_t message_id,
            double rate_hz,
            commandResultCallback callback);

    void register_plugin(PluginImplBase* plugin_impl);
    void unregister_plugin(PluginImplBase* plugin_impl);

    void call_user_callback(const std::function<void()>& func);

    void intercept_incoming_messages(std::function<bool(mavlink_message_t&)> callback);
    void intercept_outgoing_messages(std::function<bool(mavlink_message_t&)> callback);

    bool is_connected();

    uint64_t get_uuid() const;

    bool is_armed() const { return _armed; }

    void send_autopilot_version_request();
    void send_flight_information_request();

    FlightMode get_flight_mode() const;

    MAVLinkCommands::Result set_flight_mode(FlightMode mode);
    void set_flight_mode_async(
            FlightMode mode,
            commandResultCallback callback);

    std::pair<MAVLinkCommands::Result, MAVLinkCommands::CommandLong>
        make_command_flight_mode(FlightMode mode);

    MAVLinkCommands::CommandLong
    make_command_msg_rate(uint16_t message_id, double rate_hz);
private:
    std::shared_ptr<NodeImpl> node_impl() { return _node_impl; };

    uint64_t _uuid;

    std::atomic<bool> _armed{false};
    std::atomic<bool> _hitl_enabled{false};

    void process_autopilot_version(const mavlink_message_t& message);
    void process_heartbeat(const mavlink_message_t& message);

    std::atomic<FlightMode> _flight_mode{FlightMode::Unknown};

    std::shared_ptr<NodeImpl> _node_impl;
};

} // namespace mavsdk
