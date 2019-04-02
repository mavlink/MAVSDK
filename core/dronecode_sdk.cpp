#include "dronecode_sdk.h"

#include "dronecode_sdk_impl.h"
#include "global_include.h"

namespace dronecode_sdk {

DronecodeSDK::DronecodeSDK() : _impl{new DronecodeSDKImpl()} {}

DronecodeSDK::~DronecodeSDK() {}

ConnectionResult DronecodeSDK::add_any_connection(const std::string &connection_url)
{
    return _impl->add_any_connection(connection_url);
}

ConnectionResult DronecodeSDK::add_udp_connection(int local_port)
{
    return DronecodeSDK::add_udp_connection(DEFAULT_UDP_BIND_IP, local_port);
}

ConnectionResult DronecodeSDK::add_udp_connection(const std::string &local_bind_ip,
                                                  const int local_port)
{
    return _impl->add_udp_connection(local_bind_ip, local_port);
}

ConnectionResult DronecodeSDK::add_tcp_connection(const std::string &remote_ip,
                                                  const int remote_port)
{
    return _impl->add_tcp_connection(remote_ip, remote_port);
}

ConnectionResult DronecodeSDK::add_serial_connection(const std::string &dev_path,
                                                     const int baudrate)
{
    return _impl->add_serial_connection(dev_path, baudrate);
}

void DronecodeSDK::set_configuration(Configuration configuration)
{
    _impl->set_configuration(configuration);
}

std::vector<uint64_t> DronecodeSDK::system_uuids() const
{
    return _impl->get_system_uuids();
}

System &DronecodeSDK::system() const
{
    return _impl->get_system();
}

System &DronecodeSDK::system(const uint64_t uuid) const
{
    return _impl->get_system(uuid);
}

bool DronecodeSDK::is_connected() const
{
    return _impl->is_connected();
}

bool DronecodeSDK::is_connected(const uint64_t uuid) const
{
    return _impl->is_connected(uuid);
}

void DronecodeSDK::register_on_discover(const event_callback_t callback)
{
    _impl->register_on_discover(callback);
}

void DronecodeSDK::register_on_timeout(const event_callback_t callback)
{
    _impl->register_on_timeout(callback);
}

} // namespace dronecode_sdk
