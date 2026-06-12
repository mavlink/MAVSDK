#include <algorithm>
#include <mutex>
#include "mavlink_message_handler.hpp"
#include "log.hpp"

namespace mavsdk {

static thread_local bool tls_in_callback = false;

MavlinkMessageHandler::MavlinkMessageHandler()
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

    if (tls_in_callback) {
        std::lock_guard<std::mutex> deferred_lock(_deferred_mutex);
        _deferred_ops.push_back(
            DeferredOp{DeferredOp::Action::Register, entry, std::nullopt, nullptr});
    } else {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (lock.owns_lock()) {
            apply_deferred_with_mutex_held();
            _table.push_back(entry);
        } else {
            std::lock_guard<std::mutex> deferred_lock(_deferred_mutex);
            _deferred_ops.push_back(
                DeferredOp{DeferredOp::Action::Register, entry, std::nullopt, nullptr});
        }
    }
}

void MavlinkMessageHandler::unregister_one(uint16_t msg_id, const void* cookie)
{
    unregister_impl({msg_id}, cookie);
}

void MavlinkMessageHandler::unregister_all(const void* cookie)
{
    unregister_impl({}, cookie);
}

void MavlinkMessageHandler::unregister_all_blocking(const void* cookie)
{
    // Blocking version for use in destructors - waits for any in-flight callbacks to complete.
    // WARNING: Do NOT call this from within a message handler callback - it will deadlock.

    // Also purge any deferred operations for this cookie. If register_one() was
    // called while process_message() held _mutex, the entry landed in
    // _deferred_ops instead of _table.  Without this step, apply_deferred() would
    // later promote that stale entry into _table and fire a callback on a
    // destroyed object (heap-use-after-free).
    {
        std::lock_guard<std::mutex> deferred_lock(_deferred_mutex);
        _deferred_ops.erase(
            std::remove_if(
                _deferred_ops.begin(),
                _deferred_ops.end(),
                [&](const auto& op) {
                    const void* op_cookie =
                        (op.action == DeferredOp::Action::Register) ? op.entry.cookie : op.cookie;
                    return op_cookie == cookie;
                }),
            _deferred_ops.end());
    }

    std::lock_guard<std::mutex> lock(_mutex);
    erase_from_table({}, cookie);
}

void MavlinkMessageHandler::unregister_impl(
    std::optional<uint16_t> maybe_msg_id, const void* cookie)
{
    if (tls_in_callback) {
        std::lock_guard<std::mutex> deferred_lock(_deferred_mutex);
        _deferred_ops.push_back(
            DeferredOp{DeferredOp::Action::Unregister, Entry{}, maybe_msg_id, cookie});
    } else {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (lock.owns_lock()) {
            apply_deferred_with_mutex_held();
            erase_from_table(maybe_msg_id, cookie);
        } else {
            std::lock_guard<std::mutex> deferred_lock(_deferred_mutex);
            _deferred_ops.push_back(
                DeferredOp{DeferredOp::Action::Unregister, Entry{}, maybe_msg_id, cookie});
        }
    }
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

void MavlinkMessageHandler::apply_deferred_with_mutex_held()
{
    std::lock_guard<std::mutex> deferred_lock(_deferred_mutex);
    for (const auto& op : _deferred_ops) {
        if (op.action == DeferredOp::Action::Register) {
            _table.push_back(op.entry);
        } else {
            erase_from_table(op.maybe_msg_id, op.cookie);
        }
    }
    _deferred_ops.clear();
}

void MavlinkMessageHandler::process_message(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    apply_deferred_with_mutex_held();

    bool forwarded = false;

    if (_debugging) {
        LogDebug("Table entries: ");
    }

    tls_in_callback = true;
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
    tls_in_callback = false;

    apply_deferred_with_mutex_held();

    if (_debugging && !forwarded) {
        LogDebug("Ignoring msg {}", int(message.msgid));
    }
}

void MavlinkMessageHandler::update_component_id(
    uint16_t msg_id, uint8_t component_id, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    apply_deferred_with_mutex_held();

    for (auto& entry : _table) {
        if (entry.msg_id == msg_id && entry.cookie == cookie) {
            entry.component_id = component_id;
        }
    }
}

} // namespace mavsdk
