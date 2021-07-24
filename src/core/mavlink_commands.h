#pragma once

#include "mavlink_include.h"
#include "locked_queue.h"
#include "global_include.h"
#include <cstdint>
#include <string>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>

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
        CommandDenied,
        Unsupported,
        Timeout,
        InProgress,
        UnknownError
    };

    using CommandResultCallback = std::function<void(Result, float)>;

    struct CommandInt {
        uint8_t target_system_id{0};
        uint8_t target_component_id{0};
        MAV_FRAME frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
        uint16_t command{0};
        bool current = 0;
        bool autocontinue = false;
        // Most of the "Reserved" values in MAVLink spec are NAN.
        struct Params {
            float param1 = NAN;
            float param2 = NAN;
            float param3 = NAN;
            float param4 = NAN;
            int32_t x = 0;
            int32_t y = 0;
            float z = NAN;
        } params{};

        // In some cases "Reserved" value could be "0".
        // This utility method can be used in such case.
        // TODO: rename to set_all
        static void set_as_reserved(Params& params, float reserved_value = NAN)
        {
            params.param1 = reserved_value;
            params.param2 = reserved_value;
            params.param3 = reserved_value;
            params.param4 = reserved_value;
            params.x = 0;
            params.y = 0;
            params.z = reserved_value;
        }
    };

    struct CommandLong {
        CommandLong() = delete;
        explicit CommandLong(const SystemImpl& system_impl);

        uint8_t target_system_id{0};
        uint8_t target_component_id{0};
        uint16_t command{0};
        uint8_t confirmation = 0;
        struct Params {
            float param1;
            float param2;
            float param3;
            float param4;
            float param5;
            float param6;
            float param7;
        } params;

        // TODO: rename to set_all
        static void set_as_reserved(Params& params, float reserved_value = NAN)
        {
            params.param1 = reserved_value;
            params.param2 = reserved_value;
            params.param3 = reserved_value;
            params.param4 = reserved_value;
            params.param5 = reserved_value;
            params.param6 = reserved_value;
            params.param7 = reserved_value;
        }
    };

    Result send_command(const CommandInt& command);
    Result send_command(const CommandLong& command);

    void queue_command_async(const CommandInt& command, CommandResultCallback callback);
    void queue_command_async(const CommandLong& command, CommandResultCallback callback);

    void do_work();

    static const int DEFAULT_COMPONENT_ID_AUTOPILOT = MAV_COMP_ID_AUTOPILOT1;

    // Non-copyable
    MavlinkCommandSender(const MavlinkCommandSender&) = delete;
    const MavlinkCommandSender& operator=(const MavlinkCommandSender&) = delete;

private:
    struct Work {
        int retries_to_do{3};
        double timeout_s{0.5};
        uint16_t mavlink_command{0};
        bool already_sent{false};
        mavlink_message_t mavlink_message{};
        CommandResultCallback callback{};
        dl_time_t time_started{};
        void* timeout_cookie = nullptr;

        explicit Work(double new_timeout_s) : timeout_s(new_timeout_s) {}
    };

    void receive_command_ack(mavlink_message_t message);
    void receive_timeout(const uint16_t command);

    void call_callback(const CommandResultCallback& callback, Result result, float progress);

    SystemImpl& _parent;
    LockedQueue<Work> _work_queue{};
    std::unordered_map<uint16_t, std::shared_ptr<Work>> _sent_commands{};
    std::mutex _sent_commands_mutex{};
};

class MavlinkCommandReceiver {
public:
    explicit MavlinkCommandReceiver(SystemImpl& system_impl);
    ~MavlinkCommandReceiver();

    struct CommandInt {
        uint8_t target_system_id{0};
        uint8_t target_component_id{0};
        uint8_t origin_system_id{0};
        uint8_t origin_component_id{0};
        MAV_FRAME frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
        uint16_t command{0};
        bool current{0};
        bool autocontinue{false};
        // Most of the "Reserved" values in MAVLink spec are NAN.
        struct Params {
            float param1 = NAN;
            float param2 = NAN;
            float param3 = NAN;
            float param4 = NAN;
            int32_t x = 0;
            int32_t y = 0;
            float z = NAN;
        } params{};

        CommandInt(const mavlink_message_t& message)
        {
            mavlink_command_int_t command_int;
            mavlink_msg_command_int_decode(&message, &command_int);

            target_system_id = command_int.target_system;
            target_component_id = command_int.target_component;
            origin_system_id = message.sysid;
            origin_component_id = message.compid;
            command = command_int.command;
            frame = static_cast<MAV_FRAME>(command_int.frame);
            current = command_int.current;
            autocontinue = command_int.autocontinue;
            params.param1 = command_int.param1;
            params.param2 = command_int.param2;
            params.param3 = command_int.param3;
            params.param4 = command_int.param4;
            params.x = command_int.x;
            params.y = command_int.y;
            params.z = command_int.z;
        }
    };

    struct CommandLong {
        uint8_t target_system_id{0};
        uint8_t target_component_id{0};
        uint8_t origin_system_id{0};
        uint8_t origin_component_id{0};
        uint16_t command{0};
        uint8_t confirmation{0};
        struct Params {
            float param1 = NAN;
            float param2 = NAN;
            float param3 = NAN;
            float param4 = NAN;
            float param5 = NAN;
            float param6 = NAN;
            float param7 = NAN;
        } params{};

        CommandLong(const mavlink_message_t& message)
        {
            mavlink_command_long_t command_long;
            mavlink_msg_command_long_decode(&message, &command_long);

            target_system_id = command_long.target_system;
            target_component_id = command_long.target_component;
            origin_system_id = message.sysid;
            origin_component_id = message.compid;
            command = command_long.command;
            confirmation = command_long.confirmation;
            params.param1 = command_long.param1;
            params.param2 = command_long.param2;
            params.param3 = command_long.param3;
            params.param4 = command_long.param4;
            params.param5 = command_long.param5;
            params.param6 = command_long.param6;
            params.param7 = command_long.param7;
        }
    };

    using MavlinkCommandIntHandler =
        std::function<std::optional<mavlink_message_t>(const CommandInt&)>;
    using MavlinkCommandLongHandler =
        std::function<std::optional<mavlink_message_t>(const CommandLong&)>;

    void register_mavlink_command_handler(
        uint16_t cmd_id, MavlinkCommandIntHandler callback, const void* cookie);
    void register_mavlink_command_handler(
        uint16_t cmd_id, MavlinkCommandLongHandler callback, const void* cookie);

    void unregister_mavlink_command_handler(uint16_t cmd_id, const void* cookie);
    void unregister_all_mavlink_command_handlers(const void* cookie);

private:
    SystemImpl& _parent;

    void receive_command_int(const mavlink_message_t& message);
    void receive_command_long(const mavlink_message_t& message);

    struct MAVLinkCommandIntHandlerTableEntry {
        uint16_t cmd_id;
        MavlinkCommandIntHandler callback;
        const void* cookie; // This is the identification to unregister.
    };

    struct MAVLinkCommandLongHandlerTableEntry {
        uint16_t cmd_id;
        MavlinkCommandLongHandler callback;
        const void* cookie; // This is the identification to unregister.
    };

    std::mutex _mavlink_command_handler_table_mutex{};
    std::vector<MAVLinkCommandIntHandlerTableEntry> _mavlink_command_int_handler_table{};
    std::vector<MAVLinkCommandLongHandlerTableEntry> _mavlink_command_long_handler_table{};
};

} // namespace mavsdk
