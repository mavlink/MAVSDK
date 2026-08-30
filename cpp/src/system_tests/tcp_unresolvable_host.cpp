#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"

#include <atomic>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

using namespace mavsdk;
using namespace std::chrono_literals;

// A TCP client pointed at a name that cannot be resolved must keep retrying quietly and
// must not disturb anything else in the SDK.
//
// This covers the async_resolve path end to end: the failing branch has to reschedule the
// reconnect, and stop() has to cancel an in-flight resolve without leaving a handler
// behind (the teardown at the end of this test would trip over that).
//
// Note on what this does *not* prove: the reason for moving off the synchronous resolve()
// is a DNS server that hangs rather than one that answers NXDOMAIN, which is what a
// .invalid name gives us. Making a lookup actually hang needs an unreachable resolver,
// which is not something a test can arrange portably, so the "does not block the io
// thread" property is argued in the commit message rather than asserted here.
TEST(Connections, UnresolvableTcpHostDoesNotDisturbOtherConnections)
{
    const int udp_port = 17422;

    Mavsdk autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    // .invalid is reserved by RFC 2606 and is guaranteed never to resolve.
    ASSERT_EQ(
        autopilot.add_any_connection("tcpout://this-host-does-not-exist.invalid:17423"),
        ConnectionResult::Success);
    ASSERT_EQ(
        autopilot.add_any_connection("udpout://127.0.0.1:" + std::to_string(udp_port)),
        ConnectionResult::Success);

    Mavsdk ground_station{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(
        ground_station.add_any_connection("udpin://0.0.0.0:" + std::to_string(udp_port)),
        ConnectionResult::Success);

    auto maybe_system = ground_station.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "Ground station did not discover the autopilot over UDP "
                                 "while a TCP connection was stuck retrying a bad name";

    std::atomic<int> heartbeats{0};
    MavlinkDirect gcs_mavlink{maybe_system.value()};
    auto handle = gcs_mavlink.subscribe_message(
        "HEARTBEAT", [&heartbeats](MavlinkDirect::MavlinkMessage) { ++heartbeats; });

    // Several reconnect attempts happen in this window (the retry timer is 1 s), so the
    // resolve failure path is exercised repeatedly rather than just once.
    const int heartbeats_before = heartbeats;
    std::this_thread::sleep_for(3s);
    const int heartbeats_after = heartbeats;

    gcs_mavlink.unsubscribe_message(handle);

    LogInfo(
        "Heartbeats received while the TCP name kept failing to resolve: {}",
        heartbeats_after - heartbeats_before);

    EXPECT_GT(heartbeats_after, heartbeats_before)
        << "The UDP link went quiet while a TCP connection was retrying an unresolvable "
           "host";

    // Tearing down here is the other half of the test: stop() has to cancel a resolve
    // that may be in flight right now, and a leaked handler would show up as a hang or a
    // use-after-free under the sanitizers.
}
