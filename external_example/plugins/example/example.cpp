#include "example.h"
#include "example_impl.h"

namespace dronecore {

Example::Example(ExampleImpl *impl) :
    _impl(impl)
{
    _impl = new ExternalExampleImpl(device);
    _impl->init();
    _impl->enable();
}

Example::~Example()
{
    _impl->disable();
    _impl->deinit();
    delete _impl;
}

void Example::say_hello() const
{
    _impl->say_hello();
}

} // namespace dronecore
