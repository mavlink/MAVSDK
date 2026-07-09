#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>
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
    // (e.g. work-queue items). Must be called on the io thread, but not from within a
    // message callback (the removal would invalidate the dispatch in process_message()).
    void unregister_all_on_io_thread(const void* cookie);
    // Synchronous unregister for owners about to be destroyed: once this returns, no
    // callback for this cookie can fire anymore. Callable from any thread -- on the io
    // thread it removes directly, off it it posts the removal and waits for it. The one
    // exception: it must not be called from within a message callback (see
    // unregister_all_on_io_thread above); use the posted unregister_all() there instead.
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

    bool on_io_thread() const;

    // Record -- and in debug builds check -- which thread accesses _table; called by every
    // code path that touches the table. See the implementation for why this exists in
    // addition to on_io_thread().
    void note_table_thread();

    asio::io_context& _io_context;
    std::vector<Entry> _table{};

    // The single thread that is allowed to touch _table: the io thread while the
    // io_context runs, the teardown thread once it is stopped. Only used by
    // note_table_thread().
    std::atomic<std::thread::id> _table_thread_id{};

    // True while process_message() dispatches to the callbacks; only touched on the io
    // thread. Used to assert that no callback removes entries from the table directly
    // underneath the dispatch loop.
    bool _processing{false};

    bool _debugging{false};
};

} // namespace mavsdk
