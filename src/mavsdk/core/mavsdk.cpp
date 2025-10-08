#include "mavsdk.h"

#include "mavsdk_impl.h"

namespace mavsdk {

Mavsdk::Mavsdk(Configuration configuration)
{
    _impl = std::make_shared<MavsdkImpl>(configuration);
}

Mavsdk::~Mavsdk() = default;

std::string Mavsdk::version() const
{
    return _impl->version();
}

ConnectionResult
Mavsdk::add_any_connection(const std::string& connection_url, ForwardingOption forwarding_option)
{
    return _impl->add_any_connection(connection_url, forwarding_option).first;
}

std::pair<ConnectionResult, Mavsdk::ConnectionHandle> Mavsdk::add_any_connection_with_handle(
    const std::string& connection_url, ForwardingOption forwarding_option)
{
    return _impl->add_any_connection(connection_url, forwarding_option);
}

void Mavsdk::remove_connection(ConnectionHandle handle)
{
    _impl->remove_connection(handle);
}

std::vector<std::shared_ptr<System>> Mavsdk::systems() const
{
    return _impl->systems();
}

std::optional<std::shared_ptr<System>> Mavsdk::first_autopilot(double timeout_s) const
{
    return _impl->first_autopilot(timeout_s);
}

void Mavsdk::set_configuration(Configuration configuration)
{
    _impl->set_configuration(configuration);
}

void Mavsdk::set_timeout_s(double timeout_s)
{
    _impl->set_timeout_s(timeout_s);
}

Mavsdk::NewSystemHandle Mavsdk::subscribe_on_new_system(const NewSystemCallback& callback)
{
    return _impl->subscribe_on_new_system(callback);
}

void Mavsdk::unsubscribe_on_new_system(NewSystemHandle handle)
{
    _impl->unsubscribe_on_new_system(handle);
}

std::shared_ptr<ServerComponent> Mavsdk::server_component(unsigned instance)
{
    return _impl->server_component(instance);
}

std::shared_ptr<ServerComponent>
Mavsdk::server_component_by_type(ComponentType server_component_type, unsigned instance)
{
    return _impl->server_component_by_type(server_component_type, instance);
}

std::shared_ptr<ServerComponent> Mavsdk::server_component_by_id(uint8_t component_id)
{
    return _impl->server_component_by_id(component_id);
}

Mavsdk::Configuration::Configuration(
    uint8_t system_id, uint8_t component_id, bool always_send_heartbeats) :
    _system_id(system_id),
    _component_id(component_id),
    _always_send_heartbeats(always_send_heartbeats),
    _component_type(component_type_for_component_id(component_id)),
    _mav_type(mav_type_for_component_type(component_type_for_component_id(component_id)))
{}

ComponentType Mavsdk::Configuration::component_type_for_component_id(uint8_t component_id)
{
    switch (component_id) {
        case Mavsdk::DEFAULT_COMPONENT_ID_AUTOPILOT:
            return ComponentType::Autopilot;
        case Mavsdk::DEFAULT_COMPONENT_ID_GCS:
            return ComponentType::GroundStation;
        case Mavsdk::DEFAULT_COMPONENT_ID_CC:
            return ComponentType::CompanionComputer;
        case Mavsdk::DEFAULT_COMPONENT_ID_CAMERA:
            return ComponentType::Camera;
        case Mavsdk::DEFAULT_COMPONENT_ID_GIMBAL:
            return ComponentType::Gimbal;
        case Mavsdk::DEFAULT_COMPONENT_ID_REMOTEID:
            return ComponentType::RemoteId;
        default:
            return ComponentType::Custom;
    }
}

MAV_TYPE Mavsdk::Configuration::mav_type_for_component_type(ComponentType component_type)
{
    switch (component_type) {
        case ComponentType::Autopilot:
            return MAV_TYPE_GENERIC;
        case ComponentType::GroundStation:
            return MAV_TYPE_GCS;
        case ComponentType::CompanionComputer:
            return MAV_TYPE_ONBOARD_CONTROLLER;
        case ComponentType::Camera:
            return MAV_TYPE_CAMERA;
        case ComponentType::Gimbal:
            return MAV_TYPE_GIMBAL;
        case ComponentType::RemoteId:
            return MAV_TYPE_ODID;
        case ComponentType::Custom:
            return MAV_TYPE_GENERIC;
        default:
            return MAV_TYPE_GENERIC;
    }
}

Mavsdk::Configuration::Configuration(ComponentType component_type) :
    _system_id(Mavsdk::DEFAULT_SYSTEM_ID_GCS),
    _component_id(Mavsdk::DEFAULT_COMPONENT_ID_GCS),
    _always_send_heartbeats(false),
    _component_type(component_type),
    _mav_type(mav_type_for_component_type(component_type))
{
    switch (component_type) {
        case ComponentType::GroundStation:
            _system_id = Mavsdk::DEFAULT_SYSTEM_ID_GCS;
            _component_id = Mavsdk::DEFAULT_COMPONENT_ID_GCS;
            _always_send_heartbeats = false;
            break;
        case ComponentType::CompanionComputer:
            // TODO implement auto-detection of system ID - maybe from heartbeats?
            _system_id = Mavsdk::DEFAULT_SYSTEM_ID_CC;
            _component_id = Mavsdk::DEFAULT_COMPONENT_ID_CC;
            _always_send_heartbeats = true;
            break;
        case ComponentType::Autopilot:
            _system_id = Mavsdk::DEFAULT_SYSTEM_ID_AUTOPILOT;
            _component_id = Mavsdk::DEFAULT_COMPONENT_ID_AUTOPILOT;
            _always_send_heartbeats = true;
            break;
        case ComponentType::Camera:
            _system_id = Mavsdk::DEFAULT_SYSTEM_ID_CAMERA;
            _component_id = Mavsdk::DEFAULT_COMPONENT_ID_CAMERA;
            _always_send_heartbeats = true;
            break;
        case ComponentType::Gimbal:
            _system_id = Mavsdk::DEFAULT_SYSTEM_ID_GIMBAL;
            _component_id = Mavsdk::DEFAULT_COMPONENT_ID_GIMBAL;
            _always_send_heartbeats = true;
            break;
        case ComponentType::RemoteId:
            _system_id = Mavsdk::DEFAULT_SYSTEM_ID_REMOTEID;
            _component_id = Mavsdk::DEFAULT_COMPONENT_ID_REMOTEID;
            _always_send_heartbeats = true;
            break;
        default:
            break;
    }
}

uint8_t Mavsdk::Configuration::get_system_id() const
{
    return _system_id;
}

void Mavsdk::Configuration::set_system_id(uint8_t system_id)
{
    _system_id = system_id;
}

uint8_t Mavsdk::Configuration::get_component_id() const
{
    return _component_id;
}

void Mavsdk::Configuration::set_component_id(uint8_t component_id)
{
    _component_id = component_id;
}

bool Mavsdk::Configuration::get_always_send_heartbeats() const
{
    return _always_send_heartbeats;
}

void Mavsdk::Configuration::set_always_send_heartbeats(bool always_send_heartbeats)
{
    _always_send_heartbeats = always_send_heartbeats;
}

ComponentType Mavsdk::Configuration::get_component_type() const
{
    return _component_type;
}

void Mavsdk::Configuration::set_component_type(ComponentType component_type)
{
    _component_type = component_type;
}

uint8_t Mavsdk::Configuration::get_mav_type() const
{
    return _mav_type;
}

void Mavsdk::Configuration::set_mav_type(uint8_t mav_type)
{
    _mav_type = static_cast<MAV_TYPE>(mav_type);
}

void Mavsdk::intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback)
{
    _impl->intercept_incoming_messages_async(callback);
}

