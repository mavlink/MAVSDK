#include "dronecore.h"
#include "dronecore_impl.h"
#include "global_include.h"
#include "connection.h"
#include "udp_connection.h"

namespace dronecore {

DroneCore::DroneCore() :
    _impl(nullptr)
{
    _impl = new DroneCoreImpl();
}

DroneCore::~DroneCore()
{
    delete _impl;
    _impl = nullptr;
}

DroneCore::ConnectionResult DroneCore::add_udp_connection()
{
    return add_udp_connection(0);
}

DroneCore::ConnectionResult DroneCore::add_udp_connection(int local_port_number)
{
    Connection *new_connection = new UdpConnection(_impl, local_port_number);
    DroneCore::ConnectionResult ret = new_connection->start();

    if (ret != DroneCore::ConnectionResult::SUCCESS) {
        delete new_connection;
        return ret;
    }

    _impl->add_connection(new_connection);
    return DroneCore::ConnectionResult::SUCCESS;
}

const std::vector<uint64_t> &DroneCore::device_uuids() const
{
    return _impl->get_device_uuids();
}

Device &DroneCore::device() const
{
    return _impl->get_device();
}

Device &DroneCore::device(uint64_t uuid) const
{
    return _impl->get_device(uuid);
}

void DroneCore::register_on_discover(event_callback_t callback)
{
    _impl->register_on_discover(callback);
}

void DroneCore::register_on_timeout(event_callback_t callback)
{
    _impl->register_on_timeout(callback);
}

const char *DroneCore::connection_result_str(ConnectionResult result)
{
    switch (result) {
        case ConnectionResult::SUCCESS:
            return "Success";
        case ConnectionResult::TIMEOUT:
            return "Timeout";
        case ConnectionResult::SOCKET_ERROR:
            return "Socket error";
        case ConnectionResult::BIND_ERROR:
            return "Bind error";
        case ConnectionResult::CONNECTION_ERROR:
            return "Connection error";
        case ConnectionResult::NOT_IMPLEMENTED:
            return "Not implemented";
        case ConnectionResult::DEVICE_NOT_CONNECTED:
            return "Device not connected";
        case ConnectionResult::DEVICE_BUSY:
            return "Device busy";
        case ConnectionResult::COMMAND_DENIED:
            return "Command denied";
        case ConnectionResult::DESTINATION_IP_UNKNOWN:
            return "Destination IP unknown";
        case ConnectionResult::CONNECTIONS_EXHAUSTED:
            return "Connections exhausted";
        default:
            return "Unknown";
    }
}

} // namespace dronecore

