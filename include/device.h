#pragma once

#include "info.h"
#include "telemetry.h"
#include "error_handling.h"

namespace dronelink {

class DeviceImpl;

class Device
{
public:
    explicit Device(DeviceImpl *impl);
    ~Device();

    Result arm();
    Result disarm();
    Result takeoff();
    Result land();

    const Info &info() const;
    const Telemetry &telemetry() const;


private:
    // Underlying implementation, set at instantiation
    DeviceImpl *_impl;

    // Non-copyable
    Device(const Device &) = delete;
    const Device &operator=(const Device &) = delete;
};

} // namespace dronelink
