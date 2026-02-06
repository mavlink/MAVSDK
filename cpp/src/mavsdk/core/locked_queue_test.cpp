
#include "locked_queue.h"

#include <string>
#include <future>
#include <set>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(LockedQueue, FillAndEmpty)
{
    int one = 1;
    int two = 2;
    int three = 3;

    LockedQueue<int> locked_queue{};

    locked_queue.push_back(std::make_shared<int>(one));
    EXPECT_EQ(locked_queue.size(), 1);
    locked_queue.push_back(std::make_shared<int>(two));
    locked_queue.push_back(std::make_shared<int>(three));
    EXPECT_EQ(locked_queue.size(), 3);

    {
        LockedQueue<int>::Guard guard(locked_queue);
        guard.pop_front();
    }

    EXPECT_EQ(locked_queue.size(), 2);

    {
        LockedQueue<int>::Guard guard(locked_queue);
        guard.pop_front();
        guard.pop_front();
    }

    EXPECT_EQ(locked_queue.size(), 0);
}

TEST(LockedQueue, FillAndIterateAndErase)
{
    int one = 1;
    int two = 2;
    int three = 3;

    LockedQueue<int> locked_queue{};

    locked_queue.push_back(std::make_shared<int>(one));
    EXPECT_EQ(locked_queue.size(), 1);
    locked_queue.push_back(std::make_shared<int>(two));
    locked_queue.push_back(std::make_shared<int>(three));
    EXPECT_EQ(locked_queue.size(), 3);

    unsigned counter = 0;
    for (const auto& item : locked_queue) {
        ++counter;
        EXPECT_EQ(*item, counter);
    }
    EXPECT_EQ(counter, 3);

    for (auto item = locked_queue.begin(); item != locked_queue.end();
         /* manual incrementation */) {
        if (**item == 2) {
            item = locked_queue.erase(item);
        } else {
            ++item;
        }
    }

    EXPECT_EQ(locked_queue.size(), 2);

    counter = 0;
    for (const auto& item : locked_queue) {
        ++counter;
        if (counter == 1) {
            EXPECT_EQ(*item, 1);
        } else if (counter == 2) {
            EXPECT_EQ(*item, 3);
        }
    }
}

TEST(LockedQueue, GuardAndReturn)
{
    int one = 1;
    int two = 2;
    int three = 3;

    LockedQueue<int> locked_queue{};

    locked_queue.push_back(std::make_shared<int>(one));
    locked_queue.push_back(std::make_shared<int>(two));
    locked_queue.push_back(std::make_shared<int>(three));

    {
        LockedQueue<int>::Guard guard(locked_queue);
        EXPECT_EQ(*guard.get_front(), 1);
    }

    {
        LockedQueue<int>::Guard guard(locked_queue);
        EXPECT_EQ(*guard.get_front(), 1);
        guard.pop_front();
    }

    {
        LockedQueue<int>::Guard guard(locked_queue);
        EXPECT_EQ(*guard.get_front(), 2);
    }

    EXPECT_EQ(locked_queue.size(), 2);
    {
        LockedQueue<int>::Guard guard(locked_queue);
        guard.pop_front();
        guard.pop_front();
        EXPECT_EQ(guard.get_front(), nullptr);
    }
}

TEST(LockedQueue, ConcurrantAccess)
{
    int one = 1;
    int two = 2;

    LockedQueue<int> locked_queue{};

    locked_queue.push_back(std::make_shared<int>(one));
    locked_queue.push_back(std::make_shared<int>(two));

    auto prom = std::promise<void>();
    auto fut = prom.get_future();

    {
        auto* guard_ptr = new LockedQueue<int>::Guard(locked_queue);

        EXPECT_EQ(*guard_ptr->get_front(), 1);

        auto some_future = std::async(std::launch::async, [&prom, &locked_queue]() {
            // This will wait in the lock until the first item is returned.
            {
                LockedQueue<int>::Guard guard_in_callback(locked_queue);
            }
            prom.set_value();
        });

        // The promise should not be fulfilled yet because we have not
        // returned the guarded item.
        auto status = fut.wait_for(std::chrono::milliseconds(20));
        EXPECT_EQ(status, std::future_status::timeout);

        // We need to delete the guard first, before the std::async thread
        // goes out of scope, otherwise we deadlock.
        delete guard_ptr;
    }
    auto status = fut.wait_for(std::chrono::milliseconds(20));
    EXPECT_EQ(status, std::future_status::ready);
}

TEST(LockedQueue, ChangeValue)
{
    struct Item {
        int value{42};
    };

    LockedQueue<Item> locked_queue{};

    Item one;

    locked_queue.push_back(std::make_shared<Item>(one));

    {
        LockedQueue<Item>::Guard guard(locked_queue);
        EXPECT_EQ(guard.get_front()->value, 42);
        auto front = guard.get_front();
        if (front) {
            front->value = 43;
        }
    }

    {
        LockedQueue<Item>::Guard guard(locked_queue);
        EXPECT_EQ(guard.get_front()->value, 43);
    }
}

