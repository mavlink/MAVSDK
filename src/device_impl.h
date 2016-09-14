#pragma once

#include "global_include.h"
#include "mavlink_include.h"
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
    explicit DeviceImpl(DroneLinkImpl *parent);
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

    typedef void (*result_callback_t)(CommandResult result, void *user);

    struct ResultCallbackData {
        result_callback_t callback;
        void *user;
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

    CommandResult send_command(uint16_t command, const DeviceImpl::CommandParams &params);
    CommandResult send_command_with_ack(uint16_t command, const DeviceImpl::CommandParams &params);
    void send_command_with_ack_async(uint16_t command, const DeviceImpl::CommandParams &params,
                                     ResultCallbackData callback_data);

    double get_timeout() { return _timeout_s; }
    void set_timeout(double timeout_s) { _timeout_s = timeout_s; }

    void request_autopilot_version();

    uint64_t get_target_uuid() const;
    uint8_t get_target_system_id() const;
    uint8_t get_target_component_id() const;
    bool target_supports_mission_int() const { return _target_supports_mission_int; }

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

    static void device_thread(DeviceImpl *parent);
    static void send_heartbeat(DeviceImpl *parent);
    static void check_timeouts(DeviceImpl *parent);

    static void report_result(ResultCallbackData callback_data, CommandResult result);


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

    DroneLinkImpl *_parent;

    enum class CommandState : unsigned {
        NONE = 0,
        WAITING,
        RECEIVED
    };

    std::atomic<MAV_RESULT> _command_result;
    std::atomic<CommandState> _command_state;

    ResultCallbackData _result_callback_data;

    std::thread *_device_thread;
    std::atomic_bool _should_exit;

    static constexpr uint8_t _own_system_id = 0;
    static constexpr uint8_t _own_component_id = MAV_COMP_ID_SYSTEM_CONTROL;

    double _timeout_s;
    static constexpr double DEFAULT_TIMEOUT_S = 0.5;
};

} // namespace dronelink
