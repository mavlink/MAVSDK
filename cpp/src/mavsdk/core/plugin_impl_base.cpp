#include "system.hpp"
#include "plugin_impl_base.hpp"

namespace mavsdk {

PluginImplBase::PluginImplBase(System& system) : _system_impl(system.system_impl()) {}
PluginImplBase::PluginImplBase(std::shared_ptr<System> system) : _system_impl(system->system_impl())
{}

} // namespace mavsdk
