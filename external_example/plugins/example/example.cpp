#include "example.h"
#include "example_impl.h"

namespace dronecore {

Example::Example(ExampleImpl *impl) :
    _impl(impl)
{
    _impl = new ExternalExampleImpl(device);
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
