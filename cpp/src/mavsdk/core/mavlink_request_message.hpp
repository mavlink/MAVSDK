#pragma once

#include "mavlink_command_sender.hpp"
#include "mavlink_message_handler.hpp"
#include "timeout_handler.hpp"
#include "mavlink_include.hpp"
#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <vector>

namespace mavsdk {

class SystemImpl;

class MavlinkRequestMessage {
public:
    MavlinkRequestMessage(
        SystemImpl& system_impl,
        MavlinkCommandSender& command_sender,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler);
    ~MavlinkRequestMessage();
    MavlinkRequestMessage() = delete;

    using MavlinkRequestMessageCallback =
        std::function<void(MavlinkCommandSender::Result, const mavlink_message_t&)>;

    void request(
        uint32_t message_id,
        uint8_t target_component,
        MavlinkRequestMessageCallback callback,
        uint32_t param2 = 0);

private:
    static constexpr unsigned RETRIES = 4;

    struct WorkItem {
        // Unique per request, so that a command result or timeout can be matched
        // back to the exact request that caused it. The message id is not enough:
        // a request usually completes as soon as the message arrives, and the next
        // request for the same message id is already in flight by the time the
        // previous COMMAND_ACK shows up.
        uint64_t id{0};
        uint32_t message_id{0};
        uint8_t target_component{0};
        MavlinkRequestMessageCallback callback{};
        uint32_t param2{0};
        // Doubles as the attempt number, so a result from a previous attempt of
        // this same request can be told apart from the current one.
        unsigned retries{0};
        TimeoutHandler::Cookie timeout_cookie{};
        std::optional<MavlinkCommandSender::Result> maybe_result{};
    };

    void send_request(WorkItem& item);
    void send_request_using_new_command(WorkItem& item);
    bool try_sending_request_using_old_command(WorkItem& item);
    void handle_any_message(const mavlink_message_t& message);
    void handle_command_result(uint64_t id, unsigned attempt, MavlinkCommandSender::Result result);
    void handle_timeout(uint64_t id);

    SystemImpl& _system_impl;
    MavlinkCommandSender& _command_sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;

    std::mutex _mutex{};
    std::vector<WorkItem> _work_items{};
    uint64_t _next_id{1};
    // Message ids we have registered a handler for. We register lazily on first
    // request and keep the handler until destruction, so there is never an
    // unregister racing a register. This stays tiny (a handful of message ids),
    // so a flat vector is faster than a node-based set.
    std::vector<uint32_t> _registered_message_ids{};

    bool _debugging{false};
};

} // namespace mavsdk
