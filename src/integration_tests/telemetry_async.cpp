#include <iostream>
#include <iomanip>
#include <ctime>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

static void receive_result(Telemetry::Result result);
static void print_position(Telemetry::Position position);
static void print_home_position(Telemetry::Position home_position);
static void print_in_air(bool in_air);
static void print_armed(bool armed);
static void print_quaternion(Telemetry::Quaternion quaternion);
static void print_euler_angle(Telemetry::EulerAngle euler_angle);
static void print_angular_velocity_body(Telemetry::AngularVelocityBody angular_velocity_body);
static void print_fixedwing_metrics(Telemetry::FixedwingMetrics fixedwing_metrics);
static void print_ground_truth(const Telemetry::GroundTruth& ground_truth);
static void print_velocity_ned(Telemetry::VelocityNed velocity_ned);
static void print_imu(Telemetry::Imu imu);
static void print_gps_info(Telemetry::GpsInfo gps_info);
static void print_battery(Telemetry::Battery battery);
static void print_rc_status(Telemetry::RcStatus rc_status);
static void print_position_velocity_ned(Telemetry::PositionVelocityNed position_velocity_ned);
static void print_unix_epoch_time_us(uint64_t time_us);
static void print_actuator_control_target(Telemetry::ActuatorControlTarget actuator_control_target);
static void print_actuator_output_status(Telemetry::ActuatorOutputStatus actuator_output_status);
static void print_altitude(Telemetry::Altitude altitude);

static bool _set_rate_error = false;
static bool _received_position = false;
static bool _received_home_position = false;
static bool _received_in_air = false;
static bool _received_armed = false;
static bool _received_quaternion = false;
static bool _received_euler_angle = false;
static bool _received_angular_velocity_body = false;
static bool _received_fixedwing_metrics = false;
static bool _received_ground_truth = false;
static bool _received_velocity = false;
static bool _received_imu = false;
static bool _received_gps_info = false;
static bool _received_battery = false;
static bool _received_rc_status = false;
static bool _received_position_velocity_ned = false;
static bool _received_actuator_control_target = false;
static bool _received_actuator_output_status = false;
static bool _received_altitude = false;

