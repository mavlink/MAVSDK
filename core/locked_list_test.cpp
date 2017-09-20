#include "locked_list.h"
#include <gtest/gtest.h>

using namespace dronecore;

static const int LEN = 10;


TEST(LockedList, CreateAndDelete)
{
    LockedList<int> locked_list;
    EXPECT_EQ(locked_list.size(), 0);

    locked_list.push_back(1);
    EXPECT_EQ(locked_list.size(), 1);

    locked_list.push_back(2);
    EXPECT_EQ(locked_list.size(), 2);

    locked_list.pop_front();
    EXPECT_EQ(locked_list.size(), 1);

    locked_list.pop_front();
    EXPECT_EQ(locked_list.size(), 0);
}

TEST(LockedList, Iterate)
{
    LockedList<int> locked_list;
    EXPECT_EQ(locked_list.size(), 0);

    for (int i = 0; i < LEN; ++i) {
        locked_list.push_back(i);
    }
    EXPECT_EQ(locked_list.size(), LEN);

    int count = 0;
    for (auto it = locked_list.begin(); it != locked_list.end(); ++it) {
        ++count;
    }
    EXPECT_EQ(count, LEN);
}

TEST(LockedList, RemoveInBetween)
{
    LockedList<int> locked_list;
    EXPECT_EQ(locked_list.size(), 0);

    for (int i = 0; i < LEN; ++i) {
        locked_list.push_back(i);
        EXPECT_EQ(locked_list.size(), i + 1);
    }

    int count = 0;
    for (auto it = locked_list.begin(); it != locked_list.end(); /*++it*/) {
        if (*it == 5) {
            it = locked_list.erase(it);
        } else {
            ++it;
        }
        ++count;
    }

    EXPECT_EQ(locked_list.size(), LEN - 1);
    EXPECT_EQ(count, LEN);
}

TEST(LockedList, RemoveInBetweenAndAddAgain)
{
    LockedList<int> locked_list;
    EXPECT_EQ(locked_list.size(), 0);

    for (int i = 0; i < LEN; ++i) {
        locked_list.push_back(i);
        EXPECT_EQ(locked_list.size(), i + 1);
    }

    int count = 0;
    for (auto it = locked_list.begin(); it != locked_list.end(); /*++it*/) {
        if (*it == 5) {
            it = locked_list.erase(it);
            locked_list.push_back(42);
        } else {
            ++it;
        }
        ++count;
    }

    EXPECT_EQ(locked_list.size(), LEN);
    EXPECT_EQ(count, LEN + 1);

    int last_one = 0;
    for (auto it = locked_list.begin(); it != locked_list.end(); ++it) {
        EXPECT_NE(*it, 5);
        last_one = *it;
    }
    EXPECT_EQ(last_one, 42);
}
