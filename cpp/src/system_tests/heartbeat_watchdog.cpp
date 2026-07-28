#include "mavsdk.hpp"
#include "mavlink_include.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

using namespace mavsdk;
using namespace std::chrono_literals;

// The heartbeat watchdog state machine itself is covered by the fast unit test
// in core/heartbeat_watchdog_test.cpp, which drives it with a FakeTime.
//
// What is left here is the wiring that a unit test cannot reach: that the 1 Hz
// heartbeat tick in MavsdkImpl actually consults the watchdog before sending,
// and that churning the configuration concurrently with system discovery does
// not deadlock. Those need a real Mavsdk and therefore real time, so keep the
// number of cases here to a minimum.

namespace {

// MAVSDK sends heartbeats at 1 Hz, so anything observable costs about a second.
constexpr auto flow_timeout = 2500ms;

// The smallest timeout the watchdog accepts.
constexpr double watchdog_timeout_s = 2.0;

// Once this has elapsed since the last feed, the deadline has certainly
// passed, so no further heartbeat can be sent.
constexpr auto expiry_settled = 2500ms;

template<typename Predicate>
bool wait_for(Predicate predicate, std::chrono::milliseconds timeout = flow_timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) {
            return true;
        }
        std::this_thread::sleep_for(10ms);
    }
    return predicate();
}

// Message ID of a raw MAVLink frame, or -1 if it does not look like one.
int msgid_from_bytes(const char* bytes, size_t length)
{
    const auto* buffer = reinterpret_cast<const uint8_t*>(bytes);

    if (length >= 12 && buffer[0] == 0xFD) {
        // MAVLink v2: 24 bit little endian message ID.
        return int(buffer[7]) | (int(buffer[8]) << 8) | (int(buffer[9]) << 16);
    }
    if (length >= 8 && buffer[0] == 0xFE) {
        // MAVLink v1: single byte message ID.
        return int(buffer[5]);
    }
    return -1;
}

class HeartbeatCounter {
public:
    explicit HeartbeatCounter(Mavsdk& mavsdk) : _mavsdk(mavsdk)
    {
        _handle = _mavsdk.subscribe_raw_bytes_to_be_sent([this](const char* bytes, size_t length) {
            if (msgid_from_bytes(bytes, length) == MAVLINK_MSG_ID_HEARTBEAT) {
                ++_count;
            }
        });
    }

    // Unsubscribe while the Mavsdk instance is still alive, so the callback
    // cannot outlive this object.
    ~HeartbeatCounter() { _mavsdk.unsubscribe_raw_bytes_to_be_sent(_handle); }

    HeartbeatCounter(const HeartbeatCounter&) = delete;
    HeartbeatCounter& operator=(const HeartbeatCounter&) = delete;

    int count() const { return _count.load(); }

    bool wait_for_more_than(int previous) const
    {
        return wait_for([this, previous]() { return count() > previous; });
    }

private:
    Mavsdk& _mavsdk;
    std::atomic<int> _count{0};
    Mavsdk::RawBytesHandle _handle{};
};

Mavsdk::Configuration
ground_station_configuration(bool always_send_heartbeats, double timeout_s = 0.0)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(always_send_heartbeats);
    configuration.set_heartbeat_watchdog_timeout_s(timeout_s);
    return configuration;
}

// A "raw://" connection is required for subscribe_raw_bytes_to_be_sent() and
// pass_received_raw_bytes() to work. Note that adding it force-enables
// always_send_heartbeats, so tests wanting it off must set the configuration
// afterwards.
void add_raw_connection(Mavsdk& mavsdk)
{
    ASSERT_EQ(
        mavsdk.add_any_connection("raw://", ForwardingOption::ForwardingOff),
        ConnectionResult::Success);
}

void inject_autopilot_heartbeat(Mavsdk& mavsdk, uint8_t sysid)
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(
        sysid,
        MAV_COMP_ID_AUTOPILOT1,
        &message,
        MAV_TYPE_QUADROTOR,
        MAV_AUTOPILOT_PX4,
        0,
        0,
        MAV_STATE_ACTIVE);
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    const uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);
    mavsdk.pass_received_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

} // namespace

