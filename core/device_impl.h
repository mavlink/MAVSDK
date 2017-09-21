#pragma once

#include "global_include.h"
#include "mavlink_include.h"
#include "mavlink_parameters.h"
#include "mavlink_commands.h"
#include "timeout_handler.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

namespace dronecore {

class DroneCoreImpl;


class DeviceImpl
{
public:
    explicit DeviceImpl(DroneCoreImpl *parent,
                        uint8_t target_system_id);
    ~DeviceImpl();

    void process_mavlink_message(const mavlink_message_t &message);

    typedef std::function<void(const mavlink_message_t &)> mavlink_message_handler_t;

    void register_mavlink_message_handler(uint16_t msg_id, mavlink_message_handler_t callback,
                                          const void *cookie);

    void unregister_all_mavlink_message_handlers(const void *cookie);

    void register_timeout_handler(std::function<void()> callback,
                                  double duration_s,
                                  void **cookie);

    void refresh_timeout_handler(const void *cookie);

    void update_timeout_handler(double new_duration_s, const void *cookie);

    void unregister_timeout_handler(const void *cookie);

    bool send_message(const mavlink_message_t &message);

    MavlinkCommands::Result send_command_with_ack(uint16_t command,
                                                  const MavlinkCommands::Params &params,
                                                  uint8_t component_id = 0);

    typedef std::function<void(MavlinkCommands::Result, float)> command_result_callback_t;
    void send_command_with_ack_async(uint16_t command, const MavlinkCommands::Params &params,
                                     command_result_callback_t callback,
                                     uint8_t component_id = 0);

    MavlinkCommands::Result set_msg_rate(uint16_t message_id, double rate_hz);

    void set_msg_rate_async(uint16_t message_id, double rate_hz,
                            command_result_callback_t callback);

    void request_autopilot_version();

    uint64_t get_target_uuid() const;
    uint8_t get_target_system_id() const;
    uint8_t get_target_component_id() const;

    void set_target_system_id(uint8_t system_id);

    bool target_supports_mission_int() const { return _target_supports_mission_int; }

    bool is_armed() const { return _armed; }

    typedef std::function <void(bool success)> success_t;
    void set_param_float_async(const std::string &name, float value, success_t callback);
    void set_param_int_async(const std::string &name, int32_t value, success_t callback);
    void set_param_ext_float_async(const std::string &name, float value, success_t callback);
    void set_param_ext_int_async(const std::string &name, int32_t value, success_t callback);

    typedef std::function <void(bool success, float value)> get_param_float_callback_t;
    typedef std::function <void(bool success, int32_t value)> get_param_int_callback_t;

    void get_param_float_async(const std::string &name, get_param_float_callback_t callback);
    void get_param_int_async(const std::string &name, get_param_int_callback_t callback);
    void get_param_ext_float_async(const std::string &name, get_param_float_callback_t callback);
    void get_param_ext_int_async(const std::string &name, get_param_int_callback_t callback);

    static uint8_t get_own_system_id() { return _own_system_id; }
    static uint8_t get_own_component_id() { return _own_component_id; }

    // Non-copyable
    DeviceImpl(const DeviceImpl &) = delete;
    const DeviceImpl &operator=(const DeviceImpl &) = delete;

private:

    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);

    static void device_thread(DeviceImpl *self);
    static void send_heartbeat(DeviceImpl *self);
    static void check_timeouts(DeviceImpl *self);
    static void check_heartbeat_timeout(DeviceImpl *self);

    static void receive_float_param(bool success, MavlinkParameters::ParamValue value,
                                    get_param_float_callback_t callback);
    static void receive_int_param(bool success, MavlinkParameters::ParamValue value,
                                  get_param_int_callback_t callback);

    struct MavlinkHandlerTableEntry {
        uint16_t msg_id;
        mavlink_message_handler_t callback;
        const void *cookie; // This is the identification to unregister.
    };

    std::mutex _mavlink_handler_table_mutex {};
    std::vector<MavlinkHandlerTableEntry> _mavlink_handler_table {};

    std::atomic<uint8_t> _target_system_id;

    // The component ID is hardcoded for now.
    uint8_t _target_component_id = MAV_COMP_ID_AUTOPILOT1;
    uint64_t _target_uuid {0};
    bool _target_supports_mission_int {false};
    bool _armed {false};

    DroneCoreImpl *_parent;

    command_result_callback_t _command_result_callback {nullptr};

    std::thread *_device_thread {nullptr};
    std::atomic_bool _should_exit {false};

    // TODO: should our own system ID have some value?
    static constexpr uint8_t _own_system_id = 0;
    static constexpr uint8_t _own_component_id = MAV_COMP_ID_SYSTEM_CONTROL;

    static std::mutex _last_heartbeat_reiceved_time_mutex;
    static dl_time_t _last_heartbeat_received_time;

    static constexpr double _HEARTBEAT_TIMEOUT_S = 3.0;

    std::atomic<bool> _heartbeats_arriving {false};

    static constexpr double _HEARTBEAT_SEND_INTERVAL_S = 1.0;

    MavlinkParameters _params;

    MavlinkCommands _commands;

    TimeoutHandler _timeout_handler {};
};


} // namespace dronecore
