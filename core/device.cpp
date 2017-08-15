#include "device.h"
#include "device_impl.h"

namespace dronecore {

Device::Device(DeviceImpl *impl) :
    DevicePluginContainer(impl)
{
}

Device::~Device()
{
}

} // namespace dronecore
