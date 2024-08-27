#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include <optional>
#include "mavlink_include.h"

namespace mavsdk {

class MavlinkMessageHandler {
public:
    MavlinkMessageHandler();

    using Callback = std::function<void(const mavlink_message_t&)>;

    struct Entry {
        uint32_t msg_id;
        std::optional<uint8_t> component_id;
        Callback callback;
        const void* cookie; // This is the identification to unregister.
    };

    void register_one(uint16_t msg_id, const Callback& callback, const void* cookie);
    void register_one_with_component_id(
        uint16_t msg_id, uint8_t component_id, const Callback& callback, const void* cookie);
    void unregister_one(uint16_t msg_id, const void* cookie);
    void unregister_all(const void* cookie);
    void process_message(const mavlink_message_t& message);
    void update_component_id(uint16_t msg_id, uint8_t cmp_id, const void* cookie);

private:
    void register_one_impl(
        uint16_t msg_id,
        std::optional<uint8_t> maybe_component_id,
        const Callback& callback,
        const void* cookie);

    void unregister_impl(std::optional<uint16_t> maybe_msg_id, const void* cookie);

    void check_register_later();
    void check_unregister_later();

    std::mutex _mutex{};
    std::vector<Entry> _table{};

    std::mutex _register_later_mutex{};
    std::vector<Entry> _register_later_table{};

    struct UnregisterEntry {
        std::optional<uint16_t> maybe_msg_id;
        const void* cookie;
    };

    std::mutex _unregister_later_mutex{};
    std::vector<UnregisterEntry> _unregister_later_table{};

    bool _debugging{false};
};

} // namespace mavsdk
