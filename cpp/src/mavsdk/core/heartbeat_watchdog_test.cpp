#include "mavsdk.hpp"
#include "mavlink_include.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <limits>
#include <thread>

using namespace mavsdk;
using namespace std::chrono_literals;

namespace {

// These tests drive the real 1 Hz heartbeat tick, so every wait below is real
// time. Two consequences worth keeping in mind when adjusting timings:
//
// - A heartbeat is only sent when the tick runs, so "heartbeats are flowing"
//   can take up to one period to observe.
// - The watchdog deadline is evaluated when a heartbeat is due, so an expiry
//   becomes observable up to one period after the deadline itself passed.
//
// Everything goes through the public Mavsdk API on a "raw://" connection:
// subscribe_raw_bytes_to_be_sent() reports what MAVSDK sends, and
// pass_received_raw_bytes() feeds messages in.

constexpr auto heartbeat_period = 1s;

// Long enough for a due heartbeat to be observed.
constexpr auto flow_timeout = 2500ms;

// Watchdog timeout used throughout. Deliberately more than one heartbeat
// period: at the 1 s minimum a single feed and the tick can be almost in
// phase, so whether the next heartbeat still makes the deadline would be a
// coin flip. That is real behaviour (see the Heartbeat Watchdog guide), not
// something the tests should race against.
constexpr double watchdog_timeout_s = 2.0;

// Once this has elapsed since the last feed, the deadline has certainly
// passed, so no further heartbeat can be sent.
constexpr auto expiry_settled = 2500ms;

// Window over which a "heartbeats stay off" assertion is checked.
constexpr auto stays_off_window = 2s;

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

    // Checks that no heartbeat is sent for stays_off_window.
    void expect_stays_off() const
    {
        const int before = count();
        std::this_thread::sleep_for(stays_off_window);
        EXPECT_EQ(count(), before);
    }

private:
    Mavsdk& _mavsdk;
    std::atomic<int> _count{0};
    Mavsdk::RawBytesHandle _handle{};
};

// Feeds the watchdog from its own thread until destroyed.
class WatchdogFeeder {
public:
    WatchdogFeeder(Mavsdk& mavsdk, std::chrono::milliseconds interval) :
        _thread([this, &mavsdk, interval]() {
            while (!_stop) {
                mavsdk.feed_heartbeat_watchdog();
                std::this_thread::sleep_for(interval);
            }
        })
    {}

    ~WatchdogFeeder()
    {
        _stop = true;
        _thread.join();
    }

    WatchdogFeeder(const WatchdogFeeder&) = delete;
    WatchdogFeeder& operator=(const WatchdogFeeder&) = delete;

private:
    std::atomic<bool> _stop{false};
    std::thread _thread;
};

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
    // Channel 0 is fine here: mavlink_get_channel_status() is 'static inline',
    // so its status array is private to this translation unit and cannot
    // collide with MAVSDK's own packing inside the library.
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

Mavsdk::Configuration
ground_station_configuration(bool always_send_heartbeats, double timeout_s = 0.0)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(always_send_heartbeats);
    configuration.set_heartbeat_watchdog_timeout_s(timeout_s);
    return configuration;
}

} // namespace

TEST(Heartbeat, DisablingAlwaysSendStopsHeartbeatsWhenNoSystemConnected)
{
    // Heartbeats are forced on via always_send_heartbeats, with no watchdog and
    // no connected system.
    const auto configuration = ground_station_configuration(true);

    Mavsdk mavsdk{configuration};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // Confirm heartbeats are actually being sent.
    ASSERT_TRUE(heartbeats.wait_for_more_than(0));
    ASSERT_TRUE(heartbeats.wait_for_more_than(1));

    // Disabling always_send_heartbeats with no connected system must stop them.
    mavsdk.set_configuration(ground_station_configuration(false));

    // Allow an in-flight tick to complete before sampling.
    std::this_thread::sleep_for(heartbeat_period);
    heartbeats.expect_stays_off();
}

