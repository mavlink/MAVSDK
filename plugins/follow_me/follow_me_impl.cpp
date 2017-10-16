#include "follow_me_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include <functional>

namespace dronecore {

FollowMeImpl::FollowMeImpl() :
    PluginImplBase()
{
}

FollowMeImpl::~FollowMeImpl()
{
}

void FollowMeImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&FollowMeImpl::process_heartbeat, this, _1), (void *)this);

}

void FollowMeImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void FollowMeImpl::say_hello() const
{
  std::cerr << "Hello world, I'm a new plugin.";
}

void FollowMeImpl::process_heartbeat(const mavlink_message_t &message)
{
    UNUSED(message);

    std::cerr << "I received a heartbeat";
}




} // namespace dronecore
