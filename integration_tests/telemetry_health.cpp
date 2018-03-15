#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecore;

void print_health(Telemetry::Health health);

TEST_F(SitlTest, TelemetryHealth)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();

    auto telemetry = std::make_shared<Telemetry>(system);

    telemetry->health_async(std::bind(&print_health, std::placeholders::_1));
    std::this_thread::sleep_for(std::chrono::seconds(3));
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

