#pragma once

#include "mavlink_command_sender.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "mavlink_include.h"
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
    MavlinkRequestMessage() = delete;

    using MavlinkRequestMessageCallback =
        std::function<void(MavlinkCommandSender::Result, const mavlink_message_t&)>;

    void request(
        uint32_t message_id,
        uint8_t target_component,
        MavlinkRequestMessageCallback callback,
        uint32_t param2 = 0);

private:
    struct WorkItem {
        uint32_t message_id{0};
        uint8_t target_component{0};
        MavlinkRequestMessageCallback callback{};
        uint32_t param2{0};
        unsigned retries{0};
        TimeoutHandler::Cookie timeout_cookie{};
        std::optional<MavlinkCommandSender::Result> maybe_result{};
    };

    void send_request(WorkItem& item);
    void send_request_using_new_command(WorkItem& item);
    bool try_sending_request_using_old_command(WorkItem& item);
    void handle_any_message(const mavlink_message_t& message);
    void handle_command_result(uint32_t message_id, MavlinkCommandSender::Result result);
    void handle_timeout(uint32_t message_id, uint8_t target_component);

    SystemImpl& _system_impl;
    MavlinkCommandSender& _command_sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;

    std::mutex _mutex{};
    std::vector<WorkItem> _work_items{};
    std::vector<int> _deferred_message_cleanup{};
};

} // namespace mavsdk
