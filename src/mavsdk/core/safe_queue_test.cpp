#include "safe_queue.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SafeQueue, FillAndEmpty)
{
    int one = 1;
    int two = 2;
    int three = 3;

    SafeQueue<int> safe_queue{};

    safe_queue.enqueue(one);
    EXPECT_EQ(safe_queue.size(), 1);
    safe_queue.enqueue(two);
    safe_queue.enqueue(three);
    ASSERT_EQ(safe_queue.size(), 3);

    EXPECT_EQ(safe_queue.dequeue().value(), 1);
    EXPECT_EQ(safe_queue.size(), 2);
    EXPECT_EQ(safe_queue.dequeue().value(), 2);
    EXPECT_EQ(safe_queue.dequeue().value(), 3);
    EXPECT_EQ(safe_queue.size(), 0);

    safe_queue.stop();
    EXPECT_EQ(safe_queue.dequeue(), std::nullopt);
}

TEST(SafeQueue, StopEarly)
{
    int one = 1;
    int two = 2;

    SafeQueue<int> safe_queue{};

    safe_queue.enqueue(one);
    safe_queue.enqueue(two);

    ASSERT_EQ(safe_queue.size(), 2);
    EXPECT_EQ(safe_queue.dequeue().value(), 1);
    EXPECT_EQ(safe_queue.size(), 1);

    safe_queue.stop();
    EXPECT_EQ(safe_queue.dequeue(), std::nullopt);
}
