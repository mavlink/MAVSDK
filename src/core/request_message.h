#pragma once

#include "mavlink_commands.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "mavlink_include.h"
#include <functional>
#include <mutex>
#include <optional>
#include <vector>

namespace mavsdk {

class RequestMessage {
public:
    RequestMessage(
        MavlinkCommandSender& command_sender,
        MAVLinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler);
    RequestMessage() = delete;

    using RequestMessageCallback =
        std::function<void(MavlinkCommandSender::Result, const mavlink_message_t&)>;

    void request(
        uint32_t message_id,
        RequestMessageCallback callback,
        uint8_t target_component_id = MAV_COMP_ID_AUTOPILOT1,
        uint32_t param2 = 0);

private:
    struct WorkItem {
        uint32_t message_id{0};
        RequestMessageCallback callback{};
        uint32_t param2{0};
        std::size_t retries{0};
        void* timeout_cookie{nullptr};
        std::optional<MavlinkCommandSender::Result> maybe_result{};
    };

    void send_request(uint32_t message_id, uint8_t target_component_id = MAV_COMP_ID_AUTOPILOT1);
    void handle_any_message(const mavlink_message_t& message);
    void handle_command_result(uint32_t message_id, MavlinkCommandSender::Result result);
    void handle_timeout(uint32_t message_id);

    MavlinkCommandSender& _command_sender;
    MAVLinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;

    std::mutex _mutex{};
    std::vector<WorkItem> _work_items{};
    std::vector<int> _deferred_message_cleanup{};
};

} // namespace mavsdk
