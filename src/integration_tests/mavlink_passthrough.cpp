#include <iostream>
#include <future>
#include "log.h"
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"

using namespace mavsdk;

TEST_F(SitlTest, MavlinkPassthrough)
{
    Mavsdk dc;
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::Success);

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

    System& system = dc.system();
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);

    {
        mavlink_message_t message;
        mavlink_msg_command_long_pack(
            mavlink_passthrough->get_our_sysid(),
            mavlink_passthrough->get_our_compid(),
            &message,
            mavlink_passthrough->get_target_sysid(),
            mavlink_passthrough->get_target_compid(),
            MAV_CMD_SET_MESSAGE_INTERVAL,
            0, // first transmission
            float(MAVLINK_MSG_ID_HIGHRES_IMU),
            2000.0f, // 50 Hz
            NAN,
            NAN,
            NAN,
            NAN,
            NAN);
        mavlink_passthrough->send_message(message);
    }

    {
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        unsigned counter = 0;
        bool stopped = false;

        mavlink_passthrough->subscribe_message_async(
            MAVLINK_MSG_ID_HIGHRES_IMU,
            [&prom, &counter, &stopped, mavlink_passthrough](const mavlink_message_t& message) {
                mavlink_highres_imu_t highres_imu;
                mavlink_msg_highres_imu_decode(&message, &highres_imu);

                LogInfo() << "HIGHRES_IMU.temperature [1] (" << counter << ")"
                          << highres_imu.temperature << " degrees C";
                if (++counter == 100) {
                    EXPECT_FALSE(stopped);
                    if (!stopped) {
                        stopped = true;
                        // Unsubscribe again
                        mavlink_passthrough->subscribe_message_async(
                            MAVLINK_MSG_ID_HIGHRES_IMU, nullptr);
                        prom.set_value();
                    }
                };
            });

        // At 50 Hz we should have received 100 temperature measurements in 2 seconds.
        // After 3 seconds we give up.
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    }
}
