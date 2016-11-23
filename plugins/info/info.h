#pragma once

#include <cstdint>

namespace dronelink {

class InfoImpl;

class Info
{
public:
    explicit Info(InfoImpl *impl);
    ~Info();

    // 16 chars + terminating 0
    static const unsigned GIT_HASH_STR_LEN = 17;

    struct Version {
        int flight_sw_major;
        int flight_sw_minor;
        int flight_sw_patch;
        char flight_sw_git_hash[GIT_HASH_STR_LEN];
        int os_sw_major;
        int os_sw_minor;
        int os_sw_patch;
        char os_sw_git_hash[GIT_HASH_STR_LEN];
        uint16_t vendor_id;
        uint16_t product_id;
    };

    bool is_complete() const;
    Version get_version() const;

    // Non-copyable
    Info(const Info &) = delete;
    const Info &operator=(const Info &) = delete;

private:
    // Underlying implementation, set at instantiation
    InfoImpl *_impl;
};

} // namespace dronelink
