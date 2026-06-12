#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include <optional>
#include "mavlink_include.hpp"
#include "mavsdk_export.h"

namespace mavsdk {

class MAVSDK_TEST_EXPORT MavlinkMessageHandler {
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
    void unregister_all_blocking(const void* cookie);
    void process_message(const mavlink_message_t& message);
    void update_component_id(uint16_t msg_id, uint8_t cmp_id, const void* cookie);

private:
    void register_one_impl(
        uint16_t msg_id,
        std::optional<uint8_t> maybe_component_id,
        const Callback& callback,
        const void* cookie);

    void unregister_impl(std::optional<uint16_t> maybe_msg_id, const void* cookie);

    // Erase entries matching cookie (and optionally msg_id) from _table.
    // Must be called with _mutex held.
    void erase_from_table(std::optional<uint16_t> maybe_msg_id, const void* cookie);

    // Apply operations that were deferred because _mutex was held when they came in.
    // Must be called with _mutex held.
    void apply_deferred_with_mutex_held();

    std::mutex _mutex{};
    std::vector<Entry> _table{};

    // When register/unregister is called from within a callback on the same thread,
    // the operation is deferred here and applied at the end of process_message.
    struct DeferredOp {
        enum class Action { Register, Unregister } action;
        Entry entry{}; // used for Register
        std::optional<uint16_t> maybe_msg_id{}; // used for Unregister
        const void* cookie{nullptr}; // used for Unregister
    };

    std::mutex _deferred_mutex{};
    std::vector<DeferredOp> _deferred_ops{};

    bool _debugging{false};
};

} // namespace mavsdk
