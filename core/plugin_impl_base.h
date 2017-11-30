#pragma once

namespace dronecore {

class DeviceImpl;

class PluginImplBase
{
public:
    PluginImplBase();
    virtual ~PluginImplBase() = default;

    void set_parent(DeviceImpl *parent);
    virtual void init() = 0;
    virtual void deinit() = 0;

    // TODO: make this pure virtual after a while.
    virtual void enable();

    // TODO: make this pure virtual after a while.
    virtual void disable();

    // Non-copyable
    PluginImplBase(const PluginImplBase &) = delete;
    const PluginImplBase &operator=(const PluginImplBase &) = delete;

protected:
    DeviceImpl *_parent;
};

} // namespace dronecore
