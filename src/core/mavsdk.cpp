#include "mavsdk.h"

#include "mavsdk_impl.h"
#include "global_include.h"

namespace mavsdk {

Mavsdk::Mavsdk() : _impl{new MavsdkImpl()} {}

Mavsdk::~Mavsdk() {}

std::string Mavsdk::version() const
{
    return _impl->version();
}

ConnectionResult Mavsdk::add_any_connection(const std::string& connection_url)
{
    return _impl->add_any_connection(connection_url);
}

ConnectionResult Mavsdk::add_udp_connection(int local_port)
{
    return Mavsdk::add_udp_connection(DEFAULT_UDP_BIND_IP, local_port);
}

ConnectionResult Mavsdk::add_udp_connection(const std::string& local_bind_ip, const int local_port)
{
    return _impl->add_udp_connection(local_bind_ip, local_port);
}

ConnectionResult Mavsdk::setup_udp_remote(const std::string& remote_ip, int remote_port)
{
    return _impl->setup_udp_remote(remote_ip, remote_port);
}

ConnectionResult Mavsdk::add_tcp_connection(const std::string& remote_ip, const int remote_port)
{
    return _impl->add_tcp_connection(remote_ip, remote_port);
}

ConnectionResult Mavsdk::add_serial_connection(const std::string& dev_path, const int baudrate)
{
    return _impl->add_serial_connection(dev_path, baudrate);
}

void Mavsdk::set_configuration(Configuration configuration)
{
    _impl->set_configuration(configuration);
}

std::vector<uint64_t> Mavsdk::system_uuids() const
{
    return _impl->get_system_uuids();
}

System& Mavsdk::system() const
{
    return _impl->get_system();
}

System& Mavsdk::system(const uint64_t uuid) const
{
    return _impl->get_system(uuid);
}

bool Mavsdk::is_connected() const
{
    return _impl->is_connected();
}

bool Mavsdk::is_connected(const uint64_t uuid) const
{
    return _impl->is_connected(uuid);
}

void Mavsdk::register_on_discover(const event_callback_t callback)
{
    _impl->register_on_discover(callback);
}

void Mavsdk::register_on_timeout(const event_callback_t callback)
{
    _impl->register_on_timeout(callback);
}

Mavsdk::Configuration::Configuration(uint8_t system_id, uint8_t component_id) :
    _system_id(system_id),
    _component_id(component_id),
    _usage_type(Mavsdk::Configuration::UsageType::Custom)
{}

Mavsdk::Configuration::Configuration(UsageType usage_type) :
    _system_id(MavsdkImpl::DEFAULT_SYSTEM_ID_GCS),
    _component_id(MavsdkImpl::DEFAULT_COMPONENT_ID_GCS),
    _usage_type(usage_type)
{
    switch (usage_type) {
        case Mavsdk::Configuration::UsageType::GroundStation:
            _system_id = MavsdkImpl::DEFAULT_SYSTEM_ID_GCS;
            _component_id = MavsdkImpl::DEFAULT_COMPONENT_ID_GCS;
            break;
        case Mavsdk::Configuration::UsageType::CompanionComputer:
            // TODO implement autodetection of system ID - maybe from heartbeats?
            _system_id = MavsdkImpl::DEFAULT_SYSTEM_ID_CC;
            _component_id = MavsdkImpl::DEFAULT_COMPONENT_ID_CC;
            break;
        case Mavsdk::Configuration::UsageType::Autopilot:
            _system_id = MavsdkImpl::DEFAULT_SYSTEM_ID_AUTOPILOT;
            _component_id = MavsdkImpl::DEFAULT_COMPONENT_ID_AUTOPILOT;
            break;
        default:
            _system_id = MavsdkImpl::DEFAULT_SYSTEM_ID_GCS;
            _component_id = MavsdkImpl::DEFAULT_COMPONENT_ID_GCS;
            break;
    }
}

Mavsdk::Configuration::~Configuration() {}

uint8_t Mavsdk::Configuration::get_system_id() const
{
    return _system_id;
}

uint8_t Mavsdk::Configuration::get_component_id() const
{
    return _component_id;
}

Mavsdk::Configuration::UsageType Mavsdk::Configuration::get_usage_type() const
{
    return _usage_type;
}

} // namespace mavsdk
