#include "fake_system.h"
#include "log.h"
#include "udp_connection.h"

namespace dronecore {

FakeSystem::FakeSystem(State state) :
    _state(state) {}

FakeSystem::~FakeSystem()
{
    stop();
}

bool FakeSystem::start()
{
    auto new_conn = std::make_shared<UdpConnection>(
                        std::bind(&FakeSystem::receive_message, this, std::placeholders::_1),
                        "0.0.0.0", 12345);
    // TODO: we need to actually respond to 14540 but that's not implemented yet

    ConnectionResult ret = new_conn->start();
    return (ret == ConnectionResult::SUCCESS);
}

void FakeSystem::stop()
{
    // TODO: destruct connection here
}


void FakeSystem::receive_message(const mavlink_message_t &message)
{
    LogDebug() << "Got message with id: " << int(message.msgid);
    // TODO: do more things
}

} // namespace dronecore