TEST(LockedQueue, Guard)
{
    int one = 1;
    int two = 2;

    LockedQueue<int> locked_queue{};

    locked_queue.push_back(std::make_shared<int>(one));
    locked_queue.push_back(std::make_shared<int>(two));

    {
        LockedQueue<int>::Guard guard(locked_queue);
        EXPECT_EQ(*guard.get_front(), 1);
    }

    {
        LockedQueue<int>::Guard guard(locked_queue);
        EXPECT_EQ(*guard.get_front(), 1);
        guard.pop_front();
    }

    {
        LockedQueue<int>::Guard guard(locked_queue);
        EXPECT_EQ(*guard.get_front(), 2);

        guard.pop_front();

        EXPECT_EQ(guard.get_front(), nullptr);
        // more pops shouldn't do anything
    }
}

TEST(LockedQueue, WaitAndPopFrontBlocking)
{
    int one = 1;

    LockedQueue<int> locked_queue{};

    auto prom = std::promise<std::shared_ptr<int>>();
    auto fut = prom.get_future();

    // Start a thread that will block waiting for items
    auto wait_thread = std::async(std::launch::async, [&prom, &locked_queue]() {
        LockedQueue<int>::Guard guard(locked_queue);
        auto result = guard.wait_and_pop_front();
        prom.set_value(result);
    });

    // Give the thread time to start waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // The promise should not be fulfilled yet because the queue is empty
    auto status = fut.wait_for(std::chrono::milliseconds(20));
    EXPECT_EQ(status, std::future_status::timeout);

    // Now add an item to the queue
    locked_queue.push_back(std::make_shared<int>(one));

    // The wait should complete quickly now
    status = fut.wait_for(std::chrono::milliseconds(100));
    EXPECT_EQ(status, std::future_status::ready);

    auto result = fut.get();
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(*result, 1);

    // The queue should now be empty
    EXPECT_EQ(locked_queue.size(), 0);
}

TEST(LockedQueue, WaitAndPopFrontWithStop)
{
    LockedQueue<int> locked_queue{};

    auto prom = std::promise<std::shared_ptr<int>>();
    auto fut = prom.get_future();

    // Start a thread that will block waiting for items
    auto wait_thread = std::async(std::launch::async, [&prom, &locked_queue]() {
        LockedQueue<int>::Guard guard(locked_queue);
        auto result = guard.wait_and_pop_front();
        prom.set_value(result);
    });

    // Give the thread time to start waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // The promise should not be fulfilled yet
    auto status = fut.wait_for(std::chrono::milliseconds(20));
    EXPECT_EQ(status, std::future_status::timeout);

    // Stop the queue - this should wake up the waiting thread
    locked_queue.stop();

    // The wait should complete quickly now with nullptr
    status = fut.wait_for(std::chrono::milliseconds(100));
    EXPECT_EQ(status, std::future_status::ready);

    auto result = fut.get();
    EXPECT_EQ(result, nullptr);
}

TEST(LockedQueue, WaitAndPopFrontMultipleWaiters)
{
    int one = 1;
    int two = 2;

    LockedQueue<int> locked_queue{};

    auto prom1 = std::promise<std::shared_ptr<int>>();
    auto fut1 = prom1.get_future();
    auto prom2 = std::promise<std::shared_ptr<int>>();
    auto fut2 = prom2.get_future();

    // Start two threads that will block waiting for items
    auto wait_thread1 = std::async(std::launch::async, [&prom1, &locked_queue]() {
        LockedQueue<int>::Guard guard(locked_queue);
        auto result = guard.wait_and_pop_front();
        prom1.set_value(result);
    });

    auto wait_thread2 = std::async(std::launch::async, [&prom2, &locked_queue]() {
        LockedQueue<int>::Guard guard(locked_queue);
        auto result = guard.wait_and_pop_front();
        prom2.set_value(result);
    });

    // Give the threads time to start waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Both promises should not be fulfilled yet
    EXPECT_EQ(fut1.wait_for(std::chrono::milliseconds(20)), std::future_status::timeout);
    EXPECT_EQ(fut2.wait_for(std::chrono::milliseconds(20)), std::future_status::timeout);

    // Add two items to the queue
    locked_queue.push_back(std::make_shared<int>(one));
    locked_queue.push_back(std::make_shared<int>(two));

    // Both waits should complete
    EXPECT_EQ(fut1.wait_for(std::chrono::milliseconds(100)), std::future_status::ready);
    EXPECT_EQ(fut2.wait_for(std::chrono::milliseconds(100)), std::future_status::ready);

    auto result1 = fut1.get();
    auto result2 = fut2.get();

    // Both should get valid results
    ASSERT_NE(result1, nullptr);
    ASSERT_NE(result2, nullptr);

    // One should get 1, the other should get 2 (order doesn't matter for this test)
    std::set<int> received_values{*result1, *result2};
    std::set<int> expected_values{1, 2};
    EXPECT_EQ(received_values, expected_values);

    // The queue should now be empty
    EXPECT_EQ(locked_queue.size(), 0);
}
