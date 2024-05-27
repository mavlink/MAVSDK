#pragma once

#include "timeout_handler.h"
#include "mavlink_include.h"
#include "locked_queue.h"
#include "mavsdk_time.h"
#include <cmath>
#include <cstdint>
#include <string>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>

namespace mavsdk {

class SystemImpl;

class MavlinkCommandSender {
public:
    explicit MavlinkCommandSender(SystemImpl& system_impl);
    ~MavlinkCommandSender();

    enum class Result {
        Success = 0,
        NoSystem,
        ConnectionError,
        Busy,
        Denied,
        Unsupported,
        Timeout,
        InProgress,
        TemporarilyRejected,
        Failed,
        Cancelled,
        UnknownError
    };

    using CommandResultCallback = std::function<void(Result, float)>;

    struct CommandInt {
        uint8_t target_system_id{0};
        uint8_t target_component_id{0};
        MAV_FRAME frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
        uint16_t command{0};
        bool current = false;
        bool autocontinue = false;
        // Most of the "Reserved" values in MAVLink spec are NAN.
        struct Params {
            std::optional<float> maybe_param1{};
            std::optional<float> maybe_param2{};
            std::optional<float> maybe_param3{};
            std::optional<float> maybe_param4{};
            int32_t x = 0;
            int32_t y = 0;
            std::optional<float> maybe_z{};
        } params{};
    };

    struct CommandLong {
        uint8_t target_system_id{0};
        uint8_t target_component_id{0};
        uint16_t command{0};
        uint8_t confirmation = 0;
        struct Params {
            std::optional<float> maybe_param1{};
            std::optional<float> maybe_param2{};
            std::optional<float> maybe_param3{};
            std::optional<float> maybe_param4{};
            std::optional<float> maybe_param5{};
            std::optional<float> maybe_param6{};
            std::optional<float> maybe_param7{};
        } params{};
    };

    Result send_command(const CommandInt& command);
    Result send_command(const CommandLong& command);

    void queue_command_async(const CommandInt& command, const CommandResultCallback& callback);
    void queue_command_async(const CommandLong& command, const CommandResultCallback& callback);

    void do_work();

    static const int DEFAULT_COMPONENT_ID_AUTOPILOT = MAV_COMP_ID_AUTOPILOT1;

    // Non-copyable
    MavlinkCommandSender(const MavlinkCommandSender&) = delete;
    const MavlinkCommandSender& operator=(const MavlinkCommandSender&) = delete;

private:
    // The std::monostate is required to work around the fact that
    // the default ctor of CommandLong and CommandInt is ill-defined.
    using Command = std::variant<std::monostate, CommandLong, CommandInt>;

    struct CommandIdentification {
        uint32_t maybe_param1{0}; // only for commands where this matters
        uint32_t maybe_param2{0}; // only for commands where this matters
        uint16_t command{0};
        uint8_t target_system_id{0};
        uint8_t target_component_id{0};

        bool operator==(const CommandIdentification& other) const
        {
            return maybe_param1 == other.maybe_param1 && maybe_param2 == other.maybe_param2 &&
                   command == other.command && target_system_id == other.target_system_id &&
                   target_component_id == other.target_component_id;
        }

        bool operator!=(const CommandIdentification& other) const { return !(*this == other); }
    };

    struct Work {
        Command command;
        CommandIdentification identification{};
        CommandResultCallback callback{};
        SteadyTimePoint time_started{};
        TimeoutHandler::Cookie timeout_cookie{};
        double timeout_s{0.5};
        int retries_to_do{3};
        bool already_sent{false};
    };

    template<typename CommandType>
    CommandIdentification identification_from_command(const CommandType& command)
    {
        CommandIdentification identification{};

        identification.command = command.command;
        if (command.command == MAV_CMD_REQUEST_MESSAGE ||
            command.command == MAV_CMD_SET_MESSAGE_INTERVAL) {
            if (command.params.maybe_param1) {
                const uint32_t param1 =
                    static_cast<uint32_t>(std::lround(command.params.maybe_param1.value()));
                identification.maybe_param1 = param1;
                if (param1 == MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED) {
                    if (command.params.maybe_param2) {
                        identification.maybe_param2 =
                            static_cast<uint32_t>(std::lround(command.params.maybe_param2.value()));
                    }
                }
            }
        }
        identification.target_system_id = command.target_system_id;
        identification.target_component_id = command.target_component_id;

        return identification;
    }

    void receive_command_ack(mavlink_message_t message);
    void receive_timeout(const CommandIdentification& identification);

    void call_callback(const CommandResultCallback& callback, Result result, float progress);

    bool send_mavlink_message(const Command& command);

    float maybe_reserved(const std::optional<float>& maybe_param) const;

    SystemImpl& _system_impl;
    LockedQueue<Work> _work_queue{};

    bool _command_debugging{false};
};

} // namespace mavsdk
