#include "device.h"
#include "device_impl.h"

namespace dronelink {

Device::Device(DeviceImpl *impl) :
    DevicePluginContainer(impl)
{
}

Device::~Device()
{
}

} // namespace dronelink
