#pragma once

#include "mavlink_include.h"
#include "locked_queue.h"
#include <cstdint>
#include <string>
#include <functional>
#include <mutex>

namespace dronecore {

class MAVLinkSystem;

class MAVLinkCommands
{
public:
    explicit MAVLinkCommands(MAVLinkSystem &parent);
    ~MAVLinkCommands();

    enum class Result {
        SUCCESS = 0,
        NO_SYSTEM,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        IN_PROGRESS,
        ERROR
    };

    typedef std::function<void(Result, float)> command_result_callback_t;

    struct Params {
        float v[7];
    };

    Result send_command(uint16_t command,
                        const Params params,
                        uint8_t target_system_id,
                        uint8_t target_component_id);

    void queue_command_async(uint16_t command,
                             const Params params,
                             uint8_t target_system_id,
                             uint8_t target_component_id,
                             command_result_callback_t callback);

    void do_work();

    static const int DEFAULT_COMPONENT_ID_AUTOPILOT = MAV_COMP_ID_AUTOPILOT1;

    // Non-copyable
    MAVLinkCommands(const MAVLinkCommands &) = delete;
    const MAVLinkCommands &operator=(const MAVLinkCommands &) = delete;

private:
    enum class State {
        NONE,
        WAITING,
        IN_PROGRESS,
        DONE,
        FAILED
    } _state = State::NONE;
    std::mutex _state_mutex {};

    struct Work {
        int retries_to_do = 3;
        double timeout_s = 0.5;
        uint16_t mavlink_command = 0;
        mavlink_message_t mavlink_message {};
        command_result_callback_t callback {};
    };

    void receive_command_ack(mavlink_message_t message);
    void receive_timeout();

    MAVLinkSystem &_parent;
    LockedQueue<Work> _work_queue {};

    void *_timeout_cookie = nullptr;
};

} // namespace dronecore
