#include "server_plugin_impl_base.hpp"
#include "server_component.hpp"

namespace mavsdk {

ServerPluginImplBase::ServerPluginImplBase(std::shared_ptr<ServerComponent> server_component) :
    _server_component_impl(server_component->_impl)
{}

} // namespace mavsdk