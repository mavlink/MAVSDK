#include <iostream>
#include <unistd.h>
#include "dronelink.h"
#include "telemetry.h"

#define UNUSED(x) (void)(x)

#define MAGIC_NUMBER 42

void print_position(dronelink::Telemetry::Position position, void *user);
void print_home_position(dronelink::Telemetry::Position home_position, void *user);
void print_in_air(bool in_air, void *user);
void print_quaternion(dronelink::Telemetry::Quaternion quaternion, void *user);
void print_euler_angle(dronelink::Telemetry::EulerAngle euler_angle, void *user);
void print_ground_speed_ned(dronelink::Telemetry::GroundSpeedNED ground_speed_ned, void *user);
void print_gps_info(dronelink::Telemetry::GPSInfo gps_info, void *user);
void print_battery(dronelink::Telemetry::Battery battery, void *user);

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

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

    unsigned magic = MAGIC_NUMBER;

    dronelink::Telemetry::PositionCallbackData position_callback_data =
        {&print_position, &magic};
    device.telemetry().position_async(10.0, position_callback_data);

    dronelink::Telemetry::PositionCallbackData home_position_callback_data =
        {&print_home_position, nullptr};
    device.telemetry().home_position_async(10.0, home_position_callback_data);

    dronelink::Telemetry::InAirCallbackData in_air_callback_data =
        {&print_in_air, nullptr};
    device.telemetry().in_air_async(10.0, in_air_callback_data);

    dronelink::Telemetry::AttitudeQuaternionCallbackData attitude_quaternion_callback_data =
        {&print_quaternion, nullptr};
    device.telemetry().attitude_quaternion_async(10.0, attitude_quaternion_callback_data);

    dronelink::Telemetry::AttitudeEulerAngleCallbackData attitude_euler_angle_callback_data =
        {&print_euler_angle, nullptr};
    device.telemetry().attitude_euler_angle_async(10.0, attitude_euler_angle_callback_data);

    dronelink::Telemetry::GroundSpeedNEDCallbackData ground_speed_ned_callback_data =
        {&print_ground_speed_ned, nullptr};
    device.telemetry().ground_speed_ned_async(10.0, ground_speed_ned_callback_data);


    dronelink::Telemetry::GPSInfoCallbackData gps_info_callback_data =
        {&print_gps_info, nullptr};
    device.telemetry().gps_info_async(10.0, gps_info_callback_data);


    dronelink::Telemetry::BatteryCallbackData battery_callback_data =
        {&print_battery, nullptr};
    device.telemetry().battery_async(10.0, battery_callback_data);


    usleep(10000000);

    return 0;
}

void print_position(dronelink::Telemetry::Position position, void *user)
{
    std::cout << "Got position, lat: " << position.latitude_deg << " deg, "
              << "lon: " << position.longitude_deg << " deg, "
              << "relative alt: " << position.relative_altitude_m << " m" << std::endl;

    unsigned *magic = reinterpret_cast<unsigned *>(user);

    if (*magic != MAGIC_NUMBER) {
        std::cerr << "Error: we lost the magic number" << std::endl;
        exit(1);
    }

}

void print_home_position(dronelink::Telemetry::Position home_position, void *user)
{
    UNUSED(user);
    std::cout << "Got home position, lat: " << home_position.latitude_deg << " deg, "
              << "lon: " << home_position.longitude_deg << " deg, "
              << "relative alt: " << home_position.relative_altitude_m << " m" << std::endl;
}

void print_in_air(bool in_air, void *user)
{
    UNUSED(user);
    std::cout << (in_air ? "in-air" : "on-ground") << std::endl;
}

void print_quaternion(dronelink::Telemetry::Quaternion quaternion, void *user)
{
    UNUSED(user);
    std::cout << "Quaternion: [ " << quaternion.vec[0] << ", "
                                  << quaternion.vec[1] << ", "
                                  << quaternion.vec[2] << ", "
                                  << quaternion.vec[3] << " ]" << std::endl;
}

void print_euler_angle(dronelink::Telemetry::EulerAngle euler_angle, void *user)
{
    UNUSED(user);
    std::cout << "Euler angle: [ " << euler_angle.roll_deg << ", "
                                   << euler_angle.pitch_deg << ", "
                                   << euler_angle.yaw_deg << " ] deg" << std::endl;
}

void print_ground_speed_ned(dronelink::Telemetry::GroundSpeedNED ground_speed_ned, void *user)
{
    UNUSED(user);
    std::cout << "Ground speed NED: [ " << ground_speed_ned.velocity_north_m_s << ", "
                                        << ground_speed_ned.velocity_east_m_s << ", "
                                        << ground_speed_ned.velocity_down_m_s << " ]" << std::endl;
}

void print_gps_info(dronelink::Telemetry::GPSInfo gps_info, void *user)
{
    UNUSED(user);
    std::cout << "GPS, num satellites: " << gps_info.num_satellites << ", "
              << "fix type: " << gps_info.fix_type << std::endl;
}

void print_battery(dronelink::Telemetry::Battery battery, void *user)
{
    UNUSED(user);
    std::cout << "Battery: " << battery.voltage_v << " v,"
        << "remaining: " << int(battery.remaining*1e2f) << " %" << std::endl;
}
