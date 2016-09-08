#pragma once

#include "plugin_impl_base.h"
#include "error_handling.h"

namespace dronelink {

class ControlImpl : public PluginImplBase
{
public:
    ControlImpl();
    ~ControlImpl();

    void init() override;
    void deinit() override;

    Result arm() const;
    Result disarm() const;
    Result takeoff() const;
    Result land() const;

private:
};

} // namespace dronelink
