#pragma once

#include "info.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronecore {

class InfoImpl : public PluginImplBase
{
public:
    InfoImpl();
    ~InfoImpl();

    uint64_t get_uuid() const;
    bool is_complete() const;
    Info::Version get_version() const;

    void init() override;
    void deinit() override;

private:
    void set_version(Info::Version version);
    void set_uuid(uint64_t uuid);

    void process_heartbeat(const mavlink_message_t &message);
    void process_autopilot_version(const mavlink_message_t &message);

    mutable std::mutex _version_mutex {};
    Info::Version _version {};

    static void translate_binary_to_str(uint8_t *binary, unsigned binary_len,
                                        char *str, unsigned str_len);

    mutable std::mutex _uuid_mutex {};
    uint64_t _uuid = 0;
};

} // namespace dronecore
