#include "example.h"
#include "example_impl.h"

namespace dronecore {

Example::Example(ExampleImpl *impl) :
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

} // namespace dronecore
