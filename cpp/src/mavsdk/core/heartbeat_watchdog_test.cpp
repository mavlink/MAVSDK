#include "mavsdk_impl.hpp"
#include "mavsdk_time.hpp"
#include "mavlink_include.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <limits>
#include <thread>

using namespace mavsdk;

namespace {

// These tests run against MavsdkImpl with an injected FakeTime, so all
// heartbeat and watchdog periods are simulated: fake_time.sleep_for()
// advances the clock instantly. The io thread polls the timer handlers every
// 5 ms of real time, so after advancing the fake time, effects appear within
// a few milliseconds. Real-time waits below only bound that scheduling
// latency, never a simulated duration.

// Real-time wait for the io thread to act on an advanced fake clock.
template<typename Predicate>
bool wait_for(
    Predicate predicate, std::chrono::milliseconds timeout = std::chrono::milliseconds(3000))
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return predicate();
}

// Let the io thread run its timer poll several times, so that negative
// assertions ("nothing happened after advancing time") are meaningful.
void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

class HeartbeatCounter {
public:
    explicit HeartbeatCounter(MavsdkImpl& mavsdk)
    {
        mavsdk.intercept_outgoing_messages_async([this](mavlink_message_t& message) {
            if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                ++_count;
            }
            return true;
        });
    }

    int count() const { return _count.load(); }

private:
    std::atomic<int> _count{0};
};

} // namespace

TEST(Heartbeat, DisablingAlwaysSendStopsHeartbeatsWhenNoSystemConnected)
{
    // Heartbeats are forced on via always_send_heartbeats, with no watchdog and
    // no connected system.
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Confirm heartbeats are actually being sent (first one is immediate, the
    // second needs a simulated second to pass).
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));
    fake_time.sleep_for(std::chrono::milliseconds(1100));
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 2; }));

    // Disabling always_send_heartbeats with no connected system must stop them.
    auto disabled_configuration = configuration;
    disabled_configuration.set_always_send_heartbeats(false);
    mavsdk.set_configuration(disabled_configuration);

    settle();
    const int count_after_disable = heartbeats.count();
    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), count_after_disable);
}

TEST(Heartbeat, DisablingAlwaysSendDuringDiscoveryKeepsHeartbeatsRunning)
{
    // Regression test for a stale-snapshot race: set_configuration() turning
    // always_send_heartbeats off evaluates is_any_system_connected() before
    // stopping. A system finishing its connection in that window (its
    // heartbeat start is posted to the io thread) could have its
    // just-started heartbeats stopped, leaving them off although a system is
    // connected. The interleaving cannot be forced deterministically, so
    // each iteration opens the window once (a system only posts that start
    // on the heartbeat that connects it) and then asserts the invariant:
    // with a system connected, heartbeats keep ticking even with always_send
    // off.
    Mavsdk::Configuration configuration{ComponentType::GroundStation};

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};
    // Note: adding the raw connection force-enables always_send_heartbeats.
    ASSERT_EQ(
        mavsdk.add_any_connection("raw://", ForwardingOption::ForwardingOff).first,
        ConnectionResult::Success);

    auto inject_heartbeat = [&mavsdk](uint8_t sysid) {
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
    };

    auto on_configuration = configuration;
    on_configuration.set_always_send_heartbeats(true);
    auto off_configuration = configuration;
    off_configuration.set_always_send_heartbeats(false);

    for (uint8_t sysid = 1; sysid <= 30; ++sysid) {
        mavsdk.set_configuration(on_configuration);

        // Keep the already-connected systems alive: each iteration advances
        // the fake clock by 1.1 s, so without a fresh heartbeat they would
        // eventually hit the heartbeat timeout and disconnect, which stops
        // heartbeats through an unrelated code path.
        for (uint8_t alive = 1; alive < sysid; ++alive) {
            inject_heartbeat(alive);
        }

        // The first heartbeat of a new system only creates it: the system's
        // HEARTBEAT handler registration is posted to the io thread, so this
        // message is not delivered to the system itself and does not connect
        // it yet. Wait for the system to exist, then let the posted handler
        // registration run.
        inject_heartbeat(sysid);
        ASSERT_TRUE(wait_for([&]() { return mavsdk.systems().size() >= sysid; }));
        settle();

        // The second heartbeat connects the system: set_connected() posts a
        // start_sending_heartbeats() to the io thread, racing the policy-off
        // update below.
        inject_heartbeat(sysid);
        mavsdk.set_configuration(off_configuration);

        // At least one system is connected (or about to finish connecting),
        // so the policy still requires heartbeats: they must keep ticking.
        const int count_before = heartbeats.count();
        fake_time.sleep_for(std::chrono::milliseconds(1100));
        EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_before; }))
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
    EXPECT_FALSE(configuration.set_heartbeat_watchdog_timeout_s(
        std::numeric_limits<double>::infinity()));
    EXPECT_FALSE(
        configuration.set_heartbeat_watchdog_timeout_s(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_DOUBLE_EQ(configuration.get_heartbeat_watchdog_timeout_s(), 2.5);

    // 0 (disabled) is valid.
    EXPECT_TRUE(configuration.set_heartbeat_watchdog_timeout_s(0.0));
    EXPECT_DOUBLE_EQ(configuration.get_heartbeat_watchdog_timeout_s(), 0.0);
}

TEST(HeartbeatWatchdog, RejectsSubSecondTimeout)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);
    configuration.set_heartbeat_watchdog_timeout_s(2.0);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));

    // Rejected: must not shorten the active 2 s watchdog to 0.5 s.
    EXPECT_FALSE(mavsdk.set_heartbeat_watchdog_timeout_s(0.5));

    // 1.2 s after the feed: had the 0.5 s timeout been accepted, the watchdog
    // would have expired before the 1 s heartbeat tick. With the 2 s timeout
    // still active, the tick goes through.
    const int count_before_tick = heartbeats.count();
    fake_time.sleep_for(std::chrono::milliseconds(1200));
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_before_tick; }));

    // After the 2 s watchdog expires, heartbeats stop.
    fake_time.sleep_for(std::chrono::milliseconds(1000));
    settle();
    const int count_after_expiry = heartbeats.count();
    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), count_after_expiry);
}

