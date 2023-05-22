
#include "mavlink_statustext_handler.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(MavlinkStatustextHandler, Severities)
{
    const std::vector<std::pair<MAV_SEVERITY, std::string>> severities{
        {MAV_SEVERITY_DEBUG, "debug"},
        {MAV_SEVERITY_INFO, "info"},
        {MAV_SEVERITY_NOTICE, "notice"},
        {MAV_SEVERITY_WARNING, "warning"},
        {MAV_SEVERITY_ERROR, "error"},
        {MAV_SEVERITY_CRITICAL, "critical"},
        {MAV_SEVERITY_ALERT, "alert"},
        {MAV_SEVERITY_EMERGENCY, "emergency"}};

    for (const auto& severity : severities) {
        auto result = MavlinkStatustextHandler::severity_str(severity.first);
        EXPECT_EQ(severity.second, result);
    }
}

TEST(MavlinkStatustextHandler, WrongSeverity)
{
    auto result = MavlinkStatustextHandler::severity_str(static_cast<MAV_SEVERITY>(255));
    EXPECT_EQ(result, "unknown severity");
}

TEST(MavlinkStatustextHandler, SingleStatustextWithNull)
{
    mavlink_statustext_t statustext{};
    auto str = std::string{"Hello Reader"};
    strncpy(statustext.text, str.c_str(), sizeof(statustext.text) - 1);

    MavlinkStatustextHandler handler;
    auto result = handler.process(statustext);
    ASSERT_TRUE(result);
    EXPECT_EQ(str, result.value().text);
}

TEST(MavlinkStatustextHandler, SingleStatustextWithoutNull)
{
    mavlink_statustext_t statustext{};
    auto str = std::string{"asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;asdfghjkl;"};
    strncpy(statustext.text, str.c_str(), sizeof(statustext.text));

    MavlinkStatustextHandler handler;
    auto result = handler.process(statustext);
    ASSERT_TRUE(result);
    EXPECT_EQ(str, result.value().text);
}

TEST(MavlinkStatustextHandler, MultiStatustext)
{
    const std::string str = "Lorem ipsum dolor sit amet, consectetur adipiscing"
                            " elit, sed do eiusmod tempor incididunt ut labore "
                            "et dolore magna aliqua. Venenatis cras sed felis e"
                            "get velit aliquet. Ac feugiat sed lectus vestibulu"
                            "m. Condimentum lacinia quis vel eros donec ac odio"
                            ". Eleifend mi in nulla posuere sollicitudin aliqua"
                            "m ultrices. Fusce ut placerat orci nulla pellentes"
                            "que dignissim.";

    constexpr std::size_t chunk_len = sizeof(mavlink_statustext_t::text);

    MavlinkStatustextHandler handler;

    uint8_t chunk_seq = 0;
    for (std::size_t start = 0; start < str.size(); start += chunk_len) {
        bool is_last = (start + chunk_len >= str.size());

        const std::size_t len = std::min(chunk_len, str.size() - start);
        const auto chunk = str.substr(start, len);

        mavlink_statustext_t statustext{};
        strncpy(statustext.text, chunk.c_str(), sizeof(statustext.text));
        statustext.id = 42;
        statustext.chunk_seq = chunk_seq;

        const auto result = handler.process(statustext);

        if (is_last) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result.value().text, str);
        } else {
            EXPECT_FALSE(result);
        }
        ++chunk_seq;
    }
}

TEST(MavlinkStatustextHandler, MultiStatustextDivisibleByChunkLen)
{
    const std::string str = "This string is unfortunately exactly the length of"
                            "two chunks which means it needs another message ju"
                            "st to send the strange zero termination character!";

    constexpr std::size_t chunk_len = sizeof(mavlink_statustext_t::text);

    MavlinkStatustextHandler handler;

    uint8_t chunk_seq = 0;
    for (std::size_t start = 0; start <= str.size(); start += chunk_len) {
        bool is_last = (start + chunk_len > str.size());

        const std::size_t len = std::min(chunk_len, str.size() - start);
        const auto chunk = str.substr(start, len);

        mavlink_statustext_t statustext{};
        if (len > 0) {
            strncpy(statustext.text, chunk.c_str(), sizeof(statustext.text));
        } else {
            // Last message with null only.
            statustext.text[0] = '\0';
        }
        statustext.id = 42;
        statustext.chunk_seq = chunk_seq;

        const auto result = handler.process(statustext);

        if (is_last) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result.value().text, str);
        } else {
            EXPECT_FALSE(result);
        }
        ++chunk_seq;
    }
}

