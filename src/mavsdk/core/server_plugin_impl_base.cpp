#include "server_plugin_impl_base.h"
#include "server_component.h"

namespace mavsdk {

ServerPluginImplBase::ServerPluginImplBase(std::shared_ptr<ServerComponent> server_component) :
    _server_component_impl(server_component->_impl)
{}

} // namespace mavsdk