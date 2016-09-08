#pragma once

#include "error_handling.h"
#include "mavlink_include.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>

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

    typedef std::function<void(const mavlink_message_t &)> mavlink_message_handler_t;

    void register_mavlink_message_handler(uint8_t msg_id, mavlink_message_handler_t callback,
                                          const void *cookie);


    void unregister_all_mavlink_message_handlers(const void *cookie);

    Result send_command(uint16_t command, const DeviceImpl::CommandParams &params);
    Result send_command_with_ack(uint16_t command, const DeviceImpl::CommandParams &params);

    void request_autopilot_version();

    uint64_t get_uuid() const;

    // Non-copyable
    DeviceImpl(const DeviceImpl &) = delete;
    const DeviceImpl &operator=(const DeviceImpl &) = delete;

private:
    void process_heartbeat(const mavlink_message_t &message);
    void process_command_ack(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);


    struct HandlerTableEntry {
        uint8_t msg_id;
        mavlink_message_handler_t callback;
        const void *cookie; // This is the identification to unregister.
    };

    std::vector<HandlerTableEntry> _handler_table;

    uint8_t _system_id;
    uint8_t _component_id;
    uint64_t _uuid;

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
