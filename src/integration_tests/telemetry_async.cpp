#include <iostream>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/telemetry/telemetry.h"

#define CAMERA_AVAILABLE 0 // Set to 1 if camera is available and should be tested.

using namespace std::placeholders; // for `_1`
using namespace dronecode_sdk;

static void receive_result(Telemetry::Result result);
static void print_position(Telemetry::Position position);
static void print_home_position(Telemetry::Position home_position);
static void print_in_air(bool in_air);
static void print_armed(bool armed);
static void print_quaternion(Telemetry::Quaternion quaternion);
static void print_euler_angle(Telemetry::EulerAngle euler_angle);
#if CAMERA_AVAILABLE == 1
static void print_camera_quaternion(Telemetry::Quaternion quaternion);
static void print_camera_euler_angle(Telemetry::EulerAngle euler_angle);
#endif
static void print_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned);
static void print_gps_info(Telemetry::GPSInfo gps_info);
static void print_battery(Telemetry::Battery battery);
static void print_rc_status(Telemetry::RCStatus rc_status);
static void print_position_velocity_ned(Telemetry::PositionVelocityNED position_velocity_ned);

static bool _set_rate_error = false;
static bool _received_position = false;
static bool _received_home_position = false;
static bool _received_in_air = false;
static bool _received_armed = false;
static bool _received_quaternion = false;
static bool _received_euler_angle = false;
#if CAMERA_AVAILABLE == 1
static bool _received_camera_quaternion = false;
static bool _received_camera_euler_angle = false;
#endif
static bool _received_ground_speed = false;
static bool _received_gps_info = false;
static bool _received_battery = false;
static bool _received_rc_status = false;
static bool _received_position_velocity_ned = false;

TEST_F(SitlTest, TelemetryAsync)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::vector<uint64_t> uuids = dc.system_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found system with UUID: " << *it << std::endl;
    }

    ASSERT_EQ(uuids.size(), 1);

    uint64_t uuid = uuids.at(0);

    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    System &system = dc.system(uuid);

    auto telemetry = std::make_shared<Telemetry>(system);

    telemetry->set_rate_position_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_home_position_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_in_air_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_attitude_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_ground_speed_ned_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_gps_info_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_battery_async(10.0, std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->position_async(std::bind(&print_position, _1));

    telemetry->home_position_async(std::bind(&print_home_position, _1));

    telemetry->in_air_async(std::bind(&print_in_air, _1));

    telemetry->armed_async(std::bind(&print_armed, _1));

    telemetry->attitude_quaternion_async(std::bind(&print_quaternion, _1));

    telemetry->attitude_euler_angle_async(std::bind(&print_euler_angle, _1));

#if CAMERA_AVAILABLE == 1
    telemetry->camera_attitude_quaternion_async(std::bind(&print_camera_quaternion, _1));

    telemetry->camera_attitude_euler_angle_async(std::bind(&print_camera_euler_angle, _1));
#endif

    telemetry->ground_speed_ned_async(std::bind(&print_ground_speed_ned, _1));

    telemetry->gps_info_async(std::bind(&print_gps_info, _1));

    telemetry->battery_async(std::bind(&print_battery, _1));

    telemetry->rc_status_async(std::bind(&print_rc_status, _1));

    telemetry->position_velocity_ned_async(std::bind(&print_position_velocity_ned, _1));

    std::this_thread::sleep_for(std::chrono::seconds(10));

    EXPECT_FALSE(_set_rate_error);
    EXPECT_TRUE(_received_position);
    EXPECT_TRUE(_received_home_position);
    EXPECT_TRUE(_received_in_air);
    EXPECT_TRUE(_received_armed);
    EXPECT_TRUE(_received_quaternion);
    EXPECT_TRUE(_received_euler_angle);
#if CAMERA_AVAILABLE == 1
    EXPECT_TRUE(_received_camera_quaternion);
    EXPECT_TRUE(_received_camera_euler_angle);
#endif
    EXPECT_TRUE(_received_ground_speed);
    EXPECT_TRUE(_received_gps_info);
    EXPECT_TRUE(_received_battery);
    // EXPECT_TRUE(_received_rc_status); // No RC is sent in SITL.
    EXPECT_TRUE(_received_position_velocity_ned);
}

void receive_result(Telemetry::Result result)
{
    if (result != Telemetry::Result::SUCCESS) {
        _set_rate_error = true;
        std::cerr << "Received ret: " << int(result) << std::endl;
        EXPECT_TRUE(false);
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
    std::cout << "Quaternion: [ " << quaternion.w << ", " << quaternion.x << ", " << quaternion.y
              << ", " << quaternion.z << " ]" << std::endl;

    _received_quaternion = true;
}

void print_euler_angle(Telemetry::EulerAngle euler_angle)
{
    std::cout << "Euler angle: [ " << euler_angle.roll_deg << ", " << euler_angle.pitch_deg << ", "
              << euler_angle.yaw_deg << " ] deg" << std::endl;

    _received_euler_angle = true;
}

#if CAMERA_AVAILABLE == 1
void print_camera_quaternion(Telemetry::Quaternion quaternion)
{
    std::cout << "Camera Quaternion: [ " << quaternion.w << ", " << quaternion.x << ", "
              << quaternion.y << ", " << quaternion.z << " ]" << std::endl;

    _received_camera_quaternion = true;
}

void print_camera_euler_angle(Telemetry::EulerAngle euler_angle)
{
    std::cout << "Camera Euler angle: [ " << euler_angle.roll_deg << ", " << euler_angle.pitch_deg
              << ", " << euler_angle.yaw_deg << " ] deg" << std::endl;

    _received_camera_euler_angle = true;
}
#endif

void print_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned)
{
    std::cout << "Ground speed NED: [ " << ground_speed_ned.velocity_north_m_s << ", "
              << ground_speed_ned.velocity_east_m_s << ", " << ground_speed_ned.velocity_down_m_s
              << " ]" << std::endl;

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

void print_rc_status(Telemetry::RCStatus rc_status)
{
    std::cout << "RC status [ RSSI: " << rc_status.signal_strength_percent * 100 << "]"
              << std::endl;
    _received_rc_status = true;
}

void print_position_velocity_ned(Telemetry::PositionVelocityNED position_velocity_ned)
{
    std::cout << "Got position north:  " << position_velocity_ned.position.north_m << " m, "
              << "east: " << position_velocity_ned.position.east_m << " m, "
              << "down: " << position_velocity_ned.position.down_m << " m" << std::endl
              << "velocity north: " << position_velocity_ned.velocity.north_m_s << " m/s, "
              << "velocity east: " << position_velocity_ned.velocity.east_m_s << " m/s, "
              << "velocity down: " << position_velocity_ned.velocity.down_m_s << " m/s"
              << std::endl;

    _received_position_velocity_ned = true;
}
