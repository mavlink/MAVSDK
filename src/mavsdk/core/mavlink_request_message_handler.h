#pragma once

#include <array>
#include <functional>
#include <mutex>
#include <optional>
#include <vector>
#include "mavlink_include.h"
#include "mavlink_command_receiver.h"

namespace mavsdk {

class SystemImpl;

class MavlinkRequestMessageHandler {
public:
    MavlinkRequestMessageHandler() = delete;
    explicit MavlinkRequestMessageHandler(SystemImpl& system_impl);
    ~MavlinkRequestMessageHandler();

    using Params = std::array<float, 5>;
    using Callback = std::function<std::optional<MAV_RESULT>(const Params&)>;

    bool register_handler(uint32_t message_id, const Callback& callback, const void* cookie);
    void unregister_handler(uint32_t message_id, const void* cookie);
    void unregister_all_handlers(const void* cookie);

private:
    std::optional<mavlink_message_t>
    handle_command_long(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    handle_command_int(const MavlinkCommandReceiver::CommandInt& command);

    struct Entry {
        uint32_t message_id;
        Callback callback;
        const void* cookie;
    };

    std::mutex _table_mutex{};
    std::vector<Entry> _table{};

    SystemImpl& _system_impl;
};

} // namespace mavsdk
