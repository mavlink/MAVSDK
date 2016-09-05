#pragma once

#include <stdint.h>

namespace dronelink {

class InfoImpl;

class Info
{
public:
    Info(InfoImpl *impl);
    ~Info();

    bool is_complete() const;
    unsigned get_version() const;
    uint64_t get_uuid() const;

private:
    // Underlying implementation, set at instantiation
    InfoImpl *_impl;

    // Non-copyable
    Info(const Info &) = delete;
    const Info &operator=(const Info &) = delete;
};

} // namespace dronelink
