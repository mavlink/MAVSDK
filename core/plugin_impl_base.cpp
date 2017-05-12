#include "device_impl.h"
#include "global_include.h"
#include "plugin_impl_base.h"


namespace dronelink {

PluginImplBase::PluginImplBase() :
    _parent()
{
}

void PluginImplBase::set_parent(DeviceImpl *parent)
{
    _parent = parent;
}

} // namespace dronelink
