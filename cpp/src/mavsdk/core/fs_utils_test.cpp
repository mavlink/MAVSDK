#include "fs_utils.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <string>

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


TEST(FsUtils, GetCacheDirectoryHasMavsdkSegment)
{
    auto path = get_cache_directory();
    ASSERT_TRUE(path.has_value());
    const auto s = path->string();
    // Platform path contains a mavsdk-named leaf (Linux ~/.cache/mavsdk, macOS Library/Caches/mavsdk).
    EXPECT_NE(s.find("mavsdk"), std::string::npos);
    EXPECT_FALSE(s.empty());
}

TEST(FsUtils, CreateTmpDirectoryUniqueAndExists)
{
    auto a = create_tmp_directory("mavsdk-ut");
    auto b = create_tmp_directory("mavsdk-ut");
    ASSERT_TRUE(a.has_value());
    ASSERT_TRUE(b.has_value());
    EXPECT_NE(*a, *b);
    EXPECT_TRUE(std::filesystem::is_directory(*a));
    EXPECT_TRUE(std::filesystem::is_directory(*b));
    std::error_code ec;
    std::filesystem::remove_all(*a, ec);
    std::filesystem::remove_all(*b, ec);
}

TEST(FsUtils, ReplaceAsciiControlCharactersAsWhitespace)
{
    // \\r and form-feed are whitespace via isspace → underscore.
    EXPECT_EQ(replace_non_ascii_and_whitespace(std::string("a") + "\r\f" + "b"), "a__b");
}
