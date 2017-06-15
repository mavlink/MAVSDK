#pragma once

#include "info.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronelink {

class InfoImpl : public PluginImplBase
{
public:
    InfoImpl();
    ~InfoImpl();

    bool is_complete() const;
    unsigned get_version() const;

    void init() override;
    void deinit() override;

private:
    void set_version(unsigned version);

    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);

    mutable std::mutex _version_mutex;
    unsigned _version = 0;
};

} // namespace dronelink
