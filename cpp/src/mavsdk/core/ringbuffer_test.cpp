#include "ringbuffer.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace mavsdk;

namespace {

// Checks both operator[] and range-for against the expected oldest-to-newest order.
template<typename Buf> void expect_contents(Buf& buffer, const std::vector<int>& expected)
{
    ASSERT_EQ(buffer.size(), expected.size());

    for (unsigned i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(buffer[i], expected[i]) << "at index " << i;
    }

    unsigned count = 0;
    for (const auto& item : buffer) {
        ASSERT_LT(count, expected.size());
        EXPECT_EQ(item, expected[count++]);
    }
    EXPECT_EQ(count, expected.size());
}

} // namespace

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
    auto buffer = Ringbuffer<int, 4>{};
    buffer.push(42);
    EXPECT_EQ(buffer.size(), 1u);
    EXPECT_EQ(buffer[0], 42);
    EXPECT_EQ(*buffer.begin(), 42);
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

TEST(Ringbuffer, PartialFillsAndIterationOrder)
{
    auto buffer = Ringbuffer<int, 4>{};

    buffer.push(10);
    expect_contents(buffer, {10});
    buffer.push(20);
    expect_contents(buffer, {10, 20});
    buffer.push(30);
    expect_contents(buffer, {10, 20, 30});
}

TEST(Ringbuffer, ExactlyFullAndWrapAround)
{
    auto buffer = Ringbuffer<int, 4>{};

    for (int v : {10, 20, 30, 40}) {
        buffer.push(v);
    }
    expect_contents(buffer, {10, 20, 30, 40});

    buffer.push(50); // drops 10
    expect_contents(buffer, {20, 30, 40, 50});

    buffer.push(60); // drops 20
    expect_contents(buffer, {30, 40, 50, 60});
}

TEST(Ringbuffer, IteratorIndexRespectsOffset)
{
    auto buffer = Ringbuffer<int, 5>{};
    for (int v : {10, 20, 30, 40, 50}) {
        buffer.push(v);
    }

    auto it = buffer.begin();
    ++it;
    EXPECT_EQ(it[0], 20);
    EXPECT_EQ(it[1], 30);

    auto cit = buffer.cbegin();
    ++cit;
    EXPECT_EQ(cit[0], 20);
    EXPECT_EQ(cit[1], 30);
}

TEST(Ringbuffer, IteratorRandomAccessOffset)
{
    auto buffer = Ringbuffer<int, 5>{};
    for (int v : {10, 20, 30, 40}) {
        buffer.push(v); // partially filled
    }

    auto it = buffer.begin();
    for (int off = 0; off < 4; ++off) {
        auto shifted = it + off;
        for (int n = 0; n + off < 4; ++n) {
            EXPECT_EQ(shifted[n], *(shifted + n)) << "off " << off << " n " << n;
        }
        EXPECT_EQ(*shifted, buffer[off]);
    }
    EXPECT_TRUE((it + 4) == buffer.end());
    EXPECT_TRUE((buffer.end() - 4) == it);

    const auto& cbuf = buffer;
    auto cit = cbuf.begin();
    EXPECT_EQ((cit + 2)[1], 40);
    EXPECT_EQ(*(cit + 3), 40);
}
