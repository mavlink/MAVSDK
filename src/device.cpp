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

void Device::set_timeout(double timeout_s)
{
    _impl->set_timeout(timeout_s);
}

double Device::get_timeout() const
{
    return _impl->get_timeout();
}

} // namespace dronelink
