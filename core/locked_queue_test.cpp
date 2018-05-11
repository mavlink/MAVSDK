
#include "locked_queue.h"

#include <string>
#include <future>
#include <gtest/gtest.h>

using namespace dronecore;

TEST(LockedQueue, FillAndEmpty)
{
    int one = 1;
    int two = 2;
    int three = 3;

    LockedQueue<int> locked_queue {};

    locked_queue.push_back(one);
    EXPECT_EQ(locked_queue.size(), 1);
    locked_queue.push_back(two);
    locked_queue.push_back(three);
    EXPECT_EQ(locked_queue.size(), 3);

    locked_queue.pop_front();
    EXPECT_EQ(locked_queue.size(), 2);
    locked_queue.pop_front();
    locked_queue.pop_front();
    EXPECT_EQ(locked_queue.size(), 0);

    // Popping an empty queue should just fail silently.
    locked_queue.pop_front();
    EXPECT_EQ(locked_queue.size(), 0);
}

TEST(LockedQueue, BorrowAndReturn)
{
    int one = 1;
    int two = 2;
    int three = 3;

    LockedQueue<int> locked_queue {};

    locked_queue.push_back(one);
    locked_queue.push_back(two);
    locked_queue.push_back(three);

    auto borrowed_item = locked_queue.borrow_front();
    EXPECT_EQ(*borrowed_item, 1);
    locked_queue.return_front();
    locked_queue.pop_front();

    borrowed_item = locked_queue.borrow_front();
    EXPECT_EQ(*borrowed_item, 2);
    locked_queue.return_front();
    // Double returning shouldn't matter.
    locked_queue.return_front();
    locked_queue.pop_front();

    borrowed_item = locked_queue.borrow_front();
    EXPECT_EQ(*borrowed_item, 3);
    // Popping without returning should automatically return it.
    locked_queue.pop_front();
    EXPECT_EQ(locked_queue.size(), 0);

    borrowed_item = locked_queue.borrow_front();
    EXPECT_EQ(borrowed_item, nullptr);
}

TEST(LockedQueue, ConcurrantAccess)
{
    int one = 1;
    int two = 2;

    LockedQueue<int> locked_queue {};

    locked_queue.push_back(one);
    locked_queue.push_back(two);

    auto borrowed_item = locked_queue.borrow_front();
    EXPECT_EQ(*borrowed_item, 1);

    auto prom = std::make_shared<std::promise<void>>();
    auto fut = prom->get_future();

    auto some_future = std::async(std::launch::async | std::launch::deferred,
    [&prom, &locked_queue]() {
        // This will wait in the lock until the first item is returned.
        auto second_borrowed_item = locked_queue.borrow_front();
        locked_queue.return_front();
        prom->set_value();
    });

    // The promise should not be fulfilled yet because we have not
    // returned the borrowed item.
    auto status = fut.wait_for(std::chrono::milliseconds(10));
    EXPECT_EQ(status, std::future_status::timeout);

    locked_queue.return_front();

    status = fut.wait_for(std::chrono::milliseconds(100));
    EXPECT_EQ(status, std::future_status::ready);
}
