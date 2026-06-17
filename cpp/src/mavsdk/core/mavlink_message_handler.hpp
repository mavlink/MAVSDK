#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <optional>
#include <asio/io_context.hpp>
#include "mavlink_include.hpp"
#include "mavsdk_export.h"

namespace mavsdk {

// Dispatches received MAVLink messages to registered handlers.
//
// Threading: the handler table is only ever touched on the io_context thread.
// process_message() runs there (it is driven by the connection receive path, which
// posts onto the io_context), and register/unregister post their mutation onto the
// same io_context. That keeps everything single-threaded without locks, and post()
// also makes it safe to (un)register from inside a callback (the mutation runs after
// the current dispatch returns), with ordering preserved by post FIFO.
class MAVSDK_TEST_EXPORT MavlinkMessageHandler {
public:
    explicit MavlinkMessageHandler(asio::io_context& io_context);

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
    // Direct, synchronous unregister for owners that live and die ON the io_context thread
    // (e.g. work-queue items). Must be called on the io thread.
    void unregister_all_on_io_thread(const void* cookie);
    // Synchronous unregister for owners destroyed OFF the io_context thread (e.g. plugins
    // on the user thread): posts the removal to the io thread and waits for it, so no
    // callback for this cookie can fire afterwards. Must NOT be called from the io thread.
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

    // Erase entries matching cookie (and optionally msg_id) from the table.
    // Must be called on the io_context thread.
    void erase_from_table(std::optional<uint16_t> maybe_msg_id, const void* cookie);

    asio::io_context& _io_context;
    std::vector<Entry> _table{};

    bool _debugging{false};
};

} // namespace mavsdk
