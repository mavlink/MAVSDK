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

private:
    State _state;
};

} // namespace dronecore
