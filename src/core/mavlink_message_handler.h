#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include <map>
#include "mavlink_include.h"

namespace mavsdk {

class MAVLinkMessageHandler {
public:
    using Callback = std::function<void(const mavlink_message_t&)>;

    struct Entry {
        Callback callback;
        const void* cookie; // This is the identification to unregister.
    };

    void register_one(uint16_t msg_id, const Callback& callback, const void* cookie);
    void unregister_one(uint16_t msg_id, const void* cookie);
    void unregister_all(const void* cookie);
    void process_message(const mavlink_message_t& message);

private:
    std::mutex _mutex{};
    std::map<uint32_t, std::vector<Entry>> _map{};
};

} // namespace mavsdk
