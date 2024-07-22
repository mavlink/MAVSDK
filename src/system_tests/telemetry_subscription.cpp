#include "log.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, TelemetrySubscription)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("tcpin://0.0.0.0:13000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("tcpout://127.0.0.1:13000"), ConnectionResult::Success);

    auto telemetry_server = TelemetryServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = Telemetry{system};

    auto prom1 = std::promise<void>{};
    auto fut1 = prom1.get_future();
    unsigned num_subscription1_called = 0;
    auto handle1 = telemetry.subscribe_status_text([&](const Telemetry::StatusText& status_text) {
        LogInfo() << "Received: " << status_text.text;
        ++num_subscription1_called;
        if (num_subscription1_called == 2) {
            prom1.set_value();
        }
    });

    auto prom2 = std::promise<void>{};
    auto fut2 = prom2.get_future();
    unsigned num_subscription2_called = 0;

    std::function<void(Telemetry::StatusText)> callback =
        [&](const Telemetry::StatusText& status_text) {
            LogInfo() << "Also received: " << status_text.text;
            ++num_subscription2_called;
            if (num_subscription2_called == 3) {
                prom2.set_value();
            }
        };

    telemetry.subscribe_status_text(std::move(callback));

    telemetry_server.publish_status_text(
        {TelemetryServer::StatusTextType::Info, "I'm a test message"});
    telemetry_server.publish_status_text(
        {TelemetryServer::StatusTextType::Info, "I'm the second one"});

    EXPECT_EQ(fut1.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    telemetry.unsubscribe_status_text(handle1);

    telemetry_server.publish_status_text(
        {TelemetryServer::StatusTextType::Info, "I'm the third one"});

    EXPECT_EQ(fut2.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    EXPECT_EQ(num_subscription1_called, 2);
    EXPECT_EQ(num_subscription2_called, 3);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
