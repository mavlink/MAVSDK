#include "log.h"
#include "mavsdk.h"
#include "system_tests_helper.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"

using namespace mavsdk;

static constexpr auto param_name = "TEST_BLA";

static std::string generate_uppercase_ascii(size_t length)
{
    auto data = std::string();
    data.reserve(length);

    static unsigned counter = 0;

    for (size_t i = 0; i < length; ++i) {
        data += static_cast<char>((counter++ % 26) + 65); // ASCII A is 65, A..Z are 25 chars
    }

    data.resize(length);

    return data;
}

TEST(SystemTest, ParamCustomSetAndGet)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});

    Mavsdk mavsdk_autopilot;
    mavsdk_autopilot.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::Autopilot});

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{
        mavsdk_autopilot.server_component_by_type(Mavsdk::ServerComponentType::Autopilot)};

    auto fut = wait_for_first_system_detected(mavsdk_groundstation);
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    auto system = fut.get();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};
    param.late_init(1, true);

    const auto data = generate_uppercase_ascii(128);

    // Make it available.
    EXPECT_EQ(param_server.provide_param_custom(param_name, data), ParamServer::Result::Success);

    // Now it should be available
    auto result_pair = param.get_param_custom(param_name);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, data);

    const auto data_shorter = generate_uppercase_ascii(55);

    // Let's now change the values
    auto result = param.set_param_custom(param_name, data_shorter);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair = param.get_param_custom(param_name);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, data_shorter);

    // Also check the server side
    auto server_result_pair = param_server.retrieve_param_custom(param_name);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_EQ(server_result_pair.second, data_shorter);
}
