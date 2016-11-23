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
    Info::Version get_version() const;

    void init() override;
    void deinit() override;

private:
    void set_version(Info::Version version);

    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);

    mutable std::mutex _version_mutex;
    Info::Version _version = {};

    static void translate_binary_to_str(uint8_t *binary, unsigned binary_len,
                                        char *str, unsigned str_len);
};

} // namespace dronelink
