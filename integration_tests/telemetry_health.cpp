#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

void print_health(Telemetry::Health health);

TEST(Telemetry, Health)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    usleep(1500000);

    std::vector<uint64_t> uuids = dl.device_uuids();

    ASSERT_EQ(uuids.size(), 1);

    uint64_t uuid = uuids.at(0);

    Device &device = dl.device(uuid);

    device.telemetry().health_async(std::bind(&print_health, std::placeholders::_1));
    usleep(5000000);
}

void print_health(Telemetry::Health health)
{
    std::cout << "Got health: " << std::endl;

    std::cout << "Gyro calibration:  " << (health.gyrometer_calibration_ok ? "ok" : "not ok")
              << std::endl;
    std::cout << "Accel calibration: " << (health.accelerometer_calibration_ok ? "ok" : "not ok")
              << std::endl;
    std::cout << "Mag calibration:   " << (health.magnetometer_calibration_ok ? "ok" : "not ok")
              << std::endl;
    std::cout << "Level calibration: " << (health.level_calibration_ok ? "ok" : "not ok")
              << std::endl;
    std::cout << "Local position:    " << (health.local_position_ok ? "ok" : "not ok")
              << std::endl;
    std::cout << "Global position:   " << (health.global_position_ok ? "ok" : "not ok")
              << std::endl;
    std::cout << "Home position:     " << (health.home_position_ok ? "ok" : "not ok")
              << std::endl;
}

