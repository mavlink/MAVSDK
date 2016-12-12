#include <iostream>
#include <unistd.h>
#include "integration_test_helper.h"
#include "dronelink.h"

using namespace std::placeholders; // for `_1`
using namespace dronelink;


void receive_result(Telemetry::Result result);
void print_position(Telemetry::Position position);
void print_home_position(Telemetry::Position home_position);
void print_in_air(bool in_air);
void print_armed(bool armed);
void print_quaternion(Telemetry::Quaternion quaternion);
void print_euler_angle(Telemetry::EulerAngle euler_angle);
void print_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned);
void print_gps_info(Telemetry::GPSInfo gps_info);
void print_battery(Telemetry::Battery battery);

bool _set_rate_error = false;
bool _received_position = false;
bool _received_home_position = false;
bool _received_in_air = false;
bool _received_armed = false;
bool _received_quaternion = false;
bool _received_euler_angle = false;
bool _received_ground_speed = false;
bool _received_gps_info = false;
bool _received_battery = false;


TEST_F(SitlTest, TelemetryAsync)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();

    sleep(3);

    std::vector<uint64_t> uuids = dl.device_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found device with UUID: " << *it << std::endl;
    }

    ASSERT_EQ(uuids.size(), 1);

    uint64_t uuid = uuids.at(0);

    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    Device &device = dl.device(uuid);

    device.telemetry().set_rate_position_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().set_rate_home_position_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().set_rate_in_air_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().set_rate_attitude_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().set_rate_ground_speed_ned_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().set_rate_gps_info_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().set_rate_battery_async(10.0, std::bind(&receive_result, _1));
    usleep(100000);

    device.telemetry().position_async(std::bind(&print_position, _1));

    device.telemetry().home_position_async(std::bind(&print_home_position, _1));

    device.telemetry().in_air_async(std::bind(&print_in_air, _1));

    device.telemetry().armed_async(std::bind(&print_armed, _1));

    device.telemetry().attitude_quaternion_async(std::bind(&print_quaternion, _1));

    device.telemetry().attitude_euler_angle_async(std::bind(&print_euler_angle, _1));

    device.telemetry().ground_speed_ned_async(std::bind(&print_ground_speed_ned, _1));

    device.telemetry().gps_info_async(std::bind(&print_gps_info, _1));

    device.telemetry().battery_async(std::bind(&print_battery, _1));

    usleep(10000000);

    ASSERT_FALSE(_set_rate_error);
    ASSERT_TRUE(_received_position);
    ASSERT_TRUE(_received_home_position);
    ASSERT_TRUE(_received_in_air);
    ASSERT_TRUE(_received_armed);
    ASSERT_TRUE(_received_quaternion);
    ASSERT_TRUE(_received_euler_angle);
    ASSERT_TRUE(_received_ground_speed);
    ASSERT_TRUE(_received_gps_info);
    ASSERT_TRUE(_received_battery);
}

void receive_result(Telemetry::Result result)
{
    if (result != Telemetry::Result::SUCCESS) {
        _set_rate_error = true;
        std::cerr << "Received ret: " << int(result) << std::endl;
        ASSERT_TRUE(false);
    }
}

void print_position(Telemetry::Position position)
{
    std::cout << "Got position, lat: " << position.latitude_deg << " deg, "
              << "lon: " << position.longitude_deg << " deg, "
              << "relative alt: " << position.relative_altitude_m << " m" << std::endl;
    _received_position = true;
}

void print_home_position(Telemetry::Position home_position)
{
    std::cout << "Got home position, lat: " << home_position.latitude_deg << " deg, "
              << "lon: " << home_position.longitude_deg << " deg, "
              << "relative alt: " << home_position.relative_altitude_m << " m" << std::endl;
    _received_home_position = true;
}

void print_in_air(bool in_air)
{
    std::cout << (in_air ? "in-air" : "on-ground") << std::endl;
    _received_in_air = true;
}

void print_armed(bool armed)
{
    std::cout << (armed ? "armed" : "disarmed") << std::endl;
    _received_armed = true;
}

void print_quaternion(Telemetry::Quaternion quaternion)
{
    std::cout << "Quaternion: [ "
              << quaternion.w << ", "
              << quaternion.x << ", "
              << quaternion.y << ", "
              << quaternion.z << " ]" << std::endl;

    _received_quaternion = true;
}

void print_euler_angle(Telemetry::EulerAngle euler_angle)
{
    std::cout << "Euler angle: [ " << euler_angle.roll_deg << ", "
              << euler_angle.pitch_deg << ", "
              << euler_angle.yaw_deg << " ] deg" << std::endl;

    _received_euler_angle = true;
}

void print_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned)
{
    std::cout << "Ground speed NED: [ " << ground_speed_ned.velocity_north_m_s << ", "
              << ground_speed_ned.velocity_east_m_s << ", "
              << ground_speed_ned.velocity_down_m_s << " ]" << std::endl;

    _received_ground_speed = true;
}


void print_gps_info(Telemetry::GPSInfo gps_info)
{
    std::cout << "GPS, num satellites: " << gps_info.num_satellites << ", "
              << "fix type: " << gps_info.fix_type << std::endl;

    _received_gps_info = true;
}

void print_battery(Telemetry::Battery battery)
{
    std::cout << "Battery: " << battery.voltage_v << " v,"
              << "remaining: " << int(battery.remaining_percent * 1e2f) << " %" << std::endl;

    _received_battery = true;
}