void Mavsdk::intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback)
{
    _impl->intercept_outgoing_messages_async(callback);
}

bool Mavsdk::send_raw_bytes(const char* bytes, size_t length)
{
    return _impl->send_raw_bytes(bytes, length);
}

Mavsdk::RawBytesHandle Mavsdk::subscribe_raw_bytes(RawBytesCallback callback)
{
    return _impl->subscribe_raw_bytes(callback);
}

void Mavsdk::unsubscribe_raw_bytes(RawBytesHandle handle)
{
    _impl->unsubscribe_raw_bytes(handle);
}

Mavsdk::ConnectionErrorHandle
Mavsdk::subscribe_connection_errors(Mavsdk::ConnectionErrorCallback callback)
{
    return _impl->subscribe_connection_errors(callback);
}

void Mavsdk::unsubscribe_connection_errors(ConnectionErrorHandle handle)
{
    return _impl->unsubscribe_connection_errors(handle);
}

Mavsdk::InterceptJsonHandle
Mavsdk::subscribe_incoming_messages_json(const InterceptJsonCallback& callback)
{
    return _impl->subscribe_incoming_messages_json(callback);
}

void Mavsdk::unsubscribe_incoming_messages_json(InterceptJsonHandle handle)
{
    _impl->unsubscribe_incoming_messages_json(handle);
}

Mavsdk::InterceptJsonHandle
Mavsdk::subscribe_outgoing_messages_json(const InterceptJsonCallback& callback)
{
    return _impl->subscribe_outgoing_messages_json(callback);
}

void Mavsdk::unsubscribe_outgoing_messages_json(InterceptJsonHandle handle)
{
    _impl->unsubscribe_outgoing_messages_json(handle);
}

} // namespace mavsdk