TEST(SitlTest, PX4TelemetryAsync)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    const auto systems = mavsdk.systems();
    ASSERT_EQ(systems.size(), 1);
    const auto system = systems.at(0);

    std::cout << "Found system" << '\n';

    auto telemetry = std::make_shared<Telemetry>(system);

    telemetry->set_rate_position_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_home_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_in_air_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_attitude_euler_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_attitude_quaternion_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_velocity_ned_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_imu_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_gps_info_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_battery_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_actuator_control_target_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_fixedwing_metrics_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->set_rate_ground_truth_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });

    telemetry->set_rate_altitude_async(
        10.0, [](Telemetry::Result result) { return receive_result(result); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    telemetry->subscribe_position([](Telemetry::Position position) { print_position(position); });

    telemetry->subscribe_home([](Telemetry::Position home) { print_home_position(home); });

    telemetry->subscribe_in_air([](bool is_in_air) { print_in_air(is_in_air); });

    telemetry->subscribe_armed([](bool is_armed) { print_armed(is_armed); });

    telemetry->subscribe_attitude_quaternion(
        [](Telemetry::Quaternion quaternion) { print_quaternion(quaternion); });

    telemetry->subscribe_attitude_euler(
        [](Telemetry::EulerAngle euler_angle) { print_euler_angle(euler_angle); });

    telemetry->subscribe_attitude_angular_velocity_body(
        [](Telemetry::AngularVelocityBody angular_velocity_body) {
            print_angular_velocity_body(angular_velocity_body);
        });

    telemetry->subscribe_fixedwing_metrics([](Telemetry::FixedwingMetrics fixed_wing_metrics) {
        print_fixedwing_metrics(fixed_wing_metrics);
    });

    telemetry->subscribe_ground_truth(
        [](Telemetry::GroundTruth ground_truth) { print_ground_truth(ground_truth); });

    telemetry->subscribe_velocity_ned(
        [](Telemetry::VelocityNed velocity_ned) { print_velocity_ned(velocity_ned); });

    telemetry->subscribe_imu([](Telemetry::Imu imu) { print_imu(imu); });

    telemetry->subscribe_gps_info([](Telemetry::GpsInfo gps_info) { print_gps_info(gps_info); });

    telemetry->subscribe_battery([](Telemetry::Battery battery) { print_battery(battery); });

    telemetry->subscribe_rc_status(
        [](Telemetry::RcStatus rc_status) { print_rc_status(rc_status); });

    telemetry->subscribe_position_velocity_ned(
        [](Telemetry::PositionVelocityNed position_velocity_ned) {
            print_position_velocity_ned(position_velocity_ned);
        });

    // TODO: this should have a unit
    telemetry->subscribe_unix_epoch_time(
        [](uint64_t unit_epoch_time) { print_unix_epoch_time_us(unit_epoch_time); });

    telemetry->subscribe_actuator_control_target(
        [](Telemetry::ActuatorControlTarget actuator_control_target) {
            print_actuator_control_target(actuator_control_target);
        });

    telemetry->subscribe_actuator_output_status(
        [](Telemetry::ActuatorOutputStatus actuator_output_status) {
            print_actuator_output_status(actuator_output_status);
        });

    telemetry->subscribe_altitude([](Telemetry::Altitude altitude) { print_altitude(altitude); });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    EXPECT_FALSE(_set_rate_error);
    EXPECT_TRUE(_received_position);
    EXPECT_TRUE(_received_home_position);
    EXPECT_TRUE(_received_in_air);
    EXPECT_TRUE(_received_armed);
    EXPECT_TRUE(_received_quaternion);
    EXPECT_TRUE(_received_angular_velocity_body);
    EXPECT_TRUE(_received_fixedwing_metrics);
    EXPECT_TRUE(_received_ground_truth);
    EXPECT_TRUE(_received_euler_angle);
    EXPECT_TRUE(_received_velocity);
    EXPECT_TRUE(_received_imu);
    EXPECT_TRUE(_received_gps_info);
    EXPECT_TRUE(_received_battery);
    EXPECT_TRUE(_received_altitude);
    // EXPECT_TRUE(_received_rc_status); // No RC is sent in SITL.
    EXPECT_TRUE(_received_position_velocity_ned);
    // EXPECT_TRUE(_received_actuator_control_target); TODO check is that sent in SITL.
    // EXPECT_TRUE(_received_actuator_output_status); TODO check is that sent in SITL.
}

void receive_result(Telemetry::Result result)
{
    if (result != Telemetry::Result::Success) {
        _set_rate_error = true;
        std::cerr << "Received ret: " << int(result) << '\n';
        EXPECT_TRUE(false);
    }
}

void print_position(Telemetry::Position position)
{
    std::cout << "Got position, lat: " << position.latitude_deg << " deg, "
              << "lon: " << position.longitude_deg << " deg, "
              << "relative alt: " << position.relative_altitude_m << " m" << '\n';
    _received_position = true;
}

void print_home_position(Telemetry::Position home_position)
{
    std::cout << "Got home position, lat: " << home_position.latitude_deg << " deg, "
              << "lon: " << home_position.longitude_deg << " deg, "
              << "relative alt: " << home_position.relative_altitude_m << " m" << '\n';
    _received_home_position = true;
}

void print_in_air(bool in_air)
{
    std::cout << (in_air ? "in-air" : "on-ground") << '\n';
    _received_in_air = true;
}

void print_armed(bool armed)
{
    std::cout << (armed ? "armed" : "disarmed") << '\n';
    _received_armed = true;
}

void print_quaternion(Telemetry::Quaternion quaternion)
{
    std::cout << "Quaternion: [ " << quaternion.w << ", " << quaternion.x << ", " << quaternion.y
              << ", " << quaternion.z << " ]" << '\n';

    _received_quaternion = true;
}

void print_euler_angle(Telemetry::EulerAngle euler_angle)
{
    std::cout << "Euler angle: [ " << euler_angle.roll_deg << ", " << euler_angle.pitch_deg << ", "
              << euler_angle.yaw_deg << " ] deg" << '\n';

    _received_euler_angle = true;
}

void print_angular_velocity_body(Telemetry::AngularVelocityBody angular_velocity_body)
{
    std::cout << "Angular velocity: [ " << angular_velocity_body.roll_rad_s << ", "
              << angular_velocity_body.pitch_rad_s << ", " << angular_velocity_body.yaw_rad_s
              << " ] rad/s" << '\n';

    _received_angular_velocity_body = true;
}

void print_fixedwing_metrics(Telemetry::FixedwingMetrics fixedwing_metrics)
{
    std::cout << "async Airspeed: " << fixedwing_metrics.airspeed_m_s << " m/s, "
              << "Throttle: " << fixedwing_metrics.throttle_percentage << " %, "
              << "Climb: " << fixedwing_metrics.climb_rate_m_s << " m/s" << '\n';
    _received_fixedwing_metrics = true;
}

void print_ground_truth(const Telemetry::GroundTruth& ground_truth)
{
    std::cout << ground_truth << '\n';
    _received_ground_truth = true;
}

void print_velocity_ned(Telemetry::VelocityNed velocity_ned)
{
    std::cout << "Ground speed NED: [ " << velocity_ned.north_m_s << ", " << velocity_ned.east_m_s
              << ", " << velocity_ned.down_m_s << " ]" << '\n';

    _received_velocity = true;
}

void print_imu(Telemetry::Imu imu)
{
    std::cout << imu << '\n';

    _received_imu = true;
}

void print_gps_info(Telemetry::GpsInfo gps_info)
{
    std::cout << "GPS, num satellites: " << gps_info.num_satellites << ", "
              << "fix type: " << gps_info.fix_type << '\n';

    _received_gps_info = true;
}

void print_battery(Telemetry::Battery battery)
{
    std::cout << "Battery " << battery.id << ": " << battery.voltage_v << " v,"
              << "temperature: " << battery.temperature_degc << " degC,"
              << "current: " << battery.current_battery_a << " A,"
              << "capacity: " << battery.capacity_consumed_ah << " Ah,"
              << "remaining: " << int(battery.remaining_percent) << " %" << '\n';

    _received_battery = true;
}

void print_rc_status(Telemetry::RcStatus rc_status)
{
    std::cout << "RC status [ RSSI: " << rc_status.signal_strength_percent * 100 << "]" << '\n';
    _received_rc_status = true;
}

void print_position_velocity_ned(Telemetry::PositionVelocityNed position_velocity_ned)
{
    std::cout << "Got position north:  " << position_velocity_ned.position.north_m << " m, "
              << "east: " << position_velocity_ned.position.east_m << " m, "
              << "down: " << position_velocity_ned.position.down_m << " m" << '\n'
              << "velocity north: " << position_velocity_ned.velocity.north_m_s << " m/s, "
              << "velocity east: " << position_velocity_ned.velocity.east_m_s << " m/s, "
              << "velocity down: " << position_velocity_ned.velocity.down_m_s << " m/s" << '\n';

    _received_position_velocity_ned = true;
}

void print_altitude(Telemetry::Altitude altitude)
{
    std::cout << "altitude_monotonic: " << altitude.altitude_monotonic_m << "m, "
              << "altitude_local" << altitude.altitude_local_m << "m, "
              << "altitude_amsl" << altitude.altitude_amsl_m << "m, "
              << "altitude_relative" << altitude.altitude_relative_m << "m, "
              << "altitude_terrain" << altitude.altitude_terrain_m << "m" << std::endl;

    _received_altitude = true;
}

void print_unix_epoch_time_us(uint64_t time_us)
{
    std::time_t time = time_us / 1000000;
#if __GNUC__ > 4
    LogInfo() << "UTC (" << time << "): " << std::put_time(std::gmtime(&time), "%c %Z");
#else
    // TODO: remove this once we don't have to build with gcc 4 any longer.
    char time_string[24];
    if (0 < strftime(time_string, sizeof(time_string), "%c %Z", std::gmtime(&time)))
        LogInfo() << time_string;
#endif
}

static void print_actuator_control_target(Telemetry::ActuatorControlTarget actuator_control_target)
{
    std::cout << "Group:  " << static_cast<int>(actuator_control_target.group) << ", Controls: [";
    for (int i = 0; i < 8; i++) {
        std::cout << actuator_control_target.controls[i];
        if (i != 7) {
            std::cout << ", ";
        } else {
            std::cout << "]" << '\n';
        }
    }

    _received_actuator_control_target = true;
}

static void print_actuator_output_status(Telemetry::ActuatorOutputStatus actuator_output_status)
{
    std::cout << "Active:  " << actuator_output_status.active << ", Actuators: [";
    for (unsigned i = 0; i < actuator_output_status.active; i++) {
        std::cout << actuator_output_status.actuator[i];
        if (i != (actuator_output_status.active - 1)) {
            std::cout << ", ";
        } else {
            std::cout << "]" << '\n';
        }
    }

    _received_actuator_output_status = true;
}
