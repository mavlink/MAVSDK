#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include "mavlink_include.h"

namespace mavsdk {

class MAVLinkMessageHandler {
public:
    using Callback = std::function<void(const mavlink_message_t&)>;

    struct Entry {
        uint16_t msg_id;
        Callback callback;
        const void* cookie; // This is the identification to unregister.
    };

    void register_one(uint16_t msg_id, Callback callback, const void* cookie);
    void unregister_one(uint16_t msg_id, const void* cookie);
    void unregister_all(const void* cookie);
    void process_message(const mavlink_message_t& message);

private:
    std::mutex _mutex{};
    std::vector<Entry> _table{};
};

} // namespace mavsdk
