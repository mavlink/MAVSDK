#pragma once

namespace dronelink {

class InfoImpl;

class Info
{
public:
    explicit Info(InfoImpl *impl);
    ~Info();

    bool is_complete() const;
    unsigned get_version() const;

    // Non-copyable
    Info(const Info &) = delete;
    const Info &operator=(const Info &) = delete;

private:
    // Underlying implementation, set at instantiation
    InfoImpl *_impl;
};

} // namespace dronelink
