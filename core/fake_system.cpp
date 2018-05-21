#include "fake_system.h"
#include "log.h"

namespace dronecore {

FakeSystem::FakeSystem(State state) :
    _state(state) {}

FakeSystem::~FakeSystem()
{
}

} // namespace dronecore
