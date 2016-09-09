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

Result DroneLink::add_udp_connection()
{
    const std::string empty_ip;
    return add_udp_connection(empty_ip);
}

Result DroneLink::add_udp_connection(int port_number)
{
    const std::string empty_ip;
    return add_udp_connection(empty_ip, port_number);
}

Result DroneLink::add_udp_connection(const std::string &ip)
{
    int port_number = 0;
    return add_udp_connection(ip, port_number);
}

Result DroneLink::add_udp_connection(const std::string &ip, int port_number)
{
    if (port_number == 0) {
        port_number = _impl->DEFAULT_UDP_RECEIVE_PORT;
    }

    Connection *new_connection = new UdpConnection(_impl, ip, port_number);
    Result ret = new_connection->start();

    if (ret != Result::SUCCESS) {
        delete new_connection;
        return ret;
    }

    _impl->add_connection(new_connection);
    return Result::SUCCESS;
}

const std::vector<uint64_t> &DroneLink::device_uuids() const
{
    return _impl->get_device_uuids();
}

Device &DroneLink::device(uint64_t uuid) const
{
    return _impl->get_device(uuid);
}


} // namespace dronelink
