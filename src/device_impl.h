#pragma once

#include "error_handling.h"
#include "mavlink_include.h"
#include "info.h"
#include "info_impl.h"
#include "telemetry.h"
#include "telemetry_impl.h"

#include <atomic>

namespace dronelink {

class DroneLinkImpl;

class DeviceImpl
{
public:
    explicit DeviceImpl(DroneLinkImpl *parent);
    ~DeviceImpl();
    void process_mavlink_message(const mavlink_message_t &message);

    Result arm();
    Result disarm();
    Result takeoff();
    Result land();

    const Info &get_info() const { return _info; }
    const Telemetry &get_telemetry() const { return _telemetry; }

    // Non-copyable
    DeviceImpl(const DeviceImpl &) = delete;
    const DeviceImpl &operator=(const DeviceImpl &) = delete;
private:
    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);
    void process_command_ack(const mavlink_message_t &message);
    void process_global_position_int(const mavlink_message_t &message);
    void process_extended_sys_state(const mavlink_message_t &message);
    void process_attitude_quaternion(const mavlink_message_t &message);
    void process_gps_raw_int(const mavlink_message_t &message);
    void process_home_position(const mavlink_message_t &message);
    void process_sys_status(const mavlink_message_t &message);

    Result send_command(const mavlink_message_t &message);
    Result send_command_with_ack(const mavlink_message_t &message);
    void try_to_initialize_autopilot_capabilites();

    uint8_t _system_id;
    uint8_t _component_id;

    Info _info;
    InfoImpl _info_impl;

    Telemetry _telemetry;
    TelemetryImpl _telemetry_impl;

    DroneLinkImpl *_parent;

    enum class CommandState : unsigned {
        NONE = 0,
        WAITING,
        RECEIVED
    };

    std::atomic<MAV_RESULT> _command_result;
    std::atomic<CommandState> _command_state;
};

} // namespace dronelink
