#include <iostream>
#include <future>
#include "log.h"
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/mavlink_backdoor/mavlink_backdoor.h"

using namespace dronecode_sdk;

static void receive_highres_imu(/* const mavlink_message_t& message */);

TEST_F(SitlTest, MavlinkBackdoor)
{
    DronecodeSDK dc;
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        dc.register_on_discover([&prom](uint64_t uuid) {
            prom.set_value();
            UNUSED(uuid);
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    System &system = dc.system();
    auto mavlink_backdoor = std::make_shared<MavlinkBackdoor>(system);

    mavlink_backdoor->send_message();

    mavlink_backdoor->subscribe_message_async(
        std::bind(&receive_highres_imu/*, std::placeholders::_1)*/));
}

void receive_highres_imu(/* const mavlink_message_t& message */)
{

}
