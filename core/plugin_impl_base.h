#pragma once

namespace dronelink {

class DeviceImpl;

class PluginImplBase
{
public:
    PluginImplBase();
    virtual ~PluginImplBase() = default;

    void set_parent(DeviceImpl *parent);
    virtual void init() = 0;
    virtual void deinit() = 0;

    // Non-copyable
    PluginImplBase(const PluginImplBase &) = delete;
    const PluginImplBase &operator=(const PluginImplBase &) = delete;

protected:
    DeviceImpl *_parent;
};

} // namespace dronelink
