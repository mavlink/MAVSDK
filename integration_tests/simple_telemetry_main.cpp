#include <iostream>
#include <unistd.h>
#include "dronelink.h"

#define UNUSED(x) (void)(x)

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    dronelink::DroneLink dl;

    dronelink::Result ret = dl.add_udp_connection("127.0.0.1", 14540);

    usleep(1000000);

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

    if (ret != dronelink::Result::SUCCESS) {
        std::cout << "failed to add connection: " << result_str(ret) << std::endl;
    }

    dronelink::Device &device = dl.device(uuid);

    // Print 3s of telemetry.
    for (unsigned i = 0; i < 100; ++i) {

        const dronelink::Telemetry::Position &position = device.telemetry().position();
        std::cout << "Position: " << std::endl
                  << "Absolute altitude: " << position.absolute_altitude_m << " m" << std::endl
                  << "Relative altitude: " << position.relative_altitude_m << " m" << std::endl
                  << "Latitude: "
                  << position.latitude_deg << " deg"
                  << ", longitude: "
                  << position.longitude_deg << " deg" << std::endl;

        const dronelink::Telemetry::Position &home_position = device.telemetry().home_position();
        std::cout << "Home position: " << std::endl
                  << "Absolute altitude: " << home_position.absolute_altitude_m << " m" << std::endl
                  << "Relative altitude: " << home_position.relative_altitude_m << " m" << std::endl
                  << "Latitude: "
                  << home_position.latitude_deg << " deg"
                  << ", longitude: "
                  << home_position.longitude_deg << " deg" << std::endl;

        std::cout << (device.telemetry().in_air() ? "In-air" : "On-ground")
                  << std::endl;

        const dronelink::Telemetry::Quaternion &quaternion
            = device.telemetry().attitude_quaternion();
        std::cout << "Quaternion: (" << quaternion.vec[0] << ", "
                                     << quaternion.vec[1] << ","
                                     << quaternion.vec[2] << ","
                                     << quaternion.vec[3] << ")" << std::endl;

        const dronelink::Telemetry::EulerAngle &euler_angle
            = device.telemetry().attitude_euler_angle();
        std::cout << "Euler: (" << euler_angle.roll_deg << " deg, "
                                << euler_angle.pitch_deg << " deg,"
                                << euler_angle.yaw_deg << " deg)" << std::endl;

        const dronelink::Telemetry::GroundSpeedNED &ground_speed_ned
            = device.telemetry().ground_speed_ned();
        std::cout << "Speed: (" << ground_speed_ned.velocity_north_m_s << " m/s, "
                                << ground_speed_ned.velocity_east_m_s << " m/s,"
                                << ground_speed_ned.velocity_down_m_s << " m/s)" << std::endl;

        const dronelink::Telemetry::GPSInfo &gps_info = device.telemetry().gps_info();
        std::cout << "GPS sats: " << gps_info.num_satellites
                  << ", fix type: " << gps_info.fix_type << std::endl;

        const dronelink::Telemetry::Battery &battery = device.telemetry().battery();
        std::cout << "Battery voltage: " << battery.voltage_v << " v, "
                  << "remaining: " << battery.remaining * 100.0f << " \%"<< std::endl;


        usleep(30000);
    }

    return 0;
}
