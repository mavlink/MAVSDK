#include "example_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include <functional>

namespace dronelink {

ExampleImpl::ExampleImpl() :
    PluginImplBase()
{
}

ExampleImpl::~ExampleImpl()
{
}

void ExampleImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&ExampleImpl::process_heartbeat, this, _1), (void *)this);

}

void ExampleImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void ExampleImpl::say_hello() const
{
    Debug() << "Hello world, I'm a new plugin.";
}

void ExampleImpl::process_heartbeat(const mavlink_message_t &message)
{
    UNUSED(message);

    Debug() << "I received a heartbeat";
}




} // namespace dronelink
