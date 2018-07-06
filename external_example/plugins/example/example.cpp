#include "example.h"
#include "example_impl.h"

namespace dronecode_sdk {

Example::Example(System &system) : PluginBase()
{
    _impl = new ExampleImpl(system);
}

Example::~Example()
{
    _impl->disable();
}

void Example::say_hello() const
{
    _impl->say_hello();
}

} // namespace dronecode_sdk
