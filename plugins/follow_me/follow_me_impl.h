#pragma once

#include "follow_me.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"

namespace dronecore {

class FollowMeImpl : public PluginImplBase
{
public:
    FollowMeImpl();
    ~FollowMeImpl();

    void say_hello() const;

    void init() override;
    void deinit() override;

private:
    void process_heartbeat(const mavlink_message_t &message);
};

} // namespace dronecore
