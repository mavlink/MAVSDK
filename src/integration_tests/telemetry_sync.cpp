#include <iostream>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

TEST(SitlTest, PX4TelemetrySync)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    ASSERT_TRUE(system->is_connected());

    auto telemetry = std::make_shared<Telemetry>(system);

    EXPECT_EQ(telemetry->set_rate_position(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_home(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_in_air(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_attitude_quaternion(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_attitude_euler(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_velocity_ned(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_gps_info(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_battery(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_actuator_control_target(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_fixedwing_metrics(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_ground_truth(10.0), Telemetry::Result::Success);
    EXPECT_EQ(telemetry->set_rate_altitude(10.0), Telemetry::Result::Success);

    for (unsigned i = 0; i < 10; ++i) {
        std::cout << "Position: " << telemetry->position() << '\n';
        std::cout << "Home Position: " << telemetry->home() << '\n';
        std::cout << "Attitude: " << telemetry->attitude_quaternion() << '\n';
        std::cout << "Attitude: " << telemetry->attitude_euler() << '\n';
        std::cout << "Angular velocity: " << telemetry->attitude_angular_velocity_body() << '\n';
        std::cout << "Fixed wing metrics: " << telemetry->fixedwing_metrics() << '\n';
        std::cout << "Ground Truth: " << telemetry->ground_truth() << '\n';
        std::cout << "Velocity: " << telemetry->velocity_ned() << '\n';
        std::cout << "GPS Info: " << telemetry->gps_info() << '\n';
        std::cout << "Battery: " << telemetry->battery() << '\n';
        std::cout << "Actuators: " << telemetry->actuator_control_target() << '\n';
        std::cout << "Flight mode: " << telemetry->flight_mode() << '\n';
        std::cout << "Landed state: " << telemetry->landed_state()
                  << "(in air: " << telemetry->in_air() << ")" << '\n';
        std::cout << "Altitude: " << telemetry->altitude() << '\n';

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
