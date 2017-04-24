#include <iostream>
#include "integration_test_helper.h"
#include "dronelink.h"

using namespace dronelink;

void print_health(Telemetry::Health health);

TEST_F(SitlTest, TelemetryHealth)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);
    sleep(2);

    Device &device = dl.device();

    device.telemetry().health_async(std::bind(&print_health, std::placeholders::_1));
    sleep(3);
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

