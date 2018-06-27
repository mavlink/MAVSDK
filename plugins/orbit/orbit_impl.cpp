#include "orbit_impl.h"
#include "system.h"
#include "global_include.h"

namespace dronecore {

using namespace std::placeholders; // for `_1`

OrbitImpl::OrbitImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

OrbitImpl::~OrbitImpl()
{
    _parent->unregister_plugin(this);
}

void OrbitImpl::init()
{
    _parent->register_mavlink_message_handler(MAVLINK_MSG_ID_HEARTBEAT,
                                              std::bind(&OrbitImpl::process_heartbeat, this, _1),
                                              static_cast<void *>(this));
}

void OrbitImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void OrbitImpl::enable() {}

void OrbitImpl::disable() {}

Orbit::Result OrbitImpl::start()
{
    return Orbit::Result::COMMAND_DENIED;
}

Orbit::Result OrbitImpl::stop()
{
    return Orbit::Result::COMMAND_DENIED;
}

void OrbitImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    UNUSED(message);
    UNUSED(heartbeat);
}

} // namespace dronecore
