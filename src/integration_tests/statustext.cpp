#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/server_utility/server_utility.h"
#include "plugins/telemetry/telemetry.h"
#include <string>
#include <chrono>
#include <thread>
#include <future>

using namespace mavsdk;

static const std::string lorem_ipsum =
    "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium.";
static const auto type = ServerUtility::StatusTextType::Info;

TEST(StatusTextTest, TestServer)
{
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk_gcs.add_any_connection("udpin://0.0.0.0:24550"), ConnectionResult::Success);

    Mavsdk mavsdk_onboard{Mavsdk::Configuration{Mavsdk::ComponentType::CompanionComputer}};
    ASSERT_EQ(
        mavsdk_onboard.add_any_connection("udpout://127.0.0.1:24550"), ConnectionResult::Success);

    // Let the two connect to each other.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    for (auto system : mavsdk_gcs.systems()) {
        LogInfo() << "gcs system: " << int(system->get_system_id());
    }

    for (auto system : mavsdk_onboard.systems()) {
        LogInfo() << "onboard system: " << int(system->get_system_id());
    }

    ASSERT_EQ(mavsdk_gcs.systems().size(), 1);
    // FIXME: For some reason there are two onboard systems:
    //        - one of the discovered ground station (245)
    //        - and one of the created system (1)
    ASSERT_GE(mavsdk_onboard.systems().size(), 1);

    auto system_gcs = mavsdk_gcs.systems().at(0);
    auto system_onboard = mavsdk_onboard.systems().at(0);

    auto telemetry = Telemetry(system_gcs);
    auto server_utility = ServerUtility(system_onboard);

    auto prom = std::promise<void>{};
    auto fut = prom.get_future();

    telemetry.subscribe_status_text([&prom](Telemetry::StatusText status_text) {
        LogInfo() << "Received status text";
        EXPECT_EQ(status_text.text, lorem_ipsum);
        EXPECT_EQ(static_cast<int>(status_text.type), static_cast<int>(type));
        prom.set_value();
    });

    EXPECT_EQ(
        server_utility.send_status_text(ServerUtility::StatusTextType::Info, lorem_ipsum),
        ServerUtility::Result::Success);

    auto ret = fut.wait_for(std::chrono::seconds(1));

    EXPECT_EQ(ret, std::future_status::ready);
}
