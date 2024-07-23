#pragma once

#if defined(WINDOWS)
#include <winsock2.h>
#endif

namespace mavsdk {

class SocketHolder {
public:
    SocketHolder() noexcept = default;
    explicit SocketHolder(int socket_fd) noexcept;

    SocketHolder(SocketHolder&&) noexcept = default;
    SocketHolder& operator=(SocketHolder&&) noexcept = default;

    ~SocketHolder() noexcept;

    void reset(int fd) noexcept;
    void close() noexcept;

    bool empty() const noexcept;
    int get() const noexcept;

#if defined(WINDOWS)
    static constexpr int invalid_socket_fd = INVALID_SOCKET;
#else
    static constexpr int invalid_socket_fd = -1;
#endif

private:
    SocketHolder(const SocketHolder&) = delete;
    SocketHolder& operator=(const SocketHolder&) = delete;

    int _fd = invalid_socket_fd;
};
    
} // namespace mavsdk