TEST(MavlinkStatustextHandler, MultiStatustextMissingPart)
{
    const std::string str = "Lorem ipsum dolor sit amet, consectetur adipiscing"
                            " elit, sed do eiusmod tempor incididunt ut labore "
                            "et dolore magna aliqua. Venenatis cras sed felis e"
                            "get velit aliquet. Ac feugiat sed lectus vestibulu"
                            "m. Condimentum lacinia quis vel eros donec ac odio"
                            ". Eleifend mi in nulla posuere sollicitudin aliqua"
                            "m ultrices. Fusce ut placerat orci nulla pellentes"
                            "que dignissim.";

    const std::string str_missing = "Lorem ipsum dolor sit amet, consectetur adipiscing"
                                    " elit, sed do eiusmod tempor incididunt ut labore "
                                    "et dolore magna aliqua. Venenatis cras sed felis e"
                                    "[ missing ... ]"
                                    "m. Condimentum lacinia quis vel eros donec ac odio"
                                    ". Eleifend mi in nulla posuere sollicitudin aliqua"
                                    "m ultrices. Fusce ut placerat orci nulla pellentes"
                                    "que dignissim.";

    constexpr std::size_t chunk_len = sizeof(mavlink_statustext_t::text);

    MavlinkStatustextHandler handler;

    uint8_t chunk_seq = 0;
    for (std::size_t start = 0; start < str.size(); start += chunk_len) {
        bool is_last = (start + chunk_len >= str.size());

        const std::size_t len = std::min(chunk_len, str.size() - start);
        const auto chunk = str.substr(start, len);

        mavlink_statustext_t statustext{};
        strncpy(statustext.text, chunk.c_str(), sizeof(statustext.text));
        statustext.id = 42;
        statustext.chunk_seq = chunk_seq;

        if (chunk_seq != 3) {
            const auto result = handler.process(statustext);

            if (is_last) {
                ASSERT_TRUE(result);
                EXPECT_EQ(result.value().text, str_missing);
            } else {
                EXPECT_FALSE(result);
            }
        }
        ++chunk_seq;
    }
}

TEST(MavlinkStatustextHandler, MultiStatustextConsecutive)
{
    MavlinkStatustextHandler handler;

    {
        const std::string str = "Lorem ipsum dolor sit amet, consectetur adipiscing"
                                " elit, sed do eiusmod tempor incididunt ut labore "
                                "et dolore magna aliqua. Venenatis cras sed felis e"
                                "get velit aliquet. Ac feugiat sed lectus vestibulu"
                                "m. Condimentum lacinia quis vel eros donec ac odio"
                                ". Eleifend mi in nulla posuere sollicitudin aliqua"
                                "m ultrices. Fusce ut placerat orci nulla pellentes"
                                "que dignissim.";

        constexpr std::size_t chunk_len = sizeof(mavlink_statustext_t::text);

        uint8_t chunk_seq = 0;
        for (std::size_t start = 0; start < str.size(); start += chunk_len) {
            bool is_last = (start + chunk_len >= str.size());

            const std::size_t len = std::min(chunk_len, str.size() - start);
            const auto chunk = str.substr(start, len);

            mavlink_statustext_t statustext{};
            strncpy(statustext.text, chunk.c_str(), sizeof(statustext.text));
            statustext.id = 42;
            statustext.chunk_seq = chunk_seq;

            const auto result = handler.process(statustext);

            if (is_last) {
                ASSERT_TRUE(result);
                EXPECT_EQ(result.value().text, str);
            } else {
                EXPECT_FALSE(result);
            }
            ++chunk_seq;
        }
    }

    {
        const std::string str = "Blablablablablablablablablablablablablablablablabl"
                                "FooFooFooFoo.";

        constexpr std::size_t chunk_len = sizeof(mavlink_statustext_t::text);

        uint8_t chunk_seq = 0;
        for (std::size_t start = 0; start < str.size(); start += chunk_len) {
            bool is_last = (start + chunk_len >= str.size());

            const std::size_t len = std::min(chunk_len, str.size() - start);
            const auto chunk = str.substr(start, len);

            mavlink_statustext_t statustext{};
            strncpy(statustext.text, chunk.c_str(), sizeof(statustext.text));
            statustext.id = 43;
            statustext.chunk_seq = chunk_seq;

            const auto result = handler.process(statustext);

            if (is_last) {
                ASSERT_TRUE(result);
                EXPECT_EQ(result.value().text, str);
            } else {
                EXPECT_FALSE(result);
            }
            ++chunk_seq;
        }
    }
}
