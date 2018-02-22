#include <gmock/gmock.h>

#include "connection_result.h"

namespace dronecore {
namespace testing {

typedef std::function<void(uint64_t uuid)> event_callback_t;

class MockDroneCore
{
public:
    MOCK_CONST_METHOD1(add_udp_connection, ConnectionResult(int local_port_number));
    MOCK_CONST_METHOD1(register_on_discover, void(event_callback_t));
    MOCK_CONST_METHOD1(register_on_timeout, void(event_callback_t));
};

} // testing
} // dronecore
