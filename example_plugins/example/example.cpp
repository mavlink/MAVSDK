#include "example.h"
#include "example_impl.h"

namespace dronelink {

Example::Example(ExampleImpl *impl) :
    PluginBase(),
    _impl(impl)
{
}

Example::~Example()
{
}

void Example::say_hello() const
{
    _impl->say_hello();
}

} // namespace dronelink