TEST(Heartbeat, DisablingAlwaysSendDuringDiscoveryKeepsHeartbeatsRunning)
{
    // With a system connected, heartbeats must keep ticking even when
    // always_send_heartbeats is turned off: the lifetime CallEvery tick
    // re-checks connectivity each period, so a concurrent connect during a
    // configuration update cannot leave heartbeats stuck off.
    Mavsdk mavsdk{ground_station_configuration(false)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    const auto on_configuration = ground_station_configuration(true);
    const auto off_configuration = ground_station_configuration(false);

    for (uint8_t sysid = 1; sysid <= 5; ++sysid) {
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

        // At least one system is connected (or about to finish connecting),
        // so the policy still requires heartbeats: they must keep ticking.
        EXPECT_TRUE(heartbeats.wait_for_more_than(heartbeats.count()))
            << "heartbeats stopped although a system is connected (iteration "
            << static_cast<int>(sysid) << ")";
    }
}

TEST(HeartbeatWatchdog, ConfigurationStoresAndValidatesTimeout)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    EXPECT_DOUBLE_EQ(configuration.get_heartbeat_watchdog_timeout_s(), 0.0);

    EXPECT_TRUE(configuration.set_heartbeat_watchdog_timeout_s(2.5));
    EXPECT_DOUBLE_EQ(configuration.get_heartbeat_watchdog_timeout_s(), 2.5);

    // Sub-second, negative and non-finite values are rejected and leave the
    // stored value unchanged.
    EXPECT_FALSE(configuration.set_heartbeat_watchdog_timeout_s(0.5));
    EXPECT_FALSE(configuration.set_heartbeat_watchdog_timeout_s(-1.0));
    EXPECT_FALSE(
        configuration.set_heartbeat_watchdog_timeout_s(std::numeric_limits<double>::infinity()));
    EXPECT_FALSE(
        configuration.set_heartbeat_watchdog_timeout_s(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_DOUBLE_EQ(configuration.get_heartbeat_watchdog_timeout_s(), 2.5);

    // 0 (disabled) is valid.
    EXPECT_TRUE(configuration.set_heartbeat_watchdog_timeout_s(0.0));
    EXPECT_DOUBLE_EQ(configuration.get_heartbeat_watchdog_timeout_s(), 0.0);
}

TEST(HeartbeatWatchdog, RejectsSubSecondTimeout)
{
    Mavsdk mavsdk{ground_station_configuration(true, watchdog_timeout_s)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    {
        WatchdogFeeder feeder{mavsdk, 250ms};
        ASSERT_TRUE(heartbeats.wait_for_more_than(0));

        // Rejected: must not shorten the active watchdog.
        EXPECT_FALSE(mavsdk.set_heartbeat_watchdog_timeout_s(0.5));

        // The original timeout is still in force, so heartbeats keep coming.
        EXPECT_TRUE(heartbeats.wait_for_more_than(heartbeats.count()));
    }

    // Once feeding stops the watchdog expires and heartbeats stop.
    std::this_thread::sleep_for(expiry_settled);
    heartbeats.expect_stays_off();
}

TEST(HeartbeatWatchdog, FeedWithoutWatchdogConfiguredIsNoOp)
{
    Mavsdk mavsdk{ground_station_configuration(true)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // Without a watchdog configured, feeding is a no-op and heartbeats just
    // keep being sent periodically.
    mavsdk.feed_heartbeat_watchdog();

    ASSERT_TRUE(heartbeats.wait_for_more_than(0));
    EXPECT_TRUE(heartbeats.wait_for_more_than(1));
}

TEST(HeartbeatWatchdog, NoHeartbeatsOnStartupUntilFed)
{
    // With the watchdog configured, heartbeats must not start on their own at
    // startup (e.g. when always_send_heartbeats is set) until the watchdog has
    // been fed at least once.
    Mavsdk mavsdk{ground_station_configuration(true, watchdog_timeout_s)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    heartbeats.expect_stays_off();
    ASSERT_EQ(heartbeats.count(), 0);

    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(heartbeats.wait_for_more_than(0));
}

TEST(HeartbeatWatchdog, FeedDoesNotStartHeartbeatsThatNeverRan)
{
    // Watchdog configured, but heartbeats never started: no discovered system
    // and always_send_heartbeats off.
    Mavsdk mavsdk{ground_station_configuration(false, watchdog_timeout_s)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);
    // Undo the always_send_heartbeats that adding the connection forced on.
    mavsdk.set_configuration(ground_station_configuration(false, watchdog_timeout_s));

    // Feeding only resets the watchdog, it must not act as a start trigger.
    {
        WatchdogFeeder feeder{mavsdk, 250ms};
        heartbeats.expect_stays_off();
    }

    EXPECT_EQ(heartbeats.count(), 0);
}

TEST(HeartbeatWatchdog, FeedRespectsHeartbeatPolicy)
{
    const auto policy_on = ground_station_configuration(true, watchdog_timeout_s);
    const auto policy_off = ground_station_configuration(false, watchdog_timeout_s);

    Mavsdk mavsdk{policy_on};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // Never feed: the watchdog expires and heartbeats stay off.
    std::this_thread::sleep_for(expiry_settled);

    // Turn the policy off (no always_send_heartbeats, no connected system).
    mavsdk.set_configuration(policy_off);

    // Feeding now must not restart heartbeats: they are not supposed to be
    // sent while the policy is off. Waiting past the deadline expires the feed.
    mavsdk.feed_heartbeat_watchdog();
    std::this_thread::sleep_for(expiry_settled);
    const int count_with_policy_off = heartbeats.count();

    // The unused feed has expired: turning the policy back on must not resume
    // heartbeats without another feed.
    mavsdk.set_configuration(policy_on);
    heartbeats.expect_stays_off();

    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(heartbeats.wait_for_more_than(count_with_policy_off));
}

TEST(HeartbeatWatchdog, RuntimeTimeoutReconfiguration)
{
    Mavsdk mavsdk{ground_station_configuration(true, watchdog_timeout_s)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // Never feed: heartbeats stay off.
    std::this_thread::sleep_for(expiry_settled);
    const int count_after_expiry = heartbeats.count();

    // Disabling the watchdog: heartbeats resume without a feed and run
    // unconditionally.
    EXPECT_TRUE(mavsdk.set_heartbeat_watchdog_timeout_s(0));
    ASSERT_TRUE(heartbeats.wait_for_more_than(count_after_expiry));

    // Re-enabling the watchdog must stop heartbeats and keep them off until a
    // feed - never grant a free timeout period without a feed.
    EXPECT_TRUE(mavsdk.set_heartbeat_watchdog_timeout_s(watchdog_timeout_s));
    std::this_thread::sleep_for(heartbeat_period);
    heartbeats.expect_stays_off();

    // The re-enabled watchdog still reacts to a feed.
    const int count_before_feed = heartbeats.count();
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(heartbeats.wait_for_more_than(count_before_feed));
}

TEST(HeartbeatWatchdog, EnableAtRuntimeStopsHeartbeatsUntilFed)
{
    // Watchdog disabled at startup; enabled later via
    // set_heartbeat_watchdog_timeout_s(), as mavsdk_server does for the
    // SetHeartbeatWatchdogTimeout RPC.
    Mavsdk mavsdk{ground_station_configuration(true)};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // Autonomous heartbeats while the watchdog is off.
    ASSERT_TRUE(heartbeats.wait_for_more_than(0));

    // Enable the watchdog at runtime: heartbeats stop and stay off until a feed.
    EXPECT_TRUE(mavsdk.set_heartbeat_watchdog_timeout_s(watchdog_timeout_s));
    std::this_thread::sleep_for(heartbeat_period);
    heartbeats.expect_stays_off();

    const int count_before_feed = heartbeats.count();
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(heartbeats.wait_for_more_than(count_before_feed));
}

TEST(HeartbeatWatchdog, ConcurrentReconfigurationStress)
{
    const auto configuration = ground_station_configuration(true);

    Mavsdk mavsdk{configuration};
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
            mavsdk.set_heartbeat_watchdog_timeout_s((iteration++ % 2 == 0) ? 0.0 : 1.0);
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

TEST(HeartbeatWatchdog, FeedRemainsValidAcrossPolicyOff)
{
    const auto policy_on = ground_station_configuration(true, watchdog_timeout_s);
    const auto policy_off = ground_station_configuration(false, watchdog_timeout_s);

    Mavsdk mavsdk{policy_on};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // Feed so heartbeats start and arm the deadman.
    mavsdk.feed_heartbeat_watchdog();
    ASSERT_TRUE(heartbeats.wait_for_more_than(0));

    // Refresh the deadline, then stop heartbeats by turning the policy off
    // (no system is connected).
    mavsdk.feed_heartbeat_watchdog();
    mavsdk.set_configuration(policy_off);

    // Allow an in-flight tick to complete before sampling.
    std::this_thread::sleep_for(heartbeat_period);
    const int count_while_policy_off = heartbeats.count();

    // Still within the watchdog deadline, so turning the policy back on must
    // resume heartbeats without another feed.
    mavsdk.set_configuration(policy_on);
    EXPECT_TRUE(heartbeats.wait_for_more_than(count_while_policy_off));
}

TEST(HeartbeatWatchdog, FeedRemainsValidAcrossSystemDisconnect)
{
    // Heartbeats are gated by a connected system (always_send off). A feed
    // must stay valid across disconnect/reconnect until the deadline.
    const auto configuration = ground_station_configuration(false, 5.0);

    Mavsdk mavsdk{configuration};
    // Shorten the system heartbeat timeout so the disconnect below is quick.
    mavsdk.set_heartbeat_timeout_s(1.0);
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);
    // Undo the always_send_heartbeats that adding the connection forced on.
    mavsdk.set_configuration(configuration);

    // Discover and connect a system (first heartbeat creates it, second
    // connects after the handler is registered).
    inject_autopilot_heartbeat(mavsdk, 1);
    ASSERT_TRUE(wait_for([&]() { return mavsdk.systems().size() >= 1; }));
    std::this_thread::sleep_for(100ms);
    inject_autopilot_heartbeat(mavsdk, 1);
    ASSERT_TRUE(wait_for([&]() { return mavsdk.systems()[0]->is_connected(); }));

    mavsdk.feed_heartbeat_watchdog();
    ASSERT_TRUE(heartbeats.wait_for_more_than(0));

    // Refresh the deadline, then let the system disconnect by not injecting
    // any further heartbeats.
    mavsdk.feed_heartbeat_watchdog();
    ASSERT_TRUE(wait_for([&]() { return !mavsdk.systems()[0]->is_connected(); }));
    const int count_while_disconnected = heartbeats.count();

    // Reconnect within the watchdog deadline: heartbeats must resume without
    // another feed.
    inject_autopilot_heartbeat(mavsdk, 1);
    ASSERT_TRUE(wait_for([&]() { return mavsdk.systems()[0]->is_connected(); }));
    EXPECT_TRUE(heartbeats.wait_for_more_than(count_while_disconnected));
}

TEST(HeartbeatWatchdog, ExpiryStopsHeartbeatsAndFeedRestartsThem)
{
    const auto configuration = ground_station_configuration(true, watchdog_timeout_s);

    Mavsdk mavsdk{configuration};
    HeartbeatCounter heartbeats{mavsdk};
    add_raw_connection(mavsdk);

    // While the watchdog keeps being fed, periodic heartbeats keep coming.
    {
        WatchdogFeeder feeder{mavsdk, 250ms};
        ASSERT_TRUE(heartbeats.wait_for_more_than(0));
        ASSERT_TRUE(heartbeats.wait_for_more_than(1));
    }

    // Once we stop feeding, the watchdog expires and heartbeats stop.
    std::this_thread::sleep_for(expiry_settled);
    const int count_after_expiry = heartbeats.count();

    // After expiry, even toggling always_send_heartbeats (which normally
    // starts them) must not revive them until a feed.
    mavsdk.set_configuration(ground_station_configuration(false, watchdog_timeout_s));
    mavsdk.set_configuration(configuration);
    heartbeats.expect_stays_off();

    // Feeding the watchdog allows heartbeats to resume.
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(heartbeats.wait_for_more_than(count_after_expiry));
}
