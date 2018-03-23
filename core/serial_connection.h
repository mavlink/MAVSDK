#pragma once
#if !defined(WINDOWS) && !defined(APPLE)

#include <mutex>
#include <atomic>
#include "connection.h"

namespace dronecore {

class SerialConnection : public Connection
{
public:
    explicit SerialConnection(DroneCoreImpl &parent,
                              const std::string &path,
                              int baudrate);
    bool is_ok() const;
    ConnectionResult start();
    ConnectionResult stop();
    ~SerialConnection();

    bool send_message(const mavlink_message_t &message,
                      uint8_t target_sysid,
                      uint8_t target_compid);
    // Non-copyable
    SerialConnection(const SerialConnection &) = delete;
    const SerialConnection &operator=(const SerialConnection &) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();
    static void receive(SerialConnection *parent);

    static constexpr int DEFAULT_SERIAL_BAUDRATE = 9600;
    static constexpr auto DEFAULT_SERIAL_DEV_PATH = "/dev/ttyS0";
    std::string _serial_node = {};
    int _baudrate = DEFAULT_SERIAL_BAUDRATE;

    std::mutex _mutex = {};
    int _fd = -1;
    std::thread *_recv_thread = nullptr;
    std::atomic_bool _should_exit{false};
};

} // namespace dronecore
#endif
