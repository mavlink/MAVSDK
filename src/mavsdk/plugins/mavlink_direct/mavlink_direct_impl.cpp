#include "mavlink_direct_impl.h"

namespace mavsdk {

MavlinkDirectImpl::MavlinkDirectImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

MavlinkDirectImpl::MavlinkDirectImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

MavlinkDirectImpl::~MavlinkDirectImpl()
{
    _system_impl->unregister_plugin(this);
}

void MavlinkDirectImpl::init() {}

void MavlinkDirectImpl::deinit() {}

void MavlinkDirectImpl::enable() {}

void MavlinkDirectImpl::disable() {}

MavlinkDirect::Result MavlinkDirectImpl::send_message(MavlinkDirect::MavlinkMessage message)
{
    UNUSED(message);

    // TODO :)
    return {};
}

MavlinkDirect::MessageHandle
MavlinkDirectImpl::subscribe_message(const MavlinkDirect::MessageCallback& callback)
{
    UNUSED(callback);
}

void MavlinkDirectImpl::unsubscribe_message(MavlinkDirect::MessageHandle handle)
{
    UNUSED(handle);
}

MavlinkDirect::MessageTypeHandle MavlinkDirectImpl::subscribe_message_type(
    MavlinkDirect::uint32_t message_id, const MavlinkDirect::MessageTypeCallback& callback)
{
    UNUSED(message_id);

    UNUSED(callback);
}

void MavlinkDirectImpl::unsubscribe_message_type(MavlinkDirect::MessageTypeHandle handle)
{
    UNUSED(handle);
}

} // namespace mavsdk