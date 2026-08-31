#include "mavsdk.hpp"
#include "mavlink_include.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;

// Everything Mavsdk hands out -- System, ServerComponent, and any plugin built on one --
// holds a reference back into MavsdkImpl, so none of it may outlive the Mavsdk instance.
// ~MavsdkImpl detects that and aborts rather than leaving a use-after-free to surface
// somewhere else later; see MavsdkImpl::abort_if_references_outlive_us().
//
// These check that the detector actually fires, and with the message it is supposed to.
// Without them the check is untested, and the risk with a check like this is not that it
// fails to fire but that it fires when it should not -- the rest of the suite covers that
// side, by constructing systems and plugins in a hundred different shapes without tripping
// it.
//
// Death tests fork, which is not safe in a threaded program, so ask for the "threadsafe"
// style: gtest re-executes this binary for the child instead of forking.

namespace {

std::vector<char> make_heartbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack_chan(
        1,
        MAV_COMP_ID_AUTOPILOT1,
        MAVLINK_COMM_NUM_BUFFERS - 1,
        &message,
        MAV_TYPE_QUADROTOR,
        MAV_AUTOPILOT_PX4,
        MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
        0,
        MAV_STATE_STANDBY);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    const auto length = mavlink_msg_to_send_buffer(buffer.data(), &message);

    return std::vector<char>(buffer.begin(), buffer.begin() + length);
}

} // namespace

TEST(MavsdkOutlived, AbortsWhenSystemIsStillHeld)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    EXPECT_DEATH(
        {
            std::shared_ptr<System> leaked;
            {
                Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
                mavsdk.add_any_connection("raw://");

                const auto heartbeat = make_heartbeat();

                // Keep them coming rather than sending one: the first heartbeat is what
                // creates the system, and SystemImpl::init() registers its own HEARTBEAT
                // handler with a posted registration, so that first message is not yet
                // delivered to it and does not mark the system connected. A real autopilot
                // sends at 1 Hz, so this is what actually happens anyway.
                std::atomic<bool> stop{false};
                std::thread beater([&]() {
                    while (!stop) {
                        mavsdk.pass_received_raw_bytes(heartbeat.data(), heartbeat.size());
                        std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    }
                });

                auto maybe_system = mavsdk.first_autopilot(5.0);
                stop = true;
                beater.join();

                // No ASSERT here: inside a death-test statement it expands to a return,
                // which gtest rejects. If the system never showed up, `leaked` stays empty
                // and the missing abort fails the test on its own.
                if (maybe_system) {
                    leaked = maybe_system.value();
                }
            }
            // Leaving that scope destroys the Mavsdk instance while `leaked` still refers
            // into it, which has to be caught here rather than when `leaked` goes away.
        },
        "must not outlive the Mavsdk instance");
}

TEST(MavsdkOutlived, AbortsWhenServerComponentIsStillHeld)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    EXPECT_DEATH(
        {
            std::shared_ptr<ServerComponent> leaked;
            {
                Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::Autopilot}};
                leaked = mavsdk.server_component();
            }
        },
        "must not outlive the Mavsdk instance");
}
