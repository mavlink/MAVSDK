
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

TEST(Any, Copys)
{
    const float some_float = 0.7f;

    Any n1;
    Any n2;
    ASSERT_TRUE(n1.is_null());
    ASSERT_TRUE(n2.is_null());

    n1 = some_float;
    n2 = n1;
    ASSERT_TRUE(n1.is<float>());
    ASSERT_TRUE(n2.is<float>());
    ASSERT_TRUE(n1.as<float>() == n2.as<float>());
}
