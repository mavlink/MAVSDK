#include "dronelink.h"
#include "dronelink_impl.h"
#include "global_include.h"
#include "connection.h"
#include "udp_connection.h"


namespace dronelink {

DroneLink::DroneLink() :
    _impl(nullptr)
{
    _impl = new DroneLinkImpl();
}

DroneLink::~DroneLink()
{
    delete _impl;
    _impl = nullptr;
}

DroneLink::ConnectionResult DroneLink::add_udp_connection()
{
    return add_udp_connection(0);
}

DroneLink::ConnectionResult DroneLink::add_udp_connection(int local_port_number)
{
    Connection *new_connection = new UdpConnection(_impl, local_port_number);
    DroneLink::ConnectionResult ret = new_connection->start();

    if (ret != DroneLink::ConnectionResult::SUCCESS) {
        delete new_connection;
        return ret;
    }

    _impl->add_connection(new_connection);
    return DroneLink::ConnectionResult::SUCCESS;
}

const std::vector<uint64_t> &DroneLink::device_uuids() const
{
    return _impl->get_device_uuids();
}

Device &DroneLink::device() const
{
    return _impl->get_device();
}

Device &DroneLink::device(uint64_t uuid) const
{
    return _impl->get_device(uuid);
}

void DroneLink::register_on_discover(event_callback_t callback)
{
    _impl->register_on_discover(callback);
}
void DroneLink::register_on_timeout(event_callback_t callback)
{
    _impl->register_on_timeout(callback);
}


} // namespace dronelink
