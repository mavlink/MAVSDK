#include "crc32.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cstdint>

using namespace mavsdk;

TEST(Crc32, EmptyIsZero)
{
    Crc32 crc;
    EXPECT_EQ(crc.get(), 0u);
    const uint8_t* empty = nullptr;
    // Zero-length add must not read bytes even if src is null.
    EXPECT_EQ(crc.add(empty, 0), 0u);
    EXPECT_EQ(crc.get(), 0u);
}

TEST(Crc32, KnownVector123456789)
{
    // MAVLink-style CRC32 (start 0, no final XOR) of ASCII "123456789"
    const uint8_t data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    Crc32 crc;
    EXPECT_EQ(crc.add(data, sizeof(data)), 0x2dfd2d88u);
    EXPECT_EQ(crc.get(), 0x2dfd2d88u);
}

TEST(Crc32, IncrementalMatchesOneShot)
{
    const uint8_t part1[] = {'a', 'b'};
    const uint8_t part2[] = {'c'};
    const uint8_t all[] = {'a', 'b', 'c'};

    Crc32 stepwise;
    stepwise.add(part1, sizeof(part1));
    stepwise.add(part2, sizeof(part2));

    Crc32 oneshot;
    oneshot.add(all, sizeof(all));

    EXPECT_EQ(stepwise.get(), oneshot.get());
    EXPECT_EQ(oneshot.get(), 0xca6598d0u);
}

TEST(Crc32, Single0xFF)
{
    const uint8_t data[] = {0xff};
    Crc32 crc;
    EXPECT_EQ(crc.add(data, 1), 0x2d02ef8du);
}

TEST(Crc32, LongerVectorIncrementalMatches)
{
    std::vector<uint8_t> data(256);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<uint8_t>(i);
    }

    Crc32 oneshot;
    oneshot.add(data.data(), static_cast<uint32_t>(data.size()));

    Crc32 stepwise;
    stepwise.add(data.data(), 50);
    stepwise.add(data.data() + 50, 100);
    stepwise.add(data.data() + 150, 106);

    EXPECT_EQ(stepwise.get(), oneshot.get());
    // Stable golden for MAVLink CRC32 of bytes 0..255.
    EXPECT_EQ(oneshot.get(), 0x2493092bu);
}
