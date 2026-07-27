#include "heartbeat_watchdog.hpp"
#include <gtest/gtest.h>

#include <limits>

#ifdef FAKE_TIME
#define Time FakeTime
#endif

using namespace mavsdk;

namespace {

constexpr double timeout_s = 2.0;

// Advances the clock and reports what the next due heartbeat would do.
bool allows_sending_after(Time& time, HeartbeatWatchdog& watchdog, double elapsed_s)
{
    time.sleep_for(std::chrono::milliseconds(int64_t(elapsed_s * 1e3)));
    return watchdog.allows_sending();
}

} // namespace

TEST(HeartbeatWatchdog, DisabledByDefault)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);

    EXPECT_DOUBLE_EQ(watchdog.timeout_s(), 0.0);

    // Disabled means the watchdog never gets in the way, however much time
    // passes and whether or not it is fed.
    EXPECT_TRUE(watchdog.allows_sending());
    EXPECT_TRUE(allows_sending_after(time, watchdog, 100.0));

    watchdog.feed();
    EXPECT_TRUE(allows_sending_after(time, watchdog, 100.0));
}

TEST(HeartbeatWatchdog, ValidatesTimeout)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);

    EXPECT_TRUE(watchdog.set_timeout_s(2.5));
    EXPECT_DOUBLE_EQ(watchdog.timeout_s(), 2.5);

    // Sub-minimum, negative and non-finite values are rejected and leave the
    // stored timeout unchanged.
    EXPECT_FALSE(watchdog.set_timeout_s(HeartbeatWatchdog::min_timeout_s / 2.0));
    EXPECT_FALSE(watchdog.set_timeout_s(-1.0));
    EXPECT_FALSE(watchdog.set_timeout_s(std::numeric_limits<double>::infinity()));
    EXPECT_FALSE(watchdog.set_timeout_s(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_DOUBLE_EQ(watchdog.timeout_s(), 2.5);

    // The minimum and 0 (disabled) are both valid.
    EXPECT_TRUE(watchdog.set_timeout_s(HeartbeatWatchdog::min_timeout_s));
    EXPECT_TRUE(watchdog.set_timeout_s(0.0));
    EXPECT_DOUBLE_EQ(watchdog.timeout_s(), 0.0);

    // Same rule as the static helper, which is what the public API and
    // mavsdk_server validate against.
    EXPECT_TRUE(HeartbeatWatchdog::is_valid_timeout_s(0.0));
    EXPECT_TRUE(HeartbeatWatchdog::is_valid_timeout_s(HeartbeatWatchdog::min_timeout_s));
    EXPECT_FALSE(HeartbeatWatchdog::is_valid_timeout_s(HeartbeatWatchdog::min_timeout_s / 2.0));
    EXPECT_FALSE(HeartbeatWatchdog::is_valid_timeout_s(-1.0));
    EXPECT_FALSE(HeartbeatWatchdog::is_valid_timeout_s(std::numeric_limits<double>::infinity()));
    EXPECT_FALSE(HeartbeatWatchdog::is_valid_timeout_s(std::numeric_limits<double>::quiet_NaN()));
}

TEST(HeartbeatWatchdog, EnablingRequiresFeedBeforeSending)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);

    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    // Enabling must not grant a free timeout period: nothing is allowed until
    // the first feed, however long we wait.
    EXPECT_FALSE(watchdog.allows_sending());
    EXPECT_FALSE(allows_sending_after(time, watchdog, timeout_s * 10.0));

    watchdog.feed();
    EXPECT_TRUE(watchdog.allows_sending());
}

TEST(HeartbeatWatchdog, FeedIsValidUntilItsDeadline)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();

    // Just before the deadline sending is still allowed, just after it is not.
    EXPECT_TRUE(allows_sending_after(time, watchdog, timeout_s * 0.9));
    EXPECT_FALSE(allows_sending_after(time, watchdog, timeout_s * 0.2));
}

