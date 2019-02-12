#include "dronecode_sdk.h"

#include "dronecode_sdk_impl.h"
#include "global_include.h"

namespace dronecode_sdk {

DronecodeSDK::DronecodeSDK() : _impl{new DronecodeSDKImpl()} {}

DronecodeSDK::~DronecodeSDK() {}

std::vector<uint64_t> DronecodeSDK::system_uuids() const
{
    return _impl->get_system_uuids();
}

System &DronecodeSDK::system() const
{
    return _impl->get_system();
}

System &DronecodeSDK::system(const uint64_t uuid) const
{
    return _impl->get_system(uuid);
}

bool DronecodeSDK::is_connected() const
{
    return _impl->is_connected();
}

bool DronecodeSDK::is_connected(const uint64_t uuid) const
{
    return _impl->is_connected(uuid);
}

void DronecodeSDK::register_on_discover(const event_callback_t callback)
{
    _impl->register_on_discover(callback);
}

void DronecodeSDK::register_on_timeout(const event_callback_t callback)
{
    _impl->register_on_timeout(callback);
}

} // namespace dronecode_sdk
