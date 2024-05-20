#include <atomic>
#include <mutex>
#include <thread>
#include <set>
#include <optional>
#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include "callback_list.h"
#include "callback_list.tpp"
#include "log.h"
#include "unused.h"

namespace mavsdk {

template class CallbackList<int, double>;
template class CallbackList<>;

} // namespace mavsdk

using namespace mavsdk;
TEST(CallbackList, SubscribeCallUnsubscribe)
{
    unsigned first_called = 0;
    unsigned second_called = 0;
    unsigned conditional_called = 0;

    CallbackList<int, double> cl;
    auto first_handle = cl.subscribe([&](int i, double d) {
        ++first_called;
        EXPECT_GE(i, 42);
        EXPECT_LE(i, 44);
        EXPECT_GT(d, 77.0);
        EXPECT_LT(d, 112.0);
    });

    auto second_handle = cl.subscribe([&](int i, double d) {
        ++second_called;
        EXPECT_GE(i, 42);
        EXPECT_LE(i, 44);
        EXPECT_GT(d, 77.0);
        EXPECT_LT(d, 112.0);
    });

    cl.subscribe_conditional([&](int i, double d) -> bool {
        ++conditional_called;
        EXPECT_GE(i, 42);
        EXPECT_LE(i, 44);
        EXPECT_GT(d, 77.0);
        EXPECT_LT(d, 112.0);
        // Return true if the callback should be removed after being called.
        return i == 43 && d == 99.0;
    });

    // Call both a first time.
    cl(42, 77.7);
    EXPECT_EQ(first_called, 1);
    EXPECT_EQ(second_called, 1);
    EXPECT_EQ(conditional_called, 1);

    // Call both a second time.
    cl(43, 88.8);
    EXPECT_EQ(first_called, 2);
    EXPECT_EQ(second_called, 2);
    EXPECT_EQ(conditional_called, 2);

    // Now we unsubscribe the first one.
    cl.unsubscribe(first_handle);

    cl(43, 99.9);
    EXPECT_EQ(first_called, 2);
    EXPECT_EQ(second_called, 3);
    EXPECT_EQ(conditional_called, 3);

    // Unsubscribing the first once should be ignored.
    cl.unsubscribe(first_handle);

    // This should call and remove the conditional callback
    cl(43, 99.0);
    EXPECT_EQ(first_called, 2);
    EXPECT_EQ(second_called, 4);
    EXPECT_EQ(conditional_called, 4);

    // This should not call the conditional callback anymore
    cl(43, 99.0);
    EXPECT_EQ(first_called, 2);
    EXPECT_EQ(second_called, 5);
    EXPECT_EQ(conditional_called, 4);

    // Now we unsubscribe the second one as well, no more calling.
    cl.unsubscribe(second_handle);

    cl(44, 111.1);
    EXPECT_EQ(first_called, 2);
    EXPECT_EQ(second_called, 5);
    EXPECT_EQ(conditional_called, 4);

    // Both handles are manually removed and conditional callback is autoremoved
    EXPECT_TRUE(cl.empty());
}

TEST(CallbackList, UnsubscribeFromCallback)
{
    unsigned called = 0;

    CallbackList<> cl;
    Handle<> handle = cl.subscribe([&]() {
        cl.unsubscribe(handle);
        ++called;
    });

    // We call it twice but only expect it to be called once.
    cl();
    cl();
    EXPECT_EQ(called, 1);
}

TEST(CallbackList, UnsubscribeAllWithNullptr)
{
    // This is to deal with the previous API where nullptr would
    // unsubscribe the callback.
    unsigned num_called = 0;

    CallbackList<> cl;
    cl.subscribe([&]() { ++num_called; });

    // Call once.
    cl();

    // Unsubscribe using legacy way.
    cl.subscribe(nullptr);

    // Call again.
    cl();

    // It should only be called once.
    EXPECT_EQ(num_called, 1);
}

TEST(CallbackList, UnsubscribeAllWithClear)
{
    // This is to deal with the previous API where nullptr would
    // unsubscribe the callback.
    unsigned num_called = 0;
    unsigned num_called_other = 0;

    CallbackList<> cl;
    cl.subscribe([&]() { ++num_called; });
    cl.subscribe([&]() { ++num_called_other; });

    // Call once.
    cl();

    // Unsubscribe using clear.
    cl.clear();

    // Call again.
    cl();

    // It should only be called once.
    EXPECT_EQ(num_called, 1);
    EXPECT_EQ(num_called_other, 1);
}

