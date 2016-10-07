#pragma once

#include "global_include.h"
#include "mavlink_include.h"
#include "mavlink_parameters.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

namespace dronelink {

class DroneLinkImpl;



class DeviceImpl
{
public:
    explicit DeviceImpl(DroneLinkImpl *parent,
                        uint8_t target_system_id,
                        uint8_t target_component_id);
    ~DeviceImpl();
    void process_mavlink_message(const mavlink_message_t &message);

    struct CommandParams {
        float v[7];
    };

    enum class CommandResult {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT
    };

    typedef std::function<void(const mavlink_message_t &)> mavlink_message_handler_t;

    typedef std::function<void()> timeout_handler_t;

    void register_mavlink_message_handler(uint8_t msg_id, mavlink_message_handler_t callback,
                                          const void *cookie);

    void unregister_all_mavlink_message_handlers(const void *cookie);

    void register_timeout_handler(timeout_handler_t callback, const void *cookie);

    void update_timeout_handler(const void *cookie);

    void unregister_timeout_handler(const void *cookie);

    bool send_message(const mavlink_message_t &message);

    CommandResult send_command(uint16_t command, const CommandParams &params,
                               uint8_t component_id = 0);
    CommandResult send_command_with_ack(uint16_t command, const CommandParams &params,
                                        uint8_t component_id = 0);

    typedef std::function<void(CommandResult)> command_result_callback_t;

    void send_command_with_ack_async(uint16_t command, const CommandParams &params,
                                     command_result_callback_t callback,
                                     uint8_t component_id = 0);


    CommandResult set_msg_rate(uint16_t message_id, double rate_hz);

    double get_timeout() { return _timeout_s; }
    void set_timeout(double timeout_s) { _timeout_s = timeout_s; }

    void request_autopilot_version();

    uint64_t get_target_uuid() const;
    uint8_t get_target_system_id() const;
    uint8_t get_target_component_id() const;

    bool target_supports_mission_int() const { return _target_supports_mission_int; }

    bool is_armed() const { return _armed; }

    typedef std::function <void(bool success)> success_t;
    void set_param_float_async(const std::string &name, float value, success_t callback);
    void set_param_int_async(const std::string &name, int32_t value, success_t callback);

    static uint8_t get_own_system_id() { return _own_system_id; }
    static uint8_t get_own_component_id() { return _own_component_id; }

    // Non-copyable
    DeviceImpl(const DeviceImpl &) = delete;
    const DeviceImpl &operator=(const DeviceImpl &) = delete;

private:

    void process_heartbeat(const mavlink_message_t &message);
    void process_command_ack(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);

    void check_device_thread();

    // TODO: completely remove, just for testing
    //static void get_sys_autostart(bool success, MavlinkParameters::ParamValue value);

    static void device_thread(DeviceImpl *self);
    static void send_heartbeat(DeviceImpl *self);
    static void check_timeouts(DeviceImpl *self);
    static void check_heartbeat_timeout(DeviceImpl *self);

    static void report_result(const command_result_callback_t &callback, CommandResult result);


    struct MavlinkHandlerTableEntry {
        uint8_t msg_id;
        mavlink_message_handler_t callback;
        const void *cookie; // This is the identification to unregister.
    };

    std::vector<MavlinkHandlerTableEntry> _mavlink_handler_table;

    struct TimeoutHandlerMapEntry {
        dl_time_t time;
        timeout_handler_t callback;
    };

    std::mutex _timeout_handler_map_mutex;
    std::map<const void *, TimeoutHandlerMapEntry> _timeout_handler_map;

    uint8_t _target_system_id;
    uint8_t _target_component_id;
    uint64_t _target_uuid;
    bool _target_supports_mission_int;
    bool _armed;

    DroneLinkImpl *_parent;

    enum class CommandState : unsigned {
        NONE = 0,
        WAITING,
        RECEIVED
    };

    std::atomic<MAV_RESULT> _command_result;
    std::atomic<CommandState> _command_state;

    command_result_callback_t _command_result_callback;

    std::thread *_device_thread;
    std::atomic_bool _should_exit;

    static constexpr uint8_t _own_system_id = 0;
    static constexpr uint8_t _own_component_id = MAV_COMP_ID_SYSTEM_CONTROL;

    double _timeout_s;
    static constexpr double DEFAULT_TIMEOUT_S = 0.5;

    dl_time_t _last_heartbeat_received_time;
    double _heartbeat_timeout_s;
    static constexpr double DEFAULT_HEARTBEAT_TIMEOUT_S = 3.0;
    bool _heartbeat_timed_out;

    MavlinkParameters _params;
};


} // namespace dronelink
