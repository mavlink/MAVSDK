#include "device.h"
#include "global_include.h"
#include "plugin_impl_base.h"


namespace dronecore {

PluginImplBase::PluginImplBase(Device &device) :
    _parent(device) {}

} // namespace dronecore
