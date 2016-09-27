#pragma once

namespace dronelink {

class PluginBase
{
public:
    PluginBase() {}
    virtual ~PluginBase() = default;

    // Non-copyable
    PluginBase(const PluginBase &) = delete;
    const PluginBase &operator=(const PluginBase &) = delete;
};

} // namespace dronelink
