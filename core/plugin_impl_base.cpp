#include "system.h"
#include "global_include.h"
#include "plugin_impl_base.h"

namespace dronecore {

PluginImplBase::PluginImplBase(System &system) : _parent(system.system_impl()) {}

} // namespace dronecore
