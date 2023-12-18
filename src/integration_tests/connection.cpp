#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <iostream>

#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"

using namespace mavsdk;

static void
connection_test(const std::string& client_system_address, const std::string& server_system_address);
static std::shared_ptr<System> discover_system(Mavsdk& mavsdk_instance);
static void prepare_autopilot(MavlinkPassthrough& mavlink_passthrough);

TEST(ConnectionTest, UdpListenOnDefaultPath)
{
    connection_test("udp://127.0.0.1:55555", "udp://:55555");
}

TEST(ConnectionTest, UdpListenOnExplicitPath)
{
    connection_test("udp://127.0.0.1:55555", "udp://0.0.0.0:55555");
}

static void
connection_test(const std::string& client_system_address, const std::string& server_system_address)
{
    // Start instance as a UDP server pretending to be an autopilot
    Mavsdk mavsdk_server{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    ConnectionResult ret_server = mavsdk_server.add_any_connection(server_system_address);
    ASSERT_EQ(ret_server, ConnectionResult::Success);

    // Start instance as a UDP client, connecting to the server above
    Mavsdk mavsdk_client{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult ret_client = mavsdk_client.add_any_connection(client_system_address);
    ASSERT_EQ(ret_client, ConnectionResult::Success);

    // Wait for autopilot to discover client
    auto autopilot_to_client_system = discover_system(mavsdk_server);

    // Wait for client to discover autopilot
    auto client_to_autopilot_system = discover_system(mavsdk_client);

    // Prepare autopilot to ACK any command it receives
    MavlinkPassthrough mavlink_passthrough(autopilot_to_client_system);
    prepare_autopilot(mavlink_passthrough);

    // Send any command to the autopilot (which should ACK)
    Action action(client_to_autopilot_system);
    Action::Result takeoff_result = action.takeoff();

    EXPECT_EQ(takeoff_result, Action::Result::Success);
}

static std::shared_ptr<System> discover_system(Mavsdk& mavsdk_instance)
{
    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    mavsdk_instance.subscribe_on_new_system([&mavsdk_instance, &prom]() {
        const auto system = mavsdk_instance.systems().at(0);

        if (system->is_connected()) {
            prom.set_value();
        }
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);

    return mavsdk_instance.systems().at(0);
}

static void prepare_autopilot(MavlinkPassthrough& mavlink_passthrough)
{
    mavlink_passthrough.subscribe_message(
        MAVLINK_MSG_ID_COMMAND_LONG,
        [&mavlink_passthrough](const mavlink_message_t& mavlink_message) {
            mavlink_command_long_t cmd_read;
            mavlink_msg_command_long_decode(&mavlink_message, &cmd_read);

            const auto cmd_id = cmd_read.command;
            auto mav_result = MAV_RESULT_ACCEPTED;

            mavlink_passthrough.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
                mavlink_message_t message;
                mavlink_msg_command_ack_pack_chan(
                    mavlink_address.system_id,
                    mavlink_address.component_id,
                    channel,
                    &message,
                    cmd_id,
                    mav_result,
                    255,
                    -1,
                    mavlink_passthrough.get_target_sysid(),
                    mavlink_passthrough.get_target_compid());
                return message;
            });
        });
}