TEST(HeartbeatWatchdog, FeedingExtendsTheDeadline)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    // Feeding twice per timeout period keeps sending allowed indefinitely.
    for (int i = 0; i < 20; ++i) {
        watchdog.feed();
        EXPECT_TRUE(allows_sending_after(time, watchdog, timeout_s / 2.0)) << "iteration " << i;
    }

    // Stop feeding and it lapses.
    EXPECT_FALSE(allows_sending_after(time, watchdog, timeout_s));
}

TEST(HeartbeatWatchdog, ExpiryNeedsAFreshFeed)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();
    ASSERT_TRUE(watchdog.allows_sending());

    // Let it expire.
    ASSERT_FALSE(allows_sending_after(time, watchdog, timeout_s * 2.0));

    // It stays expired until fed, no matter how often it is asked.
    EXPECT_FALSE(watchdog.allows_sending());
    EXPECT_FALSE(watchdog.allows_sending());

    // A fresh feed restarts it, and grants a full timeout period from now
    // rather than from the stale deadline.
    watchdog.feed();
    EXPECT_TRUE(allows_sending_after(time, watchdog, timeout_s * 0.9));
}

TEST(HeartbeatWatchdog, FeedIsValidEvenIfNotAskedInBetween)
{
    // The watchdog must not depend on being polled: a feed stays valid until
    // its deadline even if no heartbeat was due in between (which is what
    // happens while the heartbeat policy is off, or no system is connected).
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();

    // Nobody asks for a while, then asks within the deadline.
    EXPECT_TRUE(allows_sending_after(time, watchdog, timeout_s * 0.9));
}

TEST(HeartbeatWatchdog, UnusedFeedStillExpires)
{
    // The mirror image: a feed that was never acted upon must not stay valid
    // forever, otherwise a stale feed would count as liveness once heartbeats
    // are allowed again.
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();
    EXPECT_FALSE(allows_sending_after(time, watchdog, timeout_s * 2.0));
}

TEST(HeartbeatWatchdog, DisablingAllowsSendingWithoutAFeed)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));
    ASSERT_FALSE(watchdog.allows_sending());

    // Disabling hands control back to the usual heartbeat policy.
    ASSERT_TRUE(watchdog.set_timeout_s(0.0));
    EXPECT_TRUE(watchdog.allows_sending());
    EXPECT_TRUE(allows_sending_after(time, watchdog, timeout_s * 10.0));
}

TEST(HeartbeatWatchdog, ChangingTimeoutRequiresAFreshFeed)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();
    ASSERT_TRUE(watchdog.allows_sending());

    // Changing the timeout stops sending until fed again, even though the
    // previous feed would still have been within the new deadline.
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s * 2.0));
    EXPECT_FALSE(watchdog.allows_sending());

    watchdog.feed();
    EXPECT_TRUE(watchdog.allows_sending());
}

TEST(HeartbeatWatchdog, SettingTheSameTimeoutDoesNotReArm)
{
    // MavsdkImpl pushes the whole configuration down on every update, so
    // setting an unchanged timeout must not disturb an armed watchdog.
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();
    ASSERT_TRUE(watchdog.allows_sending());

    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));
    EXPECT_TRUE(watchdog.allows_sending());

    // A rejected timeout must not disturb it either.
    ASSERT_FALSE(watchdog.set_timeout_s(0.5));
    EXPECT_TRUE(watchdog.allows_sending());
}

TEST(HeartbeatWatchdog, RejectedTimeoutKeepsPreviousDeadline)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    watchdog.feed();

    // A rejected, shorter timeout must not shorten the active deadline.
    ASSERT_FALSE(watchdog.set_timeout_s(0.5));
    EXPECT_DOUBLE_EQ(watchdog.timeout_s(), timeout_s);
    EXPECT_TRUE(allows_sending_after(time, watchdog, timeout_s * 0.9));
}

TEST(HeartbeatWatchdog, FeedWhileDisabledIsNotRemembered)
{
    Time time{};
    HeartbeatWatchdog watchdog(time);

    // Feeding before the watchdog exists must not count once it is enabled.
    watchdog.feed();
    ASSERT_TRUE(watchdog.set_timeout_s(timeout_s));

    EXPECT_FALSE(watchdog.allows_sending());
}
