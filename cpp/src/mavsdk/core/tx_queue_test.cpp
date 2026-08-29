#include "tx_queue.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace mavsdk;

TEST(TxQueue, StartsEmptyAndIdle)
{
    TxQueue<std::string> queue{4};

    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.busy());
    EXPECT_EQ(queue.size(), 0u);
    EXPECT_EQ(queue.dropped_total(), 0u);
}

TEST(TxQueue, DrainsInOrder)
{
    TxQueue<std::string> queue{4};

    EXPECT_EQ(queue.push("one"), 0u);
    EXPECT_EQ(queue.push("two"), 0u);
    EXPECT_EQ(queue.size(), 2u);

    EXPECT_EQ(queue.start(), "one");
    EXPECT_TRUE(queue.busy());
    // The in-flight item no longer counts as waiting.
    EXPECT_EQ(queue.size(), 1u);

    queue.finish();
    EXPECT_FALSE(queue.busy());

    EXPECT_EQ(queue.start(), "two");
    queue.finish();

    EXPECT_TRUE(queue.empty());
}

TEST(TxQueue, DropsOldestWhenFull)
{
    TxQueue<int> queue{3};

    EXPECT_EQ(queue.push(1), 0u);
    EXPECT_EQ(queue.push(2), 0u);
    EXPECT_EQ(queue.push(3), 0u);

    // Full: the next push evicts the oldest, not the newest.
    EXPECT_EQ(queue.push(4), 1u);
    EXPECT_EQ(queue.size(), 3u);
    EXPECT_EQ(queue.dropped_total(), 1u);

    EXPECT_EQ(queue.start(), 2);
    queue.finish();
    EXPECT_EQ(queue.start(), 3);
    queue.finish();
    EXPECT_EQ(queue.start(), 4);
    queue.finish();
    EXPECT_TRUE(queue.empty());
}

// The in-flight item is what an async write is reading from, so it must survive both a
// queue that keeps growing past its bound and an explicit clear().
TEST(TxQueue, InFlightItemSurvivesEvictionAndClear)
{
    TxQueue<std::vector<char>> queue{2};

    queue.push(std::vector<char>{'a', 'b'});
    const auto& in_flight = queue.start();
    const char* data = in_flight.data();

    // Fill past the bound so the queue evicts, and then wipe it entirely.
    queue.push(std::vector<char>{'c'});
    queue.push(std::vector<char>{'d'});
    queue.push(std::vector<char>{'e'});
    EXPECT_EQ(queue.dropped_total(), 1u);
    queue.clear();

    EXPECT_TRUE(queue.empty());
    EXPECT_TRUE(queue.busy());
    EXPECT_EQ(in_flight.data(), data);
    EXPECT_EQ(in_flight, (std::vector<char>{'a', 'b'}));

    queue.finish();
    EXPECT_FALSE(queue.busy());
}

TEST(TxQueue, ClearLeavesQueueReusable)
{
    TxQueue<int> queue{4};

    queue.push(1);
    queue.push(2);
    queue.clear();
    EXPECT_TRUE(queue.empty());

    queue.push(3);
    EXPECT_EQ(queue.start(), 3);
    queue.finish();
    EXPECT_TRUE(queue.empty());
}
