#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#include <gtest/gtest.h>

// The stall is produced with a pseudo-terminal, so this is POSIX-only. Nothing about the
// behaviour under test is platform-specific; only the way we wedge a link is.
#if defined(LINUX) || defined(APPLE)

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

using namespace mavsdk;
using namespace std::chrono_literals;

namespace {

// One end of a pseudo-terminal pair, used as a serial link that nobody ever drains.
//
// A pty's buffer is a fixed few kilobytes and does not grow. That makes it a reliable way
// to wedge a writer after a small, predictable amount of traffic -- unlike a loopback TCP
// socket, whose send buffer the kernel auto-tunes into the megabytes, which would need
// megabytes of traffic to fill and would vary by platform.
class UndrainedPty {
public:
    UndrainedPty() = default;
    ~UndrainedPty()
    {
        if (_master_fd >= 0) {
            ::close(_master_fd);
        }
    }

    UndrainedPty(const UndrainedPty&) = delete;
    UndrainedPty& operator=(const UndrainedPty&) = delete;

    bool open()
    {
        _master_fd = posix_openpt(O_RDWR | O_NOCTTY);
        if (_master_fd < 0) {
            LogErr("posix_openpt failed");
            return false;
        }
        if (grantpt(_master_fd) != 0 || unlockpt(_master_fd) != 0) {
            LogErr("grantpt/unlockpt failed");
            return false;
        }
        const char* name = ptsname(_master_fd);
        if (name == nullptr) {
            LogErr("ptsname failed");
            return false;
        }
        _slave_path = name;
        return true;
    }

    // The path MAVSDK connects to. We hold the master open and never read from it, so
    // everything MAVSDK writes piles up until the pty buffer is full.
    const std::string& slave_path() const { return _slave_path; }

private:
    int _master_fd{-1};
    std::string _slave_path{};
};

} // namespace

// A link whose peer stops reading must not stall anything else in the SDK.
//
// The autopilot has two connections: a serial link to a pty nobody drains, and a UDP link
// to a real ground station. Once the pty buffer is full, a *synchronous* write to it
// blocks the io thread indefinitely -- and because that one thread also drives the UDP
// connection, every internal timer and all the plugin state machines, the ground station
// stops hearing anything at all.
//
// With sends queued and drained by an async write chain, the wedged serial link only
// backs up its own queue and the UDP side keeps running.
TEST(Connections, StalledLinkDoesNotStallOtherConnections)
{
    const int udp_port = 17421;

    UndrainedPty pty;
    ASSERT_TRUE(pty.open()) << "Could not create a pseudo-terminal";
    LogInfo("Serial link that will never be drained: {}", pty.slave_path());

    Mavsdk autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    ASSERT_EQ(
        autopilot.add_any_connection("serial://" + pty.slave_path() + ":57600"),
        ConnectionResult::Success);
    ASSERT_EQ(
        autopilot.add_any_connection("udpout://127.0.0.1:" + std::to_string(udp_port)),
        ConnectionResult::Success);

    Mavsdk ground_station{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(
        ground_station.add_any_connection("udpin://0.0.0.0:" + std::to_string(udp_port)),
        ConnectionResult::Success);

    auto maybe_system = ground_station.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "Ground station did not discover the autopilot over UDP";

    // MAVSDK sends a heartbeat every second by itself, so this keeps ticking on its own
    // for as long as the io thread is alive to do it.
    std::atomic<int> heartbeats{0};
    MavlinkDirect gcs_mavlink{maybe_system.value()};
    auto handle = gcs_mavlink.subscribe_message(
        "HEARTBEAT", [&heartbeats](MavlinkDirect::MavlinkMessage) { ++heartbeats; });

    // Flood both connections. These messages are untargeted, so they go out on every
    // connection -- including the serial link that will never drain them.
    MavlinkDirectServer sender{autopilot.server_component()};
    MavlinkDirectServer::MavlinkMessage message;
    message.message_name = "GLOBAL_POSITION_INT";
    message.system_id = 1;
    message.component_id = 1;
    message.target_system_id = 0;
    message.target_component_id = 0;
    message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,)"
        R"("relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    // A GLOBAL_POSITION_INT is about 40 bytes on the wire and a pty buffer is a few
    // kilobytes, so this is comfortably more than the link can absorb -- and still well
    // inside the per-connection send queue, so nothing has to be dropped.
    constexpr int flood_count = 500;
    for (int i = 0; i < flood_count; ++i) {
        EXPECT_EQ(sender.send_message(message), MavlinkDirectServer::Result::Success);
    }
    LogInfo("Sent {} messages into the undrained serial link.", flood_count);

    // Let the io thread work through the flood -- or, without the fix, wedge on it.
    std::this_thread::sleep_for(1s);

    LogInfo("Checking that the UDP link is still alive...");
    const int heartbeats_before = heartbeats;
    std::this_thread::sleep_for(3s);
    const int heartbeats_after = heartbeats;

    gcs_mavlink.unsubscribe_message(handle);

    LogInfo(
        "Heartbeats received while the serial link was stalled: {}",
        heartbeats_after - heartbeats_before);

    EXPECT_GT(heartbeats_after, heartbeats_before)
        << "The ground station stopped receiving heartbeats over UDP once a serial peer "
           "stopped reading, so the io thread is stuck on a blocking write.";
}

#endif // defined(LINUX) || defined(APPLE)
