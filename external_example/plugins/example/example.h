#pragma once

#include "plugin_base.h"

namespace dronecore {

class ExampleImpl;

class Example : public PluginBase
{
public:
    explicit Example(ExampleImpl *impl);
    ~Example();

    void say_hello() const;

    // Non-copyable
    Example(const Example &) = delete;
    const Example &operator=(const Example &) = delete;

private:
    // Underlying implementation, set at instantiation
    ExampleImpl *_impl;
};

} // namespace dronecore
