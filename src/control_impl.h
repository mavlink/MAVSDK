#pragma once

#include "error_handling.h"

namespace dronelink {

class DeviceImpl;

class ControlImpl
{
public:
    explicit ControlImpl(DeviceImpl *parent);
    ~ControlImpl();

    Result arm() const;
    Result disarm() const;
    Result takeoff() const;
    Result land() const;

    // Non-copyable
    ControlImpl(const ControlImpl &) = delete;
    const ControlImpl &operator=(const ControlImpl &) = delete;
private:
    DeviceImpl *_parent;
};

} // namespace dronelink
