#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

void test_simple_telemetry()
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();

    usleep(1500000);

    std::vector<uint64_t> uuids = dl.device_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found device with UUID: " << *it << std::endl;
    }

    ASSERT_EQ(uuids.size(), 1);

    uint64_t uuid = uuids.at(0);

    if (ret != DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }

    Device &device = dl.device(uuid);


    Telemetry::Result result = device.telemetry().set_rate_position(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = device.telemetry().set_rate_home_position(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = device.telemetry().set_rate_in_air(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = device.telemetry().set_rate_attitude(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = device.telemetry().set_rate_ground_speed_ned(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = device.telemetry().set_rate_gps_info(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);
    result = device.telemetry().set_rate_battery(10.0);
    ASSERT_EQ(result, Telemetry::Result::SUCCESS);

    // Print 3s of telemetry.
    for (unsigned i = 0; i < 500; ++i) {

        const Telemetry::Position &position = device.telemetry().position();
        std::cout << "Position: " << std::endl
                  << "Absolute altitude: " << position.absolute_altitude_m << " m" << std::endl
                  << "Relative altitude: " << position.relative_altitude_m << " m" << std::endl
                  << "Latitude: "
                  << position.latitude_deg << " deg"
                  << ", longitude: "
                  << position.longitude_deg << " deg" << std::endl;

        const Telemetry::Position &home_position = device.telemetry().home_position();
        std::cout << "Home position: " << std::endl
                  << "Absolute altitude: " << home_position.absolute_altitude_m
                  << " m" << std::endl
                  << "Relative altitude: " << home_position.relative_altitude_m
                  << " m" << std::endl
                  << "Latitude: "
                  << home_position.latitude_deg << " deg"
                  << ", longitude: "
                  << home_position.longitude_deg << " deg" << std::endl;

        std::cout << (device.telemetry().in_air() ? "In-air" : "On-ground")
                  << std::endl;

        const Telemetry::Quaternion &quaternion
            = device.telemetry().attitude_quaternion();
        std::cout << "Quaternion: ("
                  << quaternion.w << ", "
                  << quaternion.x << ","
                  << quaternion.y << ","
                  << quaternion.z << ")" << std::endl;

        const Telemetry::EulerAngle &euler_angle
            = device.telemetry().attitude_euler_angle();
        std::cout << "Euler: (" << euler_angle.roll_deg << " deg, "
                  << euler_angle.pitch_deg << " deg,"
                  << euler_angle.yaw_deg << " deg)" << std::endl;

        const Telemetry::GroundSpeedNED &ground_speed_ned
            = device.telemetry().ground_speed_ned();
        std::cout << "Speed: (" << ground_speed_ned.velocity_north_m_s << " m/s, "
                  << ground_speed_ned.velocity_east_m_s << " m/s,"
                  << ground_speed_ned.velocity_down_m_s << " m/s)" << std::endl;

        const Telemetry::GPSInfo &gps_info = device.telemetry().gps_info();
        std::cout << "GPS sats: " << gps_info.num_satellites
                  << ", fix type: " << gps_info.fix_type << std::endl;

        const Telemetry::Battery &battery = device.telemetry().battery();
        std::cout << "Battery voltage: " << battery.voltage_v << " v, "
                  << "remaining: " << battery.remaining_percent * 100.0f << " \%" << std::endl;


        usleep(30000);
    }
}

TEST(Telemetry, Simple)
{
    test_simple_telemetry();
}
