#pragma once

#include "info.h"
#include "mavlink_include.h"
#include <cstdint>

namespace dronelink {

class DeviceImpl;

class InfoImpl {
public:
    explicit InfoImpl(DeviceImpl *parent);
    ~InfoImpl();

    void init();
    void deinit();

    bool is_complete() const;
    unsigned get_version() const;
    uint64_t get_uuid() const;

    // Non-copyable
    InfoImpl(const InfoImpl &) = delete;
    const InfoImpl &operator=(const InfoImpl &) = delete;

private:
    void set_version(unsigned version);
    void set_uuid(uint64_t uuid);

    void process_autopilot_version(const mavlink_message_t &message);

    DeviceImpl *_parent;

    unsigned _version;
    uint64_t _uuid;
};

} // namespace dronelink
