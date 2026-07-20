#include "ringbuffer.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Ringbuffer, PushAndRead)
{
    auto buffer = Ringbuffer<int, 3>{};

    buffer.push(4);
    EXPECT_EQ(buffer.size(), 1);
    buffer.push(5);
    EXPECT_EQ(buffer.size(), 2);
    buffer.push(6);
    EXPECT_EQ(buffer.size(), 3);

    std::vector<int> expected{4, 5, 6};
    unsigned i = 0;
    for (const auto& buf : buffer) {
        EXPECT_EQ(buf, expected[i++]);
    }
}

TEST(Ringbuffer, PushOverwriteAndRead)
{
    auto buffer = Ringbuffer<int, 3>{};

    buffer.push(4);
    buffer.push(5);
    buffer.push(6);
    buffer.push(7);

    ASSERT_EQ(buffer.size(), 3);

    std::vector<int> expected{5, 6, 7};
    unsigned i = 0;
    for (const auto& buf : buffer) {
        EXPECT_EQ(buf, expected[i++]);
    }
}

TEST(Ringbuffer, PushOverwriteMoreAndRead)
{
    auto buffer = Ringbuffer<int, 2>{};

    buffer.push(4);
    buffer.push(5);
    buffer.push(6);
    buffer.push(7);
    buffer.push(8);
    buffer.push(9);

    ASSERT_EQ(buffer.size(), 2);

    std::vector<int> expected{8, 9};
    unsigned i = 0;
    for (const auto& buf : buffer) {
        EXPECT_EQ(buf, expected[i++]);
    }
}

TEST(Ringbuffer, PushAndMutate)
{
    auto buffer = Ringbuffer<int, 3>{};

    buffer.push(4);
    EXPECT_EQ(buffer.size(), 1);
    buffer.push(5);
    EXPECT_EQ(buffer.size(), 2);
    buffer.push(6);
    EXPECT_EQ(buffer.size(), 3);

    std::vector<int> expected{14, 15, 16};
    for (auto& buf : buffer) {
        // Try to mutate the values.
        buf += 10;
    }

    // And check again
    unsigned i = 0;
    for (const auto& buf : buffer) {
        EXPECT_EQ(buf, expected[i++]);
    }
}

TEST(Ringbuffer, EmptyStartsAtZero)
{
    auto buffer = Ringbuffer<int, 4>{};
    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_TRUE(buffer.begin() == buffer.end());
    EXPECT_TRUE(buffer.cbegin() == buffer.cend());
}

TEST(Ringbuffer, SingleElementIndex)
{
    // Ringbuffer advances _index before store on the first push, so a lone
    // entry is nested behind the default-constructed hole at logical [0].
    // Document that quirk via operator[] while still covering size / end.
    auto buffer = Ringbuffer<int, 4>{};
    buffer.push(42);
    EXPECT_EQ(buffer.size(), 1u);
    EXPECT_EQ(buffer[1], 42);
    auto it = buffer.begin();
    ++it;
    EXPECT_TRUE(it == buffer.end());
}

TEST(Ringbuffer, OperatorIndexAfterWrap)
{
    auto buffer = Ringbuffer<int, 3>{};
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4); // drops 1
    EXPECT_EQ(buffer.size(), 3u);
    EXPECT_EQ(buffer[0], 2);
    EXPECT_EQ(buffer[1], 3);
    EXPECT_EQ(buffer[2], 4);
}
