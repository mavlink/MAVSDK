#include "server_component_impl.h"
#include "mavsdk_impl.h"

namespace mavsdk {

ServerComponent::ServerComponent(MavsdkImpl& mavsdk_impl, uint8_t component_id) :
    _impl(std::make_unique<ServerComponentImpl>(mavsdk_impl, component_id))
{}

uint8_t ServerComponent::component_id() const
{
    return _impl->get_own_component_id();
}

void ServerComponent::set_system_status(uint8_t system_status)
{
    _impl->set_system_status(system_status);
}

} // namespace mavsdk
