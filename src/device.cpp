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

const Control &Device::control() const
{
    return _impl->get_control();
}

Device::~Device()
{
    // The implementation instance needs to be deleted separately.
    _impl = nullptr;
}

} // namespace dronelink
