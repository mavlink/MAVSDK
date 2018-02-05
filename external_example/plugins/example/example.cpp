#include "example.h"
#include "example_impl.h"

namespace dronecore {

Example::Example(Device *device) :
    PluginBase()
{
    _impl = new ExampleImpl(device);
}

Example::~Example()
{
    _impl->disable();
}

void Example::say_hello() const
{
    _impl->say_hello();
}

} // namespace dronecore