TEST(HeartbeatWatchdog, FeedWithoutWatchdogConfiguredIsNoOp)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Without a watchdog configured, feeding is a no-op and heartbeats just
    // keep being sent periodically.
    mavsdk.feed_heartbeat_watchdog();

    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));
    fake_time.sleep_for(std::chrono::milliseconds(1100));
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 2; }));
}

TEST(HeartbeatWatchdog, NoHeartbeatsOnStartupUntilFed)
{
    // With the watchdog configured, heartbeats must not start on their own at
    // startup (e.g. when a system is discovered or always_send_heartbeats is
    // set) until the watchdog has been fed at least once.
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);
    configuration.set_heartbeat_watchdog_timeout_s(1);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), 0);

    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));
}

TEST(HeartbeatWatchdog, FeedDoesNotStartHeartbeatsThatNeverRan)
{
    // Watchdog configured, but heartbeats never started: no connection, no
    // discovered system, and always_send_heartbeats off.
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_heartbeat_watchdog_timeout_s(1);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Feeding only resets the watchdog, it must not act as a start trigger.
    for (int i = 0; i < 5; ++i) {
        mavsdk.feed_heartbeat_watchdog();
        fake_time.sleep_for(std::chrono::milliseconds(500));
        settle();
    }

    EXPECT_EQ(heartbeats.count(), 0);
}

TEST(HeartbeatWatchdog, FeedRespectsHeartbeatPolicy)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);
    configuration.set_heartbeat_watchdog_timeout_s(1);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Never feed: the watchdog expires and latches heartbeats off.
    fake_time.sleep_for(std::chrono::milliseconds(1200));
    settle();

    // Turn the policy off (no always_send_heartbeats, no connected system).
    auto policy_off_configuration = configuration;
    policy_off_configuration.set_always_send_heartbeats(false);
    mavsdk.set_configuration(policy_off_configuration);
    const int count_with_policy_off = heartbeats.count();

    // Feeding now must not restart heartbeats: they are not supposed to be
    // sent while the policy is off.
    mavsdk.feed_heartbeat_watchdog();
    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), count_with_policy_off);

    // The feed cleared the latch though: once the policy allows heartbeats
    // again, they start without another feed.
    mavsdk.set_configuration(configuration);
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_with_policy_off; }));
}

TEST(HeartbeatWatchdog, RuntimeTimeoutReconfiguration)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);
    configuration.set_heartbeat_watchdog_timeout_s(1);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Never feed: the watchdog expires and latches heartbeats off.
    fake_time.sleep_for(std::chrono::milliseconds(1200));
    settle();
    const int count_latched = heartbeats.count();

    // Disabling the watchdog (timeout 0) clears the latch: heartbeats resume
    // without a feed and run unconditionally.
    EXPECT_TRUE(mavsdk.set_heartbeat_watchdog_timeout_s(0));
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_latched; }));

    // Re-enabling the watchdog must stop heartbeats immediately and keep them
    // off until a feed - never grant a free timeout period without a feed.
    settle();
    const int count_before_reenable = heartbeats.count();
    EXPECT_TRUE(mavsdk.set_heartbeat_watchdog_timeout_s(1));
    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), count_before_reenable);

    // The re-enabled watchdog still reacts to a feed.
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_before_reenable; }));
}

