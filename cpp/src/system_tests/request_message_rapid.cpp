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
#include <json/json.h>

using namespace mavsdk;

// Reproduces a race when the same message is requested in rapid succession via
// MAV_CMD_REQUEST_MESSAGE.
//
// MavlinkRequestMessage registers a message handler for each request and defers
// the corresponding unregister to the next request() call (the unregister can't
// happen from inside the message handler callback). The deferred unregister
// removes *all* handler entries for that message id and, in the message handler,
// deferred registrations are applied before deferred unregistrations. So if a
// request for a message comes in while a previous unregister for the same id is
// still deferred -- and the receive thread is holding the message handler mutex
// (deferring both the unregister and the new register) -- the deferred
// unregister deletes the handler that the new request just registered. That
// request then never receives its message and times out.
//
// A single sequential requester never hits this: the cleanup runs when no one
// holds the message handler mutex. To reproduce we hammer the same request from
// several threads at once while the autopilot streams the response message at a
// high rate (keeping the ground station receive thread inside process_message,
// holding the mutex). Note that even a request that ends up returning Busy first
// runs the cleanup loop, so it too can unregister the in-flight winner's handler.
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

            Json::Value root;
            Json::Reader reader;
            if (!reader.parse(command.fields_json, root)) {
                return;
            }
            const int command_id = root["command"].asInt();

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

    EXPECT_FALSE(bad_result) << "A request lost its handler and did not complete.";
    EXPECT_GT(success_count, 0) << "No request ever succeeded; test did not exercise the path.";
}
