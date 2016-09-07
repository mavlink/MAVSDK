#pragma once

#include "error_handling.h"

namespace dronelink {

class ControlImpl;

class Control
{
public:
    Control(ControlImpl *impl);
    ~Control();

    Result arm() const;
    Result disarm() const;
    Result takeoff() const;
    Result land() const;

private:
    // Underlying implementation, set at instantiation
    ControlImpl *_impl;

    // Non-copyable
    Control(const Control &) = delete;
    const Control &operator=(const Control &) = delete;
};

} // namespace dronelink
