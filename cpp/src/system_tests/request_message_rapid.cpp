#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

// Reproduces a race when the same message is requested in rapid succession via
// MAV_CMD_REQUEST_MESSAGE.
//
// A request completes as soon as the requested message arrives, which -- with the
// autopilot streaming it at a high rate -- is usually before the COMMAND_ACK for
// the command that asked for it. The work item is then gone, and the next request
// for the same message id is already in flight by the time that ack shows up. If
// results and timeouts are matched back to a request by message id alone, they
// land on that innocent successor instead: its retry counter gets bumped by acks
// it never asked for, and its pending timeout is silently replaced, leaking a
// timer that can no longer be cancelled and later fires on some later request.
// Enough of those and a request that had nothing wrong with it exhausts its
// retries and reports a Timeout.
//
// A single sequential requester never hits this: nothing else is in flight for an
// old ack to be confused with. To reproduce we hammer the same request from
// several threads at once while the autopilot streams the response message at a
// high rate.
//
// Expected results are only Success (the winning request) or Busy (losing,
// overlapping requests). The bug shows up as a Timeout.
TEST(SystemTest, RequestMessageRapid)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15230"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15230"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    // Held by shared_ptr so the COMMAND_LONG callback (which runs on the
    // autopilot's user-callback thread, outliving this scope) can hold a
    // weak_ptr and safely no-op if we have already been torn down.
    auto autopilot_direct = std::make_shared<MavlinkDirect>(gs_system);
    std::weak_ptr<MavlinkDirect> autopilot_direct_weak = autopilot_direct;

    // The autopilot acks any REQUEST_MESSAGE command so the command queue does
    // not fill up with unacked, retrying commands over many iterations.
    autopilot_direct->subscribe_message(
        "COMMAND_LONG", [autopilot_direct_weak](MavlinkDirect::MavlinkMessage command) {
            auto direct = autopilot_direct_weak.lock();
            if (!direct) {
                return;
            }

            nlohmann::json root;
            if (((root = nlohmann::json::parse(command.fields_json, nullptr, false))
                     .is_discarded())) {
                return;
            }
            const int command_id = root["command"].get<int>();

            MavlinkDirect::MavlinkMessage ack;
            ack.message_name = "COMMAND_ACK";
            ack.system_id = 1;
            ack.component_id = 1;
            ack.target_system_id = 0;
            ack.target_component_id = 0;
            ack.fields_json = R"({"command":)" + std::to_string(command_id) +
                              R"(,"result":0,"progress":0,"result_param2":0,)" +
                              R"("target_system":0,"target_component":0})";
            direct->send_message(ack);
        });

    // Stream GPS_GLOBAL_ORIGIN at a high rate. This keeps the ground station's
    // receive thread inside process_message (holding the message handler mutex)
    // and also serves as the response to each request.
    MavlinkDirect::MavlinkMessage gps;
    gps.message_name = "GPS_GLOBAL_ORIGIN";
    gps.system_id = 1;
    gps.component_id = 1;
    gps.target_system_id = 0;
    gps.target_component_id = 0;
    gps.fields_json =
        R"({"latitude":473977418,"longitude":85455938,"altitude":488000,"time_usec":0})";

    std::atomic<bool> keep_streaming{true};
    std::thread streamer([&]() {
        while (keep_streaming) {
            autopilot_direct->send_message(gps);
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    });

    // Let discovery and streaming settle.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Hammer the same request from several threads. Stop as soon as any thread
    // sees an unexpected result (the bug manifests as a Timeout).
    const unsigned num_threads = 8;
    const int iterations_per_thread = 1000;

    std::atomic<bool> bad_result{false};
    std::atomic<int> success_count{0};

    std::vector<std::thread> requesters;
    for (unsigned t = 0; t < num_threads; ++t) {
        requesters.emplace_back([&]() {
            for (int i = 0; i < iterations_per_thread && !bad_result; ++i) {
                auto result = telemetry.get_gps_global_origin();
                if (result.first == Telemetry::Result::Success) {
                    ++success_count;
                } else if (result.first == Telemetry::Result::Busy) {
                    // Expected: another request for the same message was in flight.
                } else {
                    LogErr("Unexpected result: {}", static_cast<int>(result.first));
                    bad_result = true;
                }
            }
        });
    }

    for (auto& requester : requesters) {
        requester.join();
    }

    keep_streaming = false;
    streamer.join();

    EXPECT_FALSE(bad_result) << "A request timed out even though its message kept arriving.";
    EXPECT_GT(success_count, 0) << "No request ever succeeded; test did not exercise the path.";
}
