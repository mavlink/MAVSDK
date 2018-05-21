#pragma once

namespace dronecore {

class FakeSystem
{
public:
    enum class State {
        DEAD,
        LOYAL,
        LAZY,
        REFUSING
    };

    FakeSystem(State state);
    ~FakeSystem();

    bool start();
    void stop();

private:
    State _state;

    void receive_message(const mavlink_message_t &message);
};

} // namespace dronecore
