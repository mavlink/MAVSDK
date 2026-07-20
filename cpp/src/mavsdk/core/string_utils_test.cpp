#include "string_utils.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(StringUtils, StartsWith)
{
    EXPECT_TRUE(starts_with("blafoo", "bla"));
    EXPECT_FALSE(starts_with("blafoo", "1234"));
    EXPECT_FALSE(starts_with("blafoo", "foo"));
    EXPECT_TRUE(starts_with("", ""));
    EXPECT_FALSE(starts_with("", "b"));
    EXPECT_TRUE(starts_with("b", ""));
}

TEST(StringUtils, StripPrefix)
{
    EXPECT_EQ(strip_prefix("blafoo", "bla"), "foo");
    EXPECT_EQ(strip_prefix("blafoo", "1234"), "blafoo");
    EXPECT_EQ(strip_prefix("blafoo", "foo"), "blafoo");
    EXPECT_EQ(strip_prefix("", ""), "");
    EXPECT_EQ(strip_prefix("", "b"), "");
    EXPECT_EQ(strip_prefix("f", ""), "f");
}

TEST(StringUtils, StartsWithFullString)
{
    EXPECT_TRUE(starts_with("mavsdk", "mavsdk"));
    EXPECT_FALSE(starts_with("mav", "mavsdk"));
}

TEST(StringUtils, StripPrefixFullString)
{
    EXPECT_EQ(strip_prefix("mavsdk", "mavsdk"), "");
    EXPECT_EQ(strip_prefix("prefix", "prefixx"), "prefix");
}

TEST(StringUtils, StartsWithLongerPrefixRejected)
{
    EXPECT_FALSE(starts_with("ab", "abc"));
    EXPECT_EQ(strip_prefix("ab", "abc"), "ab");
}
