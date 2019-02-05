#include <functional>
#include "mavlink_backdoor_impl.h"
#include "system.h"
#include "global_include.h"

namespace dronecode_sdk {

MavlinkBackdoorImpl::MavlinkBackdoorImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MavlinkBackdoorImpl::~MavlinkBackdoorImpl()
{
    _parent->unregister_plugin(this);
}

void MavlinkBackdoorImpl::init() {}

void MavlinkBackdoorImpl::deinit() {}

void MavlinkBackdoorImpl::enable() {}

void MavlinkBackdoorImpl::disable() {}

MavlinkBackdoor::Result MavlinkBackdoorImpl::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    return MavlinkBackdoor::Result::UNKNOWN;
}

void MavlinkBackdoorImpl::subscribe_message_async(
    std::function<void(const mavlink_message_t &)> callback)
{
    UNUSED(callback);
}

} // namespace dronecode_sdk
