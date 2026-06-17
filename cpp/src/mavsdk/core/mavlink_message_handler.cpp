#include <algorithm>
#include <future>
#include <utility>
#include <asio/post.hpp>
#include "mavlink_message_handler.hpp"
#include "log.hpp"

namespace mavsdk {

MavlinkMessageHandler::MavlinkMessageHandler(asio::io_context& io_context) : _io_context(io_context)
{
    if (const char* env_p = std::getenv("MAVSDK_MESSAGE_HANDLER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug("Mavlink message handler debugging is on.");
            _debugging = true;
        }
    }
}

void MavlinkMessageHandler::register_one(
    uint16_t msg_id, const Callback& callback, const void* cookie)
{
    register_one_impl(msg_id, {}, callback, cookie);
}

void MavlinkMessageHandler::register_one_with_component_id(
    uint16_t msg_id, uint8_t component_id, const Callback& callback, const void* cookie)
{
    register_one_impl(msg_id, {component_id}, callback, cookie);
}

void MavlinkMessageHandler::register_one_impl(
    uint16_t msg_id,
    std::optional<uint8_t> maybe_component_id,
    const Callback& callback,
    const void* cookie)
{
    Entry entry = {msg_id, maybe_component_id, callback, cookie};
    asio::post(_io_context, [this, entry = std::move(entry)]() mutable {
        _table.push_back(std::move(entry));
    });
}

void MavlinkMessageHandler::unregister_one(uint16_t msg_id, const void* cookie)
{
    unregister_impl({msg_id}, cookie);
}

void MavlinkMessageHandler::unregister_all(const void* cookie)
{
    unregister_impl({}, cookie);
}

void MavlinkMessageHandler::unregister_impl(
    std::optional<uint16_t> maybe_msg_id, const void* cookie)
{
    asio::post(
        _io_context, [this, maybe_msg_id, cookie]() { erase_from_table(maybe_msg_id, cookie); });
}

void MavlinkMessageHandler::unregister_all_on_io_thread(const void* cookie)
{
    // Direct, synchronous removal. MUST be called on the io_context thread (e.g. from an
    // owner that is itself created and destroyed on the io thread, like a work-queue item).
    // The table is only touched on the io thread, so this is race-free, and removing the
    // entry before the owner is freed avoids any fire-after-free without a posted round-trip.
    erase_from_table({}, cookie);
}

void MavlinkMessageHandler::unregister_all_blocking(const void* cookie)
{
    // Synchronous removal for callers that are NOT on the io_context thread (e.g. plugin
    // destructors on the user thread). MUST NOT be called from the io thread -- it would
    // post a task and then wait on the very thread that needs to run it. io-thread owners
    // use unregister_all_on_io_thread() instead.
    if (_io_context.stopped()) {
        // The io_context thread is dead -- safe to access the table directly.
        erase_from_table({}, cookie);
        return;
    }

    std::promise<void> done;
    asio::post(_io_context, [this, cookie, &done]() {
        erase_from_table({}, cookie);
        done.set_value();
    });
    done.get_future().wait();
}

void MavlinkMessageHandler::erase_from_table(
    std::optional<uint16_t> maybe_msg_id, const void* cookie)
{
    _table.erase(
        std::remove_if(
            _table.begin(),
            _table.end(),
            [&](auto& entry) {
                if (maybe_msg_id) {
                    return (entry.msg_id == maybe_msg_id.value() && entry.cookie == cookie);
                } else {
                    return (entry.cookie == cookie);
                }
            }),
        _table.end());
}

void MavlinkMessageHandler::process_message(const mavlink_message_t& message)
{
    // Runs on the io_context thread; _table is only touched there, so no lock is needed.
    bool forwarded = false;

    if (_debugging) {
        LogDebug("Table entries: ");
    }

    for (auto& entry : _table) {
        if (_debugging) {
            LogDebug(
                "Msg id: {}, component id: {}",
                entry.msg_id,
                (entry.component_id.has_value() ? std::to_string(entry.component_id.value()) :
                                                  "none"));
        }

        if (entry.msg_id == message.msgid &&
            (!entry.component_id.has_value() || entry.component_id.value() == message.compid)) {
            if (_debugging) {
                LogDebug("Using msg {} to {}", int(message.msgid), size_t(entry.cookie));
            }

            forwarded = true;
            entry.callback(message);
        }
    }

    if (_debugging && !forwarded) {
        LogDebug("Ignoring msg {}", int(message.msgid));
    }
}

void MavlinkMessageHandler::update_component_id(
    uint16_t msg_id, uint8_t component_id, const void* cookie)
{
    asio::post(_io_context, [this, msg_id, component_id, cookie]() {
        for (auto& entry : _table) {
            if (entry.msg_id == msg_id && entry.cookie == cookie) {
                entry.component_id = component_id;
            }
        }
    });
}

} // namespace mavsdk
