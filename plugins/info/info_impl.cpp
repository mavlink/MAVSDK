#include "info_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include <functional>

namespace dronelink {

InfoImpl::InfoImpl() :
    PluginImplBase(),
    _version(0)
{
}

InfoImpl::~InfoImpl()
{
}

void InfoImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&InfoImpl::process_heartbeat, this, _1), (void *)this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&InfoImpl::process_autopilot_version, this, _1), (void *)this);

}

void InfoImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void InfoImpl::process_heartbeat(const mavlink_message_t &message)
{
    UNUSED(message);

    if (!is_complete()) {
        // We try to request more info if not all info is available.
        // We can't rely on DeviceImpl to request the autopilot_version,
        // so we do it here, anyway.
        _parent->request_autopilot_version();
    }
}

void InfoImpl::process_autopilot_version(const mavlink_message_t &message)
{
    mavlink_autopilot_version_t autopilot_version;

    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    // TODO: actually use message
    UNUSED(autopilot_version);

    // TODO: remove hardcoded version
    set_version(1);
}

bool InfoImpl::is_complete() const
{
    if (_version == 0) {
        return false;
    }

    return true;
}

unsigned InfoImpl::get_version() const
{
    return _version;
}

void InfoImpl::set_version(unsigned version)
{
    _version = version;
}


} // namespace dronelink
