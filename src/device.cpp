#include "device.h"
#include "device_impl.h"

namespace dronelink {

Device::Device(DeviceImpl *impl) :
    _impl(impl)
{
}

const Info &Device::info() const
{
    return _impl->get_info();
}

const Telemetry &Device::telemetry() const
{
    return _impl->get_telemetry();
}

Device::~Device()
{
    // The implementation instance needs to be deleted separately.
    _impl = nullptr;
}

Result Device::arm()
{
    return _impl->arm();
}

Result Device::disarm()
{
    return _impl->disarm();
}

Result Device::takeoff()
{
    return _impl->takeoff();
}

Result Device::land()
{
    return _impl->land();
}

} // namespace dronelink
