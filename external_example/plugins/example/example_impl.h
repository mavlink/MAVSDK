#pragma once

#include "example.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace dronecore {

class ExampleImpl : public PluginImplBase
{
public:
    ExampleImpl(System &system);
    ~ExampleImpl();

    void say_hello() const;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

private:
    void process_heartbeat(const mavlink_message_t &message);
};

} // namespace dronecore
