
#include <string>
#include "any.h"
#include <gtest/gtest.h>

using namespace dronecore;

TEST(Any, StringAndInt)
{
    Any n;
    ASSERT_TRUE(n.is_null());

    std::string s1 = "foo";

    Any a1 = s1;

    ASSERT_TRUE(a1.not_null());
    ASSERT_TRUE(a1.is<std::string>());
    ASSERT_TRUE(!a1.is<int>());

    Any a2(a1);

    ASSERT_TRUE(a2.not_null());
    ASSERT_TRUE(a2.is<std::string>());
    ASSERT_TRUE(!a2.is<int>());

    std::string s2 = a2;

    ASSERT_TRUE(s1 == s2);
}

TEST(Any, Casts)
{
    const int some_number = 42;
    Any n;
    ASSERT_TRUE(n.is_null());

    int i = some_number;
    n = i;

    ASSERT_TRUE(n.not_null());
    ASSERT_EQ(n.as<int>(), some_number);

    // We can't actually cast using `as`.
    ASSERT_FLOAT_EQ(float(n.as<int>()), float(some_number));
}
