#include "mavlink_channels.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace mavsdk;

TEST(MavlinkChannels, MaxChannelsSanity)
{
    ASSERT_TRUE(MavlinkChannels::get_max_channels() < std::numeric_limits<uint8_t>::max());
    ASSERT_TRUE(MavlinkChannels::get_max_channels() > 0);
}

TEST(MavlinkChannels, TryAll)
{
    // Checkout all first
    for (unsigned i = 0; i < std::numeric_limits<uint8_t>::max(); ++i) {
        uint8_t channel;
        if (i < MavlinkChannels::get_max_channels()) {
            ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(channel));
            ASSERT_EQ(i, channel);
        } else {
            ASSERT_FALSE(MavlinkChannels::Instance().checkout_free_channel(channel));
        }
    }

    // Give them all back, even the invalid ones
    for (unsigned i = 0; i < std::numeric_limits<uint8_t>::max(); ++i) {
        MavlinkChannels::Instance().checkin_used_channel(i);
    }
}

TEST(MavlinkChannels, ReuseChannels)
{
    // Checkout 0,1,2
    uint8_t channels[3];
    ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(channels[0]));
    ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(channels[1]));
    ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(channels[2]));

    // Give back 1.
    MavlinkChannels::Instance().checkin_used_channel(channels[1]);

    // And ask for 1 once again.
    uint8_t new_channel;
    ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(new_channel));
    ASSERT_EQ(new_channel, 1);

    // And make sure it continues at 3.
    ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(new_channel));
    ASSERT_EQ(new_channel, 3);
}

TEST(MavlinkChannels, FullCycleThenReuseFromZero)
{
    const unsigned max_ch = MavlinkChannels::get_max_channels();

    // Prior channel tests leave the singleton pool dirty — check everything in first.
    for (unsigned i = 0; i < max_ch; ++i) {
        MavlinkChannels::Instance().checkin_used_channel(static_cast<uint8_t>(i));
    }

    // Exhaust the pool; accept any free channel (not necessarily i, if ordering changes).
    std::vector<bool> seen(max_ch, false);
    for (unsigned i = 0; i < max_ch; ++i) {
        uint8_t channel = 255;
        ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(channel));
        ASSERT_LT(channel, max_ch);
        EXPECT_FALSE(seen[channel]);
        seen[channel] = true;
    }
    for (unsigned i = 0; i < max_ch; ++i) {
        EXPECT_TRUE(seen[i]);
    }
    uint8_t blocked = 0;
    EXPECT_FALSE(MavlinkChannels::Instance().checkout_free_channel(blocked));

    // Release all — first checkout must succeed again (lowest free is 0).
    for (unsigned i = 0; i < max_ch; ++i) {
        MavlinkChannels::Instance().checkin_used_channel(static_cast<uint8_t>(i));
    }
    uint8_t again = 255;
    ASSERT_TRUE(MavlinkChannels::Instance().checkout_free_channel(again));
    EXPECT_EQ(again, 0);
    MavlinkChannels::Instance().checkin_used_channel(again);
}
