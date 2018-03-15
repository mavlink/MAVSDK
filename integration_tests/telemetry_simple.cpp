#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecore;

TEST_F(SitlTest, TelemetrySimple)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();

    auto telemetry = std::make_shared<Telemetry>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Telemetry::Result result = telemetry->set_rate_position(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = telemetry->set_rate_home_position(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = telemetry->set_rate_in_air(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = telemetry->set_rate_attitude(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = telemetry->set_rate_ground_speed_ned(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = telemetry->set_rate_gps_info(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = telemetry->set_rate_battery(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);

    // Print 3s of telemetry.
    for (unsigned i = 0; i < 50; ++i) {

        const Telemetry::Position &position = telemetry->position();
        std::cout << "Position: " << std::endl
                  << "Absolute altitude: " << position.absolute_altitude_m << " m" << std::endl
                  << "Relative altitude: " << position.relative_altitude_m << " m" << std::endl
                  << "Latitude: "
                  << position.latitude_deg << " deg"
                  << ", longitude: "
                  << position.longitude_deg << " deg" << std::endl;

        const Telemetry::Position &home_position = telemetry->home_position();
        std::cout << "Home position: " << std::endl
                  << "Absolute altitude: " << home_position.absolute_altitude_m
                  << " m" << std::endl
                  << "Relative altitude: " << home_position.relative_altitude_m
                  << " m" << std::endl
                  << "Latitude: "
                  << home_position.latitude_deg << " deg"
                  << ", longitude: "
                  << home_position.longitude_deg << " deg" << std::endl;

        std::cout << (telemetry->in_air() ? "In-air" : "On-ground")
                  << std::endl;

        const Telemetry::Quaternion &quaternion
            = telemetry->attitude_quaternion();
        std::cout << "Quaternion: ("
                  << quaternion.w << ", "
                  << quaternion.x << ","
                  << quaternion.y << ","
                  << quaternion.z << ")" << std::endl;

        const Telemetry::EulerAngle &euler_angle
            = telemetry->attitude_euler_angle();
        std::cout << "Euler: (" << euler_angle.roll_deg << " deg, "
                  << euler_angle.pitch_deg << " deg,"
                  << euler_angle.yaw_deg << " deg)" << std::endl;

        const Telemetry::GroundSpeedNED &ground_speed_ned
            = telemetry->ground_speed_ned();
        std::cout << "Speed: (" << ground_speed_ned.velocity_north_m_s << " m/s, "
                  << ground_speed_ned.velocity_east_m_s << " m/s,"
                  << ground_speed_ned.velocity_down_m_s << " m/s)" << std::endl;

        const Telemetry::GPSInfo &gps_info = telemetry->gps_info();
        std::cout << "GPS sats: " << gps_info.num_satellites
                  << ", fix type: " << gps_info.fix_type << std::endl;

        const Telemetry::Battery &battery = telemetry->battery();
        std::cout << "Battery voltage: " << battery.voltage_v << " v, "
                  << "remaining: " << battery.remaining_percent * 100.0f << " %" << std::endl;


        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}