TEST(CallbackList, SubscribeAndUnsubscribeWithinCallbacks)
{
    const int test_value1 = 42;
    const double test_value2 = 3.14;
    const double unsub_value = 4.669;
    std::atomic<int> callback_count{0};
    std::atomic<int> nested_callback_count{0};

    CallbackList<int, double> cl;

    // Lambda function for subscribing within a callback
    auto subscribeCallback = [test_value1, test_value2, unsub_value, &nested_callback_count, &cl](
                                 std::shared_ptr<std::optional<Handle<int, double>>> handle_ptr) {
        *handle_ptr = cl.subscribe(
            [&nested_callback_count, &cl, test_value1, test_value2, unsub_value, handle_ptr](
                int i, double d) {
                UNUSED(i);
                nested_callback_count++;
                // Unsubscribe after execution if unsub_value provided
                if (d == unsub_value) {
                    if (handle_ptr->has_value()) {
                        cl.unsubscribe(
                            handle_ptr->value()); // this will get deferred unsubscription
                    }
                }
            });
    };

    // Main thread subscription
    auto first_handle = cl.subscribe(
        [&subscribeCallback, &callback_count, &cl, test_value1, test_value2](int i, double d) {
            UNUSED(i);
            UNUSED(d);
            callback_count++;

            // Define a placeholder for the subscription handle
            auto handle_ptr = std::make_shared<std::optional<Handle<int, double>>>();
            subscribeCallback(handle_ptr); // this will get deferred sub
        });

    EXPECT_EQ(callback_count, 0);
    EXPECT_EQ(nested_callback_count, 0);

    cl(test_value1, test_value2); // Calls 1st callback that adds 2nd callback
    EXPECT_EQ(callback_count, 1);
    EXPECT_EQ(nested_callback_count, 0);

    cl(test_value1, test_value2); // Calls 1st callback that adds 3rd callback, calls 2nd callback
    EXPECT_EQ(callback_count, 2);
    EXPECT_EQ(nested_callback_count, 1);

    // Calls 1st callback that adds 4th callback, calls 2nd callback which increments and
    // unsubscribes, calls the 3rd callback which increments and unsubscribes
    cl(test_value1, unsub_value);
    EXPECT_EQ(callback_count, 3);
    EXPECT_EQ(nested_callback_count, 3);

    // Remove the 1st callback
    cl.unsubscribe(first_handle);

    // Calls the only remaining 4th callback that increments and unsubscribes
    cl(test_value1, unsub_value);
    EXPECT_EQ(callback_count, 3);
    EXPECT_EQ(nested_callback_count, 4);

    // List is now empty
    EXPECT_TRUE(cl.empty());
}

TEST(CallbackList, SubscribeAndUnsubscribeWithinCallbacks2)
{
    const int test_value1 = 42;
    const double test_value2 = 3.14;
    const int thread_count = 50000;
    std::atomic<int> nested_callback_count{0};
    CallbackList<int, double> cl;

    // Define the vector type
    using HandleType = Handle<int, double>;

    // Create a vector of std::shared_ptr<std::optional<Handle<int, double>>>
    std::set<HandleType> handle_set;
    std::mutex mutex_;

    // Lambda function to subscribe a callback as assign the handle to the arg
    auto addSubCallback = [test_value1, test_value2, &nested_callback_count, &cl](
                              std::shared_ptr<std::optional<Handle<int, double>>> handle_ptr) {
        *handle_ptr = cl.subscribe(
            [&nested_callback_count, &cl, test_value1, test_value2, handle_ptr](int i, double d) {
                EXPECT_EQ(i, test_value1);
                EXPECT_DOUBLE_EQ(d, test_value2);
                nested_callback_count.fetch_add(1, std::memory_order_relaxed);
            });
    };

    // Create multiple threads to simulate concurrent notifications
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&]() {
            // Add a burst of subscriptions in a multithreaded env all at once
            auto handle_ptr = std::make_shared<std::optional<Handle<int, double>>>();
            addSubCallback(handle_ptr); // this will get deferred sub

            // safely put them in a vec for later comparison
            std::lock_guard<std::mutex> guard(mutex_);
            if (handle_ptr && handle_ptr->has_value()) {
                if (!handle_set.insert(handle_ptr->value()).second) {
                    EXPECT_TRUE(false); // handles are not unique
                }
            } else {
                EXPECT_TRUE(false); // handles not filled in
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads) {
        thread.join();
    }
}