TEST(HeartbeatWatchdog, GatesHeartbeatsOnTheWire)
{
    // always_send_heartbeats would normally keep heartbeats going forever, so
    // if none appear the tick must be consulting the watchdog.
    Mavsdk mavsdk{ground_station_configuration(true, watchdog_timeout_s)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    std::this_thread::sleep_for(flow_timeout);
    ASSERT_EQ(heartbeats.count(), 0) << "heartbeats were sent without ever feeding the watchdog";

    // Feeding lets them through.
    mavsdk.feed_heartbeat_watchdog();
    ASSERT_TRUE(heartbeats.wait_for_more_than(0));

    // And once feeding stops they cease again.
    std::this_thread::sleep_for(expiry_settled);
    const int count_after_expiry = heartbeats.count();
    std::this_thread::sleep_for(flow_timeout);
    EXPECT_EQ(heartbeats.count(), count_after_expiry);
}

TEST(HeartbeatWatchdog, ConcurrentReconfigurationDoesNotDeadlock)
{
    Mavsdk mavsdk{ground_station_configuration(true)};
    add_raw_connection(mavsdk);

    // Churn the configuration and the watchdog from a separate thread. These
    // calls serialize on the configuration writer lock and take the systems,
    // server components and heartbeat locks individually.
    std::atomic<bool> done{false};
    std::thread configuration_thread([&mavsdk, &done]() {
        bool always_send_heartbeats = true;
        int iteration = 0;
        while (!done) {
            always_send_heartbeats = !always_send_heartbeats;
            mavsdk.set_configuration(ground_station_configuration(always_send_heartbeats));
            mavsdk.set_heartbeat_watchdog_timeout_s(
                (iteration++ % 2 == 0) ? 0.0 : watchdog_timeout_s);
            mavsdk.feed_heartbeat_watchdog();
        }
    });

    // Meanwhile keep injecting heartbeats from new system IDs, so the io
    // thread keeps discovering systems while holding the systems lock. This
    // combination used to be an ABBA deadlock with configuration updates,
    // which held the server components lock while taking the systems lock.
    for (int sysid = 1; sysid <= 150; ++sysid) {
        inject_autopilot_heartbeat(mavsdk, static_cast<uint8_t>(sysid));
        std::this_thread::sleep_for(2ms);
    }

    done = true;
    configuration_thread.join();
}

TEST(HeartbeatWatchdog, DisablingAlwaysSendDuringDiscoveryKeepsHeartbeatsRunning)
{
    // With a system connected, heartbeats must keep ticking even when
    // always_send_heartbeats is turned off: the tick re-checks connectivity
    // each period, so a connect racing a configuration update cannot leave
    // heartbeats stuck off.
    Mavsdk mavsdk{ground_station_configuration(false)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    const auto on_configuration = ground_station_configuration(true);
    const auto off_configuration = ground_station_configuration(false);

    for (uint8_t sysid = 1; sysid <= 3; ++sysid) {
        mavsdk.set_configuration(on_configuration);

        // Keep the already-connected systems alive so they do not hit the
        // heartbeat timeout and disconnect, which would stop heartbeats
        // through an unrelated code path.
        for (uint8_t alive = 1; alive < sysid; ++alive) {
            inject_autopilot_heartbeat(mavsdk, alive);
        }

        // The first heartbeat of a new system only creates it: the system's
        // HEARTBEAT handler registration is posted to the io thread, so this
        // message is not delivered to the system itself and does not connect
        // it yet. Wait for the system to exist, then let the posted handler
        // registration run.
        inject_autopilot_heartbeat(mavsdk, sysid);
        ASSERT_TRUE(wait_for([&]() { return mavsdk.systems().size() >= sysid; }));
        std::this_thread::sleep_for(100ms);

        // The second heartbeat connects the system, racing the policy-off
        // update below.
        inject_autopilot_heartbeat(mavsdk, sysid);
        mavsdk.set_configuration(off_configuration);

        // At least one system is connected (or about to finish connecting), so
        // the policy still requires heartbeats: they must keep ticking.
        EXPECT_TRUE(heartbeats.wait_for_more_than(heartbeats.count()))
            << "heartbeats stopped although a system is connected (iteration "
            << static_cast<int>(sysid) << ")";
    }
}
