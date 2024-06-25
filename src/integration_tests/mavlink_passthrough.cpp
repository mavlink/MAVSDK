#include <iostream>
#include <future>
#include "log.h"
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"

using namespace mavsdk;

TEST(SitlTest, PX4MavlinkPassthrough)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("udpin://0.0.0.0:14540"), ConnectionResult::Success);

    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                prom.set_value();
            }
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);

    {
        mavlink_passthrough->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_command_long_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
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
            return message;
        });
    }

    {
        auto counter = std::make_shared<unsigned>(0);

        mavlink_passthrough->subscribe_message(
            MAVLINK_MSG_ID_HIGHRES_IMU, [counter](const mavlink_message_t& message) {
                mavlink_highres_imu_t highres_imu;
                mavlink_msg_highres_imu_decode(&message, &highres_imu);

                LogInfo() << "HIGHRES_IMU.temperature [1] (" << counter << ")"
                          << highres_imu.temperature << " degrees C";
                ++(*counter);
            });

        std::this_thread::sleep_for(std::chrono::seconds(3));
        EXPECT_GT(*counter, 100);
    }
}
