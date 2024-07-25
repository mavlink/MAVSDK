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
        shutdown(_fd, SD_BOTH);
        closesocket(_fd);
        WSACleanup();
#else
        // This should interrupt a recv/recvfrom call.
        shutdown(_fd, SHUT_RDWR);

        // But on Mac, closing is also needed to stop blocking recv/recvfrom.
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
