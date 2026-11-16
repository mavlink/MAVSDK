#include "binary_to_str.hpp"
#include <cstdint>
#include <string>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(BinaryToStr, TranslateBinaryToStr)
{
    const uint8_t bytes[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    const auto str = translate_binary_to_str(bytes, sizeof(bytes));
    EXPECT_EQ(str.size(), 16u);
    EXPECT_EQ(str, "0123456789abcdef");
}

TEST(BinaryToStr, TranslateUid2Zeros)
{
    const uint8_t uid2[18] = {};
    const auto str = translate_binary_to_str(uid2, sizeof(uid2));
    EXPECT_EQ(str.size(), 36u);
    EXPECT_EQ(str, std::string(36, '0'));
}

TEST(BinaryToStr, SwapAndTranslateFlightSwGitHash)
{
    const uint8_t flight_sw[] = {0xef, 0xcd, 0xab, 0x89, 0x67};
    const auto str = swap_and_translate_binary_to_str(flight_sw, sizeof(flight_sw));
    EXPECT_EQ(str.size(), 10u);
    EXPECT_EQ(str, "6789abcdef");
}

TEST(BinaryToStr, SwapAndTranslateOsSwGitHash)
{
    const uint8_t os_sw[] = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
    const auto str = swap_and_translate_binary_to_str(os_sw, sizeof(os_sw));
    EXPECT_EQ(str.size(), 16u);
    EXPECT_EQ(str, "1032547698badcfe");
}

TEST(BinaryToStr, EmptyInput)
{
    EXPECT_EQ(translate_binary_to_str(nullptr, 0), "");
    EXPECT_EQ(swap_and_translate_binary_to_str(nullptr, 0), "");
}
