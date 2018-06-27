#pragma once

#include "orbit.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "timeout_handler.h"
#include "global_include.h"
#include "log.h"

namespace dronecore {

class OrbitImpl : public PluginImplBase {
public:
    OrbitImpl(System &system);
    ~OrbitImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Orbit::Result start();
    Orbit::Result stop();

private:
    void process_heartbeat(const mavlink_message_t &message);
};

} // namespace dronecore
