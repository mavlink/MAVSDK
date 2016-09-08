#pragma once

#include "example.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"

namespace dronelink {

class ExampleImpl : public PluginImplBase {
public:
    ExampleImpl();
    ~ExampleImpl();

    void say_hello() const;

    void init() override;
    void deinit() override;

private:
    void process_heartbeat(const mavlink_message_t &message);
};

} // namespace dronelink
