#pragma once

#include "global_include.h"
#include "mavlink_include.h"
#include "mavlink_parameters.h"
#include "mavlink_commands.h"
#include "timeout_handler.h"
#include "call_every_handler.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_set>
#include <map>
#include <thread>
#include <mutex>

namespace dronecore {

class DroneCoreImpl;
class PluginImplBase;


// This represents DroneCore client application which is a GCS.
struct GCSClient {
    static constexpr uint8_t system_id = 0;
    static constexpr uint8_t component_id = MAV_COMP_ID_SYSTEM_CONTROL;
    static constexpr MAV_TYPE type = MAV_TYPE_GCS;
};

// This class represents a MAVLink system, made up of one or more components
// (e.g. autopilot, cameras, servos, gimbals, etc). Commonly MAVLinkSystem objects are
// used to interact with UAVs (including their components) and standalone cameras.
class MAVLinkSystem
{
public:
    enum class FlightMode {
        HOLD = 0,
        RETURN_TO_LAUNCH,
        TAKEOFF,
        LAND,
        MISSION,
        FOLLOW_ME,
        OFFBOARD,
    };

    explicit MAVLinkSystem(DroneCoreImpl &parent,
                           uint8_t system_id, uint8_t component_id);
    ~MAVLinkSystem();

    void process_mavlink_message(const mavlink_message_t &message);

    typedef std::function<void(const mavlink_message_t &)> mavlink_message_handler_t;

    void register_mavlink_message_handler(uint16_t msg_id, mavlink_message_handler_t callback,
                                          const void *cookie);

    void unregister_all_mavlink_message_handlers(const void *cookie);

    void register_timeout_handler(std::function<void()> callback,
                                  double duration_s,
                                  void **cookie);
    void refresh_timeout_handler(const void *cookie);
    void unregister_timeout_handler(const void *cookie);

    void add_call_every(std::function<void()> callback, float interval_s, void **cookie);
    void change_call_every(float interval_s, const void *cookie);
    void reset_call_every(const void *cookie);
    void remove_call_every(const void *cookie);

    bool send_message(const mavlink_message_t &message,
                      uint8_t target_component_id = MAV_COMP_ID_ALL);

    MAVLinkCommands::Result send_command_with_ack(uint16_t command,
                                                  const MAVLinkCommands::Params &params,
                                                  uint8_t component_id = 0);

    typedef std::function<void(MAVLinkCommands::Result, float)> command_result_callback_t;
    void send_command_with_ack_async(uint16_t command, const MAVLinkCommands::Params &params,
                                     command_result_callback_t callback,
                                     uint8_t component_id = 0);

    MAVLinkCommands::Result set_msg_rate(uint16_t message_id, double rate_hz, uint8_t component_id = 0);

    void set_msg_rate_async(uint16_t message_id, double rate_hz,
                            command_result_callback_t callback, uint8_t component_id = 0);

    void request_autopilot_version();

    // Adds unique component ids
    void add_new_component(uint8_t component_id);
    size_t total_components() const;

    uint8_t get_autopilot_id() const;
    std::vector<uint8_t> get_camera_ids() const;
    uint8_t get_gimbal_id() const;

    bool is_standalone() const;
    bool is_autopilot() const;
    bool has_camera(uint8_t camera_id = 1) const;
    bool has_gimbal() const;

    uint64_t get_uuid() const;
    uint8_t get_system_id() const;

    void set_system_id(uint8_t system_id);

    bool does_support_mission_int() const { return _supports_mission_int; }

    bool is_armed() const { return _armed; }

    typedef std::function <void(bool success)> success_t;
    void set_param_float_async(const std::string &name, float value, success_t callback);
    void set_param_int_async(const std::string &name, int32_t value, success_t callback);
    void set_param_ext_float_async(const std::string &name, float value, success_t callback);
    void set_param_ext_int_async(const std::string &name, int32_t value, success_t callback);
    MAVLinkCommands::Result set_flight_mode(FlightMode mode);
    void set_flight_mode_async(FlightMode mode, command_result_callback_t callback);

    typedef std::function <void(bool success, float value)> get_param_float_callback_t;
    typedef std::function <void(bool success, int32_t value)> get_param_int_callback_t;

    void get_param_float_async(const std::string &name, get_param_float_callback_t callback);
    void get_param_int_async(const std::string &name, get_param_int_callback_t callback);
    void get_param_ext_float_async(const std::string &name, get_param_float_callback_t callback);
    void get_param_ext_int_async(const std::string &name, get_param_int_callback_t callback);

    bool is_connected() const;

    Time &get_time() { return _time; };

    void register_plugin(PluginImplBase *plugin_impl);
    void unregister_plugin(PluginImplBase *plugin_impl);

    // This allows a plugin to lock and unlock all mavlink communication.
    // The functionality is currently not used by a plugin included here
    // but nevertheless there for other plugins that can be added from external.
    void lock_communication();
    void unlock_communication();

    // Non-copyable
    MAVLinkSystem(const MAVLinkSystem &) = delete;
    const MAVLinkSystem &operator=(const MAVLinkSystem &) = delete;

private:

    // Helper methods added to increase readablity
    bool is_autopilot(uint8_t comp_id) const { return comp_id == MAV_COMP_ID_AUTOPILOT1; }
    bool have_uuid() const { return _uuid != 0 && _uuid_initialized; }

    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);
    void process_statustext(const mavlink_message_t &message);
    void heartbeats_timed_out();
    void set_connected();
    void set_disconnected();

    static std::string component_name(uint8_t component_id);

    static void system_thread(MAVLinkSystem *self);
    static void send_heartbeat(MAVLinkSystem &self);

    static void receive_float_param(bool success, MAVLinkParameters::ParamValue value,
                                    get_param_float_callback_t callback);
    static void receive_int_param(bool success, MAVLinkParameters::ParamValue value,
                                  get_param_int_callback_t callback);

    struct MAVLinkHandlerTableEntry {
        uint16_t msg_id;
        mavlink_message_handler_t callback;
        const void *cookie; // This is the identification to unregister.
    };

    std::mutex _mavlink_handler_table_mutex {};
    std::vector<MAVLinkHandlerTableEntry> _mavlink_handler_table {};

    std::atomic<uint8_t> _system_id;

    uint64_t _uuid {0};

    int _uuid_retries = 0;
    std::atomic<bool> _uuid_initialized {false};

    uint8_t _non_autopilot_heartbeats = 0;

    bool _supports_mission_int {false};
    std::atomic<bool> _armed {false};
    std::atomic<bool> _hitl_enabled {false};

    DroneCoreImpl &_parent;

    command_result_callback_t _command_result_callback {nullptr};

    std::thread *_system_thread {nullptr};
    std::atomic<bool> _should_exit {false};

    static constexpr double _HEARTBEAT_TIMEOUT_S = 3.0;

    std::mutex _connection_mutex {};
    bool _connected {false};
    void *_heartbeat_timeout_cookie = nullptr;

    std::atomic<bool> _autopilot_version_pending {false};
    void *_autopilot_version_timed_out_cookie = nullptr;

    static constexpr double _HEARTBEAT_SEND_INTERVAL_S = 1.0;

    MAVLinkParameters _params;

    MAVLinkCommands _commands;

    TimeoutHandler _timeout_handler;
    CallEveryHandler _call_every_handler;

    Time _time {};

    std::atomic<bool> _communication_locked {false};

    std::mutex _plugin_impls_mutex {};
    std::vector<PluginImplBase *> _plugin_impls {};

    // We used set to maintain unique component ids
    std::unordered_set<uint8_t> _components;
};


} // namespace dronecore
