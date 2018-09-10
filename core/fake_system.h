#pragma once

#include "system.h"
#include "mavlink_include.h"
#include "udp_connection.h"
#include <thread>
#include <atomic>

namespace dronecode_sdk {

class FakeSystem {
public:
    enum class State { DEAD, LOYAL, LAZY, REFUSING };

    FakeSystem(State state);
    ~FakeSystem();

    bool start();
    void stop();

private:
    State _state = State::DEAD;
    bool _landed = true;

    std::shared_ptr<std::thread> _thread{};
    std::atomic<bool> _should_exit{false};
    std::shared_ptr<UdpConnection> _connection{};

    void receive_message(const mavlink_message_t &message);
    void run();
    void send_heartbeat();
    void send_extended_sys_state();
    void respond_to_command_int(const mavlink_message_t &message);
    void respond_to_command_long(const mavlink_message_t &message);
    void send_command_ack(uint16_t command);
};

} // namespace dronecode_sdk
