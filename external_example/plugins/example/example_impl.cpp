#include "example_impl.h"
#include "mavlink_system.h"
#include "global_include.h"
#include <functional>

namespace dronecore {

ExampleImpl::ExampleImpl(System &system) :
    PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ExampleImpl::~ExampleImpl()
{
    _parent->register_plugin(this);
}

void ExampleImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&ExampleImpl::process_heartbeat, this, _1), this);
}

void ExampleImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void ExampleImpl::enable() {}

void ExampleImpl::disable() {}

void ExampleImpl::say_hello() const
{
    LogInfo() << "Hello world, I'm a new plugin.";
}

void ExampleImpl::process_heartbeat(const mavlink_message_t &message)
{
    UNUSED(message);

    LogDebug() << "I received a heartbeat";
}

} // namespace dronecore
