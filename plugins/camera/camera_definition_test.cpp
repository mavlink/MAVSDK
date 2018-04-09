#include "camera_definition.h"
#include "log.h"
#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

using namespace dronecore;

static const char *e90_unit_test_file = "plugins/camera/e90_unit_test.xml";

TEST(CameraDefinition, E90LoadInfoFile)
{
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));
    EXPECT_STREQ(cd.get_vendor().c_str(), "Yuneec");
    EXPECT_STREQ(cd.get_model().c_str(), "E90");
}

TEST(CameraDefinition, E90LoadInfoString)
{
    std::ifstream file_stream(e90_unit_test_file);
    ASSERT_TRUE(file_stream.is_open());

    // Read whole file into string
    std::string content;
    std::getline(file_stream, content, '\0');

    CameraDefinition cd;
    ASSERT_TRUE(cd.load_string(content));
    EXPECT_STREQ(cd.get_vendor().c_str(), "Yuneec");
    EXPECT_STREQ(cd.get_model().c_str(), "E90");
}

TEST(CameraDefinition, E90CheckDefaultSettings)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    cd.assume_default_settings();

    {
        std::map<std::string, MAVLinkParameters::ParamValue> settings {};
        EXPECT_TRUE(cd.get_all_settings(settings));
        EXPECT_EQ(settings.size(), 16);

        EXPECT_EQ(settings["CAM_MODE"].get_uint32(), 1);
        EXPECT_EQ(settings["CAM_WBMODE"].get_uint32(), 0);
        EXPECT_EQ(settings["CAM_EXPMODE"].get_uint32(), 0);
        EXPECT_FLOAT_EQ(settings["CAM_SHUTTERSPD"].get_float(), 0.016666f);
        EXPECT_EQ(settings["CAM_ISO"].get_uint32(), 100);
        EXPECT_FLOAT_EQ(settings["CAM_EV"].get_float(), 0.0f);
        EXPECT_EQ(settings["CAM_VIDRES"].get_uint32(), 0);
        EXPECT_EQ(settings["CAM_VIDFMT"].get_uint32(), 1);
        EXPECT_EQ(settings["CAM_PHOTORATIO"].get_uint8(), 1);
        EXPECT_EQ(settings["CAM_METERING"].get_uint32(), 0);
        EXPECT_EQ(settings["CAM_COLORMODE"].get_uint32(), 1);
        EXPECT_EQ(settings["CAM_FLICKER"].get_uint32(), 0);
        EXPECT_EQ(settings["CAM_PHOTOFMT"].get_uint32(), 0);
        EXPECT_EQ(settings["CAM_PHOTOQUAL"].get_uint32(), 1);
        EXPECT_EQ(settings["CAM_IMAGEDEWARP"].get_uint8(), 0);
        EXPECT_EQ(settings["CAM_COLORENCODE"].get_uint32(), 0);
    }

    // Get only settings for video mode.
    {
        MAVLinkParameters::ParamValue value;
        value.set_uint32(1);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    {
        std::map<std::string, MAVLinkParameters::ParamValue> settings {};
        EXPECT_TRUE(cd.get_possible_settings(settings));
        EXPECT_EQ(settings.size(), 9);
    }

    // Get only settings for photo mode.
    {
        MAVLinkParameters::ParamValue value;
        value.set_uint32(0);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    {
        std::map<std::string, MAVLinkParameters::ParamValue> settings {};
        EXPECT_TRUE(cd.get_possible_settings(settings));
        EXPECT_EQ(settings.size(), 12);
    }
}

TEST(CameraDefinition, E90ChangeSettings)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    cd.assume_default_settings();

    {
        // Check default
        MAVLinkParameters::ParamValue value;
        EXPECT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get_uint32(), 0);
    }

    {
        // Set WBMODE to 1
        MAVLinkParameters::ParamValue value;
        value.set_uint32(1);
        EXPECT_TRUE(cd.set_setting("CAM_WBMODE", value));
    }

    {
        // Check if WBMODE was correctly set
        MAVLinkParameters::ParamValue value;
        EXPECT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get_uint32(), 1);
    }

    {
        // Interleave COLORMODE, first check default
        MAVLinkParameters::ParamValue value;
        EXPECT_TRUE(cd.get_setting("CAM_COLORMODE", value));
        EXPECT_EQ(value.get_uint32(), 1);
    }

    {
        // Then set COLORMODE to 5
        MAVLinkParameters::ParamValue value;
        value.set_uint32(5);
        EXPECT_TRUE(cd.set_setting("CAM_COLORMODE", value));
    }

    {
        // COLORMODE should now be 5
        MAVLinkParameters::ParamValue value;
        EXPECT_TRUE(cd.get_setting("CAM_COLORMODE", value));
        EXPECT_EQ(value.get_uint32(), 5);
    }

    {
        // WBMODE should still be 1
        MAVLinkParameters::ParamValue value;
        EXPECT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get_uint32(), 1);
    }

    {
        // Change WBMODE to 7
        MAVLinkParameters::ParamValue value;
        value.set_uint32(7);
        EXPECT_TRUE(cd.set_setting("CAM_WBMODE", value));
    }

    {
        // And check WBMODE again
        MAVLinkParameters::ParamValue value;
        EXPECT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get_uint32(), 7);
    }
}

TEST(CameraDefinition, E90ShowOptions)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    cd.assume_default_settings();

    {
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_all_options("CAM_WBMODE", values));
        EXPECT_EQ(values.size(), 8);
    }

    {
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_all_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 19);
    }

    {
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 12);
    }
}