TEST(HeartbeatWatchdog, EnableViaGrpcStyleApiOnStartup)
{
    // Watchdog disabled at startup; enable at runtime via set_heartbeat_watchdog_timeout_s().
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Autonomous heartbeats while the watchdog is off.
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));

    // Enable the watchdog at runtime: heartbeats stop immediately and stay
    // off until a feed.
    settle();
    const int count_before_enable = heartbeats.count();
    EXPECT_TRUE(mavsdk.set_heartbeat_watchdog_timeout_s(1));
    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), count_before_enable);

    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_before_enable; }));
}

TEST(HeartbeatWatchdog, ConcurrentReconfigurationStress)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    ASSERT_EQ(
        mavsdk.add_any_connection("raw://", ForwardingOption::ForwardingOff).first,
        ConnectionResult::Success);

    // Churn the configuration and the watchdog from a separate thread. These
    // calls serialize on the configuration writer lock and take the systems,
    // server components and heartbeat locks individually.
    std::atomic<bool> done{false};
    std::thread configuration_thread([&mavsdk, &configuration, &done]() {
        auto toggled_configuration = configuration;
        bool always_send_heartbeats = true;
        int iteration = 0;
        while (!done) {
            always_send_heartbeats = !always_send_heartbeats;
            toggled_configuration.set_always_send_heartbeats(always_send_heartbeats);
            mavsdk.set_configuration(toggled_configuration);
            mavsdk.set_heartbeat_watchdog_timeout_s((iteration++ % 2 == 0) ? 0.0 : 1.0);
            mavsdk.feed_heartbeat_watchdog();
        }
    });

    // Meanwhile keep injecting heartbeats from new system IDs, so the io
    // thread keeps discovering systems and calling start_sending_heartbeats()
    // while holding the systems lock. This combination used to be an ABBA
    // deadlock with configuration updates, which held the server components
    // lock while taking the systems lock.
    for (int sysid = 1; sysid <= 150; ++sysid) {
        mavlink_message_t message;
        mavlink_msg_heartbeat_pack(
            static_cast<uint8_t>(sysid),
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
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    done = true;
    configuration_thread.join();
}

TEST(HeartbeatWatchdog, StopLatchesHeartbeatsOffUntilFed)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);
    configuration.set_heartbeat_watchdog_timeout_s(2);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // Feed so heartbeats start.
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));

    // Feed again so the watchdog is not expired, then stop heartbeats by
    // turning the policy off (no system is connected).
    mavsdk.feed_heartbeat_watchdog();
    auto policy_off_configuration = configuration;
    policy_off_configuration.set_always_send_heartbeats(false);
    mavsdk.set_configuration(policy_off_configuration);

    // Give a possibly in-flight heartbeat tick time to drain, then snapshot.
    settle();
    const int count_latched = heartbeats.count();

    // Turning the policy back on must not restart heartbeats: the stop
    // latched them off until the next feed, even though the watchdog had
    // not expired. Otherwise a dead client would get a fresh watchdog
    // period of heartbeats after a reconnect. The check window (1.5 s) is
    // shorter than the watchdog timeout (2 s), so without the latch the
    // restarted heartbeats would be observed here.
    mavsdk.set_configuration(configuration);

    fake_time.sleep_for(std::chrono::milliseconds(1500));
    settle();
    EXPECT_EQ(heartbeats.count(), count_latched);

    // Feeding clears the latch and restarts heartbeats.
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_latched; }));
}

TEST(HeartbeatWatchdog, ExpiryStopsHeartbeatsAndFeedRestartsThem)
{
    Mavsdk::Configuration configuration{ComponentType::GroundStation};
    configuration.set_always_send_heartbeats(true);
    configuration.set_heartbeat_watchdog_timeout_s(1);

    FakeTime fake_time;
    MavsdkImpl mavsdk{configuration, fake_time};
    HeartbeatCounter heartbeats{mavsdk};

    // While the watchdog keeps being fed, periodic heartbeats keep coming
    // (they are sent every simulated second).
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() >= 1; }));
    for (int i = 0; i < 4; ++i) {
        mavsdk.feed_heartbeat_watchdog();
        fake_time.sleep_for(std::chrono::milliseconds(600));
        settle();
    }
    EXPECT_GE(heartbeats.count(), 2);

    // Once we stop feeding, the watchdog expires and heartbeats stop.
    fake_time.sleep_for(std::chrono::milliseconds(1200));
    settle();
    const int count_after_expiry = heartbeats.count();

    // Expiry latches heartbeats off: even toggling always_send_heartbeats
    // (which normally starts them) must not revive them.
    auto toggled_configuration = configuration;
    toggled_configuration.set_always_send_heartbeats(false);
    mavsdk.set_configuration(toggled_configuration);
    mavsdk.set_configuration(configuration);

    fake_time.sleep_for(std::chrono::seconds(5));
    settle();
    EXPECT_EQ(heartbeats.count(), count_after_expiry);

    // Feeding the watchdog clears the latch and restarts heartbeats.
    mavsdk.feed_heartbeat_watchdog();
    EXPECT_TRUE(wait_for([&]() { return heartbeats.count() > count_after_expiry; }));
}
