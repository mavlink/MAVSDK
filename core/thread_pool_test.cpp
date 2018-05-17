#include "thread_pool.h"
#include <gtest/gtest.h>
#include <atomic>

// We don't (yet) use fake time for this
// because the threads actually need to run.
//#ifdef FAKE_TIME
//#define Time FakeTime
//#endif

using namespace dronecore;
using namespace std::placeholders;

static std::atomic<bool> task_one_ran {false};

static Time our_time;

static void run_delayed()
{
    our_time.sleep_for(std::chrono::milliseconds(500));
    task_one_ran = true;
}

TEST(ThreadPool, SimpleTask)
{
    ThreadPool tp(3);
    ASSERT_TRUE(tp.start());

    task_one_ran = false;
    tp.enqueue(std::bind(run_delayed));

    our_time.sleep_for(std::chrono::milliseconds(250));
    EXPECT_FALSE(task_one_ran);
    our_time.sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(task_one_ran);
}

static void add_first_to_second(int arg1, int &arg2)
{

    arg2 += arg1;
}

TEST(ThreadPool, SimpleTaskWithArgs)
{

    ThreadPool tp(3);
    ASSERT_TRUE(tp.start());

    const int first = 5;
    int second = 10;
    const int sum = first + second;

    tp.enqueue(std::bind(add_first_to_second, first, std::ref(second)));

    our_time.sleep_for(std::chrono::milliseconds(250));
    EXPECT_EQ(second, sum);
}

TEST(ThreadPool, LambdaWithArgs)
{

    ThreadPool tp(3);
    ASSERT_TRUE(tp.start());

    const int first = 5;
    int second = 10;
    const int sum = first + second;

    tp.enqueue([first, &second]() {
        second += first;
    });

    our_time.sleep_for(std::chrono::milliseconds(250));
    EXPECT_EQ(second, sum);
}

TEST(ThreadPool, ManyTasks)
{

    ThreadPool tp(3);
    ASSERT_TRUE(tp.start());

    const int tasks_num = 100;
    int tasks[tasks_num] {};

    for (int i = 0; i < tasks_num; ++i) {
        tp.enqueue([&tasks, i]() {
            tasks[i] = i;
        });
    }

    our_time.sleep_for(std::chrono::milliseconds(250));

    for (int i = 0; i < tasks_num; ++i) {
        EXPECT_EQ(tasks[i], i);
    }
}
