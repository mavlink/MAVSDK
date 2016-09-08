#pragma once

#include "plugin_base.h"
#include "error_handling.h"

namespace dronelink {

class ControlImpl;

class Control : public PluginBase
{
public:
    explicit Control(ControlImpl *impl);
    ~Control();

    Result arm() const;
    Result disarm() const;
    Result takeoff() const;
    Result land() const;

    // Non-copyable
    Control(const Control &) = delete;
    const Control &operator=(const Control &) = delete;

private:
    // Underlying implementation, set at instantiation
    ControlImpl *_impl;
};

} // namespace dronelink
