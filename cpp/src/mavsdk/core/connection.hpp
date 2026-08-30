#pragma once

#include "mavsdk.hpp"
#include "mavlink_receiver.hpp"
#include "libmav_receiver.hpp"
#include <asio/io_context.hpp>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <utility>

namespace mavsdk {

class MavsdkImpl; // Forward declaration

class Connection {
public:
    using ReceiverCallback = std::function<void(
        MavlinkReceiver::ParseResult result, mavlink_message_t& message, Connection* connection)>;
    using LibmavReceiverCallback =
        std::function<void(const Mavsdk::MavlinkMessage& message, Connection* connection)>;

    explicit Connection(
        ReceiverCallback receiver_callback,
        LibmavReceiverCallback libmav_receiver_callback,
        MavsdkImpl& mavsdk_impl,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    virtual ~Connection();

    virtual ConnectionResult start() = 0;
    virtual ConnectionResult stop() = 0;

    virtual std::pair<bool, std::string> send_message(const mavlink_message_t& message) = 0;

    // Send raw bytes for forwarding unknown messages
    virtual std::pair<bool, std::string> send_raw_bytes(const char* bytes, size_t length) = 0;

    bool has_system_id(uint8_t system_id);
    bool should_forward_messages() const;
    static unsigned forwarding_connections_count();

    // Called by MavsdkImpl::add_connection() while the connection is being registered,
    // before anything can send through it. Lets the connection attribute an
    // asynchronous send failure to itself; see report_send_error().
    void set_handle(Mavsdk::ConnectionHandle handle) { _handle = handle; }

    // Access to libmav receiver for message creation.
    // Returns a shared_ptr so the caller can safely use the object even if
    // stop_libmav_receiver() is called concurrently on another thread.
    std::shared_ptr<LibmavReceiver> get_libmav_receiver()
    {
        std::lock_guard<std::mutex> lock(_libmav_receiver_mutex);
        return _libmav_receiver;
    }

    // Non-copyable
    Connection(const Connection&) = delete;
    const Connection& operator=(const Connection&) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(
        MavlinkReceiver::ParseResult result, mavlink_message_t& message, Connection* connection);

    bool start_libmav_receiver();
    void stop_libmav_receiver();
    void receive_libmav_message(const Mavsdk::MavlinkMessage& message, Connection* connection);

    // The io_context every connection does its async I/O on, owned by MavsdkImpl.
    // Preferable to static_cast-ing socket.get_executor().context() back to an
    // io_context&, which is only correct as long as nothing rebinds the executor.
    asio::io_context& io_context();

    // Post an empty handler onto the io_context and wait for it, so that every handler
    // queued before it has run. Used by stop() to make sure no completion handler still
    // references the connection once it returns. No-op if the io_context is already
    // stopped, in which case nothing can be running anymore anyway.
    //
    // Must not be called from the io_context thread -- it would wait for itself.
    void drain_io_context();

    // Report a send failure that was only discovered asynchronously, i.e. after
    // send_message()/send_raw_bytes() already returned. Reaches the same
    // Mavsdk::subscribe_connection_errors() subscribers as a synchronous failure.
    // Must be called on the io_context thread.
    void report_send_error(const std::string& message);

    // How many datagrams/chunks a connection queues for sending before the oldest is
    // dropped. At the MAVLink maximum of 280 bytes this caps a stalled link at ~280 KiB.
    static constexpr std::size_t MAX_TX_QUEUE_ITEMS = 1024;

    ReceiverCallback _receiver_callback{};
    LibmavReceiverCallback _libmav_receiver_callback{};
    MavsdkImpl& _mavsdk_impl; // For thread-safe MessageSet access
    std::unique_ptr<MavlinkReceiver> _mavlink_receiver;
    std::shared_ptr<LibmavReceiver> _libmav_receiver; // guarded by _libmav_receiver_mutex
    mutable std::mutex _libmav_receiver_mutex;
    ForwardingOption _forwarding_option;
    std::mutex _system_ids_mutex;
    std::unordered_set<uint8_t> _system_ids;

    // Set once by set_handle() during registration, read afterwards by
    // report_send_error() on the io thread. The registration happens under
    // MavsdkImpl::_mutex, which the io thread also takes before it can reach this
    // connection at all, so no further synchronisation is needed.
    Mavsdk::ConnectionHandle _handle{};

    bool _debugging = false;

    static std::atomic<unsigned> _forwarding_connections_count;

    // void received_mavlink_message(mavlink_message_t &);
};

#ifdef WINDOWS
std::string get_socket_error_string(int error_code);
#endif

} // namespace mavsdk
