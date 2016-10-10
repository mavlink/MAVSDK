#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace std::placeholders; // for `_1`

void print_position(dronelink::Telemetry::Position position);
void print_home_position(dronelink::Telemetry::Position home_position);
void print_in_air(bool in_air);
void print_armed(bool armed);
void print_quaternion(dronelink::Telemetry::Quaternion quaternion);
void print_euler_angle(dronelink::Telemetry::EulerAngle euler_angle);
void print_ground_speed_ned(dronelink::Telemetry::GroundSpeedNED ground_speed_ned);
void print_gps_info(dronelink::Telemetry::GPSInfo gps_info);
void print_battery(dronelink::Telemetry::Battery battery);

int test_async_telemetry()
{
    dronelink::DroneLink dl;

    dronelink::DroneLink::ConnectionResult ret = dl.add_udp_connection();

    usleep(1500000);

    std::vector<uint64_t> uuids = dl.device_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found device with UUID: " << *it << std::endl;
    }

    if (uuids.size() > 1) {
        std::cout << "found more than one device, not sure which one to use." << std::endl;
        return -1;
    } else if (uuids.size() == 0) {
        std::cout << "no device found." << std::endl;
        return -1;
    }

    uint64_t uuid = uuids.at(0);

    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }

    dronelink::Device &device = dl.device(uuid);

    device.telemetry().position_async(10.0, std::bind(&print_position, _1));

    device.telemetry().home_position_async(10.0, std::bind(&print_home_position, _1));

    device.telemetry().in_air_async(10.0, std::bind(&print_in_air, _1));

    device.telemetry().armed_async(std::bind(&print_armed, _1));

    device.telemetry().attitude_quaternion_async(10.0, std::bind(&print_quaternion, _1));

    device.telemetry().attitude_euler_angle_async(10.0, std::bind(&print_euler_angle, _1));

    device.telemetry().ground_speed_ned_async(10.0, std::bind(&print_ground_speed_ned, _1));

    device.telemetry().gps_info_async(10.0, std::bind(&print_gps_info, _1));

    device.telemetry().battery_async(10.0, std::bind(&print_battery, _1));


    usleep(10000000);

    return 0;
}

void print_position(dronelink::Telemetry::Position position)
{
    std::cout << "Got position, lat: " << position.latitude_deg << " deg, "
              << "lon: " << position.longitude_deg << " deg, "
              << "relative alt: " << position.relative_altitude_m << " m" << std::endl;

}

void print_home_position(dronelink::Telemetry::Position home_position)
{
    std::cout << "Got home position, lat: " << home_position.latitude_deg << " deg, "
              << "lon: " << home_position.longitude_deg << " deg, "
              << "relative alt: " << home_position.relative_altitude_m << " m" << std::endl;
}

void print_in_air(bool in_air)
{
    std::cout << (in_air ? "in-air" : "on-ground") << std::endl;
}

void print_armed(bool armed)
{
    std::cout << (armed ? "armed" : "disarmed") << std::endl;
}

void print_quaternion(dronelink::Telemetry::Quaternion quaternion)
{
    std::cout << "Quaternion: [ " << quaternion.vec[0] << ", "
              << quaternion.vec[1] << ", "
              << quaternion.vec[2] << ", "
              << quaternion.vec[3] << " ]" << std::endl;
}

void print_euler_angle(dronelink::Telemetry::EulerAngle euler_angle)
{
    std::cout << "Euler angle: [ " << euler_angle.roll_deg << ", "
              << euler_angle.pitch_deg << ", "
              << euler_angle.yaw_deg << " ] deg" << std::endl;
}

void print_ground_speed_ned(dronelink::Telemetry::GroundSpeedNED ground_speed_ned)
{
    std::cout << "Ground speed NED: [ " << ground_speed_ned.velocity_north_m_s << ", "
              << ground_speed_ned.velocity_east_m_s << ", "
              << ground_speed_ned.velocity_down_m_s << " ]" << std::endl;
}

void print_gps_info(dronelink::Telemetry::GPSInfo gps_info)
{
    std::cout << "GPS, num satellites: " << gps_info.num_satellites << ", "
              << "fix type: " << gps_info.fix_type << std::endl;
}

void print_battery(dronelink::Telemetry::Battery battery)
{
    std::cout << "Battery: " << battery.voltage_v << " v,"
              << "remaining: " << int(battery.remaining * 1e2f) << " %" << std::endl;
}

TEST(Telemetry, Async)
{
    ASSERT_EQ(test_async_telemetry(), 0);
}
