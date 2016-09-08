#include "info_impl.h"
#include "device_impl.h"
#include <functional>

namespace dronelink {

InfoImpl::InfoImpl(DeviceImpl *parent) :
    _parent(parent),
    _version(0),
    _uuid(0)
{
}

InfoImpl::~InfoImpl()
{
}

void InfoImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&InfoImpl::process_autopilot_version, this, _1), (void *)this);
}

void InfoImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void InfoImpl::process_autopilot_version(const mavlink_message_t &message)
{
    mavlink_autopilot_version_t autopilot_version;

    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    // TODO: remove hardcoded version
    set_version(1);
    set_uuid(autopilot_version.uid);
}

bool InfoImpl::is_complete() const
{
    if (_version == 0) {
        return false;
    }
    else if (_uuid == 0) {
        return false;
    }

    return true;
}

unsigned InfoImpl::get_version() const
{
    return _version;
}

uint64_t InfoImpl::get_uuid() const
{
    return _uuid;
}

void InfoImpl::set_version(unsigned version)
{
    _version = version;
}

void InfoImpl::set_uuid(uint64_t uuid)
{
    _uuid = uuid;
}


} // namespace dronelink
