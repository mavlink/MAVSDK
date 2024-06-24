#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "integration_test_helper.h"

using namespace mavsdk;

TEST(SitlTestDisabled, TelemetryGpsOrigin)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto telemetry = Telemetry{system};

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [&telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry.health_all_ok();
        },
        std::chrono::seconds(10)));

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto result = telemetry.get_gps_global_origin();
    ASSERT_EQ(result.first, Telemetry::Result::Success);
    LogInfo() << result.second;
}
