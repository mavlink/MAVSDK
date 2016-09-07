#pragma once

#include "info.h"
#include "telemetry.h"
#include "control.h"

namespace dronelink {

class DeviceImpl;

class Device
{
public:
    explicit Device(DeviceImpl *impl);
    ~Device();

    const Info &info() const;
    const Telemetry &telemetry() const;
    const Control &control() const;

private:
    // Underlying implementation, set at instantiation
    DeviceImpl *_impl;

    // Non-copyable
    Device(const Device &) = delete;
    const Device &operator=(const Device &) = delete;
};

} // namespace dronelink
