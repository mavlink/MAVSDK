#include "connection.h"

#include <memory>
#include <utility>
#include "mavsdk_impl.h"
#include "log.h"

namespace mavsdk {

std::atomic<unsigned> Connection::_forwarding_connections_count = 0;

Connection::Connection(
    ReceiverCallback receiver_callback,
    LibmavReceiverCallback libmav_receiver_callback,
    MavsdkImpl& mavsdk_impl,
    ForwardingOption forwarding_option) :
    _receiver_callback(std::move(receiver_callback)),
    _libmav_receiver_callback(std::move(libmav_receiver_callback)),
    _mavsdk_impl(mavsdk_impl),
    _mavlink_receiver(),
    _libmav_receiver(),
    _forwarding_option(forwarding_option)
{
    // Insert system ID 0 in all connections for broadcast.
    _system_ids.insert(0);

    if (forwarding_option == ForwardingOption::ForwardingOn) {
        _forwarding_connections_count++;
    }

    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
}

Connection::~Connection()
{
    // Just in case a specific connection didn't call it already.
    stop_mavlink_receiver();
    stop_libmav_receiver();
    _receiver_callback = {};
    _libmav_receiver_callback = {};
}

bool Connection::start_mavlink_receiver()
{
    _mavlink_receiver = std::make_unique<MavlinkReceiver>();
    return true;
}

void Connection::stop_mavlink_receiver()
{
    if (_mavlink_receiver) {
        _mavlink_receiver.reset();
    }
}

bool Connection::start_libmav_receiver()
{
    _libmav_receiver = std::make_unique<LibmavReceiver>(_mavsdk_impl);
    return true;
}

void Connection::stop_libmav_receiver()
{
    if (_libmav_receiver) {
        _libmav_receiver.reset();
    }
}

void Connection::receive_libmav_message(
    const Mavsdk::MavlinkMessage& message, Connection* connection)
{
    // Register system ID when receiving a message from a new system.
    if (_system_ids.find(message.system_id) == _system_ids.end()) {
        _system_ids.insert(message.system_id);
    }

    if (_debugging) {
        LogDebug() << "Connection::receive_libmav_message: " << message.message_name
                   << " from system " << message.system_id;
    }

    if (_libmav_receiver_callback) {
        if (_debugging) {
            LogDebug() << "Calling libmav receiver callback for: " << message.message_name;
        }
        _libmav_receiver_callback(message, connection);
    } else {
        LogWarn() << "No libmav receiver callback set!";
    }
}

void Connection::receive_message(mavlink_message_t& message, Connection* connection)
{
    // Register system ID when receiving a message from a new system.
    if (_system_ids.find(message.sysid) == _system_ids.end()) {
        _system_ids.insert(message.sysid);
    }
    _receiver_callback(message, connection);
}

bool Connection::should_forward_messages() const
{
    return _forwarding_option == ForwardingOption::ForwardingOn;
}

unsigned Connection::forwarding_connections_count()
{
    return _forwarding_connections_count;
}

bool Connection::has_system_id(uint8_t system_id)
{
    return _system_ids.find(system_id) != _system_ids.end();
}

} // namespace mavsdk
