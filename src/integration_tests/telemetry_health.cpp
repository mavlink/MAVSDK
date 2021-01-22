#include <iostream>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

void print_health(Telemetry::Health health);

TEST_F(SitlTest, TelemetryHealth)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);

    auto telemetry = std::make_shared<Telemetry>(system);

    telemetry->subscribe_health(std::bind(&print_health, std::placeholders::_1));
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void print_health(Telemetry::Health health)
{
    std::cout << "Got health: " << '\n';

    std::cout << "Gyro calibration:  " << (health.is_gyrometer_calibration_ok ? "ok" : "not ok")
              << '\n';
    std::cout << "Accel calibration: " << (health.is_accelerometer_calibration_ok ? "ok" : "not ok")
              << '\n';
    std::cout << "Mag calibration:   " << (health.is_magnetometer_calibration_ok ? "ok" : "not ok")
              << '\n';
    std::cout << "Level calibration: " << (health.is_level_calibration_ok ? "ok" : "not ok")
              << '\n';
    std::cout << "Local position:    " << (health.is_local_position_ok ? "ok" : "not ok") << '\n';
    std::cout << "Global position:   " << (health.is_global_position_ok ? "ok" : "not ok") << '\n';
    std::cout << "Home position:     " << (health.is_home_position_ok ? "ok" : "not ok") << '\n';
}
