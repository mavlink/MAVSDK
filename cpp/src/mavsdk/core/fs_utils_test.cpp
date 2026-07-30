#include "fs_utils.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(FsUtils, ReplaceAsciiUnchanged)
{
    EXPECT_EQ(replace_non_ascii_and_whitespace("cache-tag_v1"), "cache-tag_v1");
    EXPECT_EQ(replace_non_ascii_and_whitespace(""), "");
}

TEST(FsUtils, ReplaceWhitespace)
{
    EXPECT_EQ(replace_non_ascii_and_whitespace("a b\tc"), "a_b_c");
    EXPECT_EQ(replace_non_ascii_and_whitespace(" \n"), "__");
}

TEST(FsUtils, ReplaceNonAscii)
{
    // UTF-8 multi-byte bytes are >127 and must become underscores.
    const std::string input = std::string("id-") + "\xC3\xA9" + "-x"; // "é"
    EXPECT_EQ(replace_non_ascii_and_whitespace(input), "id-__-x");
}
