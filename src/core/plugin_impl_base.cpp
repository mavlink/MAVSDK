#include "system.h"
#include "global_include.h"
#include "plugin_impl_base.h"

namespace dronecode_sdk {

PluginImplBase::PluginImplBase(System &system) : _parent(system.system_impl()) {}

} // namespace dronecode_sdk
