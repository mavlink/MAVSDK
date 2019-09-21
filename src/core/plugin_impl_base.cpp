#include "system.h"
#include "global_include.h"
#include "plugin_impl_base.h"

namespace mavsdk {

PluginImplBase::PluginImplBase(System& system) : _parent(system.system_impl()) {}
PluginImplBase::PluginImplBase(std::shared_ptr<System> ptr) : _parent(ptr->system_impl()) {}
PluginImplBase::PluginImplBase(std::shared_ptr<SystemImpl> impl_ptr) : _parent(impl_ptr) {}

} // namespace mavsdk
