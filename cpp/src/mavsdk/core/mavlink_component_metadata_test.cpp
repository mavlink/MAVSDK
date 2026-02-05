#include "mavlink_component_metadata.h"

#include <gtest/gtest.h>

using namespace mavsdk;

TEST(MavlinkComponentMetadata, mavftp_uri)
{
    std::string download_path;
    uint8_t compid = 3;
    EXPECT_FALSE(MavlinkComponentMetadata::uri_is_mavlinkftp(
        "http://url.com/file.json", download_path, compid));

    EXPECT_TRUE(
        MavlinkComponentMetadata::uri_is_mavlinkftp("mftp:///file.json", download_path, compid));
    EXPECT_EQ(compid, 3);
    EXPECT_EQ(download_path, "/file.json");

    EXPECT_TRUE(MavlinkComponentMetadata::uri_is_mavlinkftp(
        "mftp://[;comp=39]/path/to/file.json", download_path, compid));
    EXPECT_EQ(compid, 39);
    EXPECT_EQ(download_path, "/path/to/file.json");
}

TEST(MavlinkComponentMetadata, filename_from_uri)
{
    EXPECT_EQ(
        MavlinkComponentMetadata::filename_from_uri("http://url.com/file.json.xz"), "file.json.xz");
    EXPECT_EQ(
        MavlinkComponentMetadata::filename_from_uri("http://url.com/path/file.json?a=x&b=y"),
        "file.json");
}
