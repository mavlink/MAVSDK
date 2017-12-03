#include "device.h"
#include "global_include.h"
#include "plugin_impl_base.h"


namespace dronecore {

PluginImplBase::PluginImplBase() :
    _parent() {}

void PluginImplBase::set_parent(DeviceImpl *parent)
{
    _parent = parent;
}

} // namespace dronecore
