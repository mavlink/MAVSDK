#pragma once

#include <mutex>
#include <atomic>
#include "connection.h"

#if defined(WINDOWS)
#include <windows.h>
#endif

namespace dronecore {

class SerialConnection : public Connection
{
public:
    explicit SerialConnection(DroneCoreImpl &parent,
                              const std::string &path,
                              int baudrate);
    ConnectionResult start();
    ConnectionResult stop();
    ~SerialConnection();

    bool send_message(const mavlink_message_t &message);

    // Non-copyable
    SerialConnection(const SerialConnection &) = delete;
    const SerialConnection &operator=(const SerialConnection &) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();
    void receive();

    std::string _serial_node;
    int _baudrate;

    std::mutex _mutex = {};
#if !defined(WINDOWS)
    int _fd = -1;
#else
    HANDLE _handle;
#endif
    std::thread *_recv_thread = nullptr;
    std::atomic_bool _should_exit{false};
};

} // namespace dronecore
