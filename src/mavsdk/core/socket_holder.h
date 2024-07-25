#pragma once

#if defined(WINDOWS)
#include <winsock2.h>
#endif

namespace mavsdk {

class SocketHolder {
public:
#if defined(WINDOWS)
    using DescriptorType = SOCKET;
    static constexpr DescriptorType invalid_socket_fd = INVALID_SOCKET;
#else
    using DescriptorType = int;
    static constexpr DescriptorType invalid_socket_fd = -1;
#endif

    SocketHolder() noexcept = default;
    explicit SocketHolder(DescriptorType socket_fd) noexcept;

    ~SocketHolder() noexcept;

    void reset(DescriptorType fd) noexcept;
    void close() noexcept;

    bool empty() const noexcept;
    DescriptorType get() const noexcept;

private:
    SocketHolder(const SocketHolder&) = delete;
    SocketHolder& operator=(const SocketHolder&) = delete;

    DescriptorType _fd = invalid_socket_fd;
};

} // namespace mavsdk
