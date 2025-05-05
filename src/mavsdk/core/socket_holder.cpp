#include "socket_holder.h"

#ifndef WINDOWS
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace mavsdk {

SocketHolder::SocketHolder(DescriptorType fd) noexcept : _fd{fd} {}

SocketHolder::~SocketHolder() noexcept
{
    close();
}

void SocketHolder::reset(DescriptorType fd) noexcept
{
    if (_fd != fd) {
        close();
        _fd = fd;
    }
}

void SocketHolder::close() noexcept
{
    if (!empty()) {
#if defined(WINDOWS)
        closesocket(_fd);
        WSACleanup();
#else
        ::close(_fd);
#endif
        _fd = invalid_socket_fd;
    }
}

bool SocketHolder::empty() const noexcept
{
    return _fd == invalid_socket_fd;
}

SocketHolder::DescriptorType SocketHolder::get() const noexcept
{
    return _fd;
}

} // namespace mavsdk
