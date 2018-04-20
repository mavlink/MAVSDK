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
        EXPECT_EQ(settings.size(), 7);
        // LogDebug() << "Found settings:";
        // for (const auto &setting : settings) {
        //     LogDebug() << " - " << setting.first;
        // }
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
        EXPECT_EQ(settings.size(), 9);
        // LogDebug() << "Found settings:";
        // for (const auto &setting : settings) {
        //     LogDebug() << " - " << setting.first;
        // }
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
        // We can only set CAM_COLORMODE in photo mode
        MAVLinkParameters::ParamValue value;
        value.set_uint32(0);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
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
        // Currently not applicable because exposure mode is in Auto.
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_FALSE(cd.get_possible_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 0);
    }

    {
        // Set exposure mode to manual
        MAVLinkParameters::ParamValue value;
        value.set_uint32(1);
        EXPECT_TRUE(cd.set_setting("CAM_EXPMODE", value));

        // Currently not applicable because exposure mode is in Auto.
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 12);
    }

    {
        // Test VIDRES without range restrictions in h.264.
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_VIDRES", values));
        EXPECT_EQ(values.size(), 32);
    }

    {
        // Set it to one that is allowed.
        MAVLinkParameters::ParamValue value;
        value.set_uint32(3);
        EXPECT_TRUE(cd.set_setting("CAM_VIDRES", value));
    }

    {
        // Now switch to HEVC
        MAVLinkParameters::ParamValue value;
        value.set_uint32(3);
        EXPECT_TRUE(cd.set_setting("CAM_VIDFMT", value));
    }

    {
        // Test VIDRES with range restrictions in HEVC.
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_VIDRES", values));
        EXPECT_EQ(values.size(), 26);
    }

    {
        // Then one that is allowed.
        MAVLinkParameters::ParamValue value;
        value.set_uint32(5);
        EXPECT_TRUE(cd.set_setting("CAM_VIDRES", value));
    }

    {
        // Back to h.264
        MAVLinkParameters::ParamValue value;
        value.set_uint32(1);
        EXPECT_TRUE(cd.set_setting("CAM_VIDFMT", value));
    }

    {
        // Test VIDRES without range restrictions in h.264.
        std::vector<MAVLinkParameters::ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_VIDRES", values));
        EXPECT_EQ(values.size(), 32);
    }

    {
        // And 4K 60 Hz is now allowed again.
        MAVLinkParameters::ParamValue value;
        value.set_uint32(0);
        EXPECT_TRUE(cd.set_setting("CAM_VIDRES", value));
    }
}

TEST(CameraDefinition, E90SettingsToUpdate)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    {
        std::vector<std::string> params;
        EXPECT_TRUE(cd.get_unknown_params(params));
        EXPECT_EQ(params.size(), 16);
        for (const auto &param : params) {
            LogInfo() << param;
        }
    }

    {
        MAVLinkParameters::ParamValue value;
        value.set_uint32(1);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    // Now that we set one param it should be less that we need to fetch.
    {
        std::vector<std::string> params;
        EXPECT_TRUE(cd.get_unknown_params(params));
        EXPECT_EQ(params.size(), 15);
    }

    cd.set_all_params_unknown();

    {
        std::vector<std::string> params;
        EXPECT_TRUE(cd.get_unknown_params(params));
        EXPECT_EQ(params.size(), 16);
    }

}

TEST(CameraDefinition, E90SettingsCauseUpdates)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    cd.assume_default_settings();

    {
        std::vector<std::string> params;
        EXPECT_TRUE(cd.get_unknown_params(params));
        EXPECT_EQ(params.size(), 0);
        for (const auto &param : params) {
            LogInfo() << param;
        }
    }

    {
        MAVLinkParameters::ParamValue value;
        value.set_uint32(0);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    // Now that we set one param it should be less that we need to fetch.
    {
        std::vector<std::string> params;
        EXPECT_TRUE(cd.get_unknown_params(params));
        EXPECT_EQ(params.size(), 4);

        // TODO: improve the poor man's vector search.
        bool found_shutterspd = false;
        bool found_iso = false;
        bool found_vidres = false;
        //bool found_aspectratio = false;
        bool found_photoratio = false;

        for (const auto &param : params) {
            if (strcmp("CAM_SHUTTERSPD", param.c_str()) == 0) {
                found_shutterspd = true;
            }
            if (strcmp("CAM_ISO", param.c_str()) == 0) {
                found_iso = true;
            }
            if (strcmp("CAM_VIDRES", param.c_str()) == 0) {
                found_vidres = true;
            }
            // We don't yet handle ASPECTRATIO
            //if (strcmp("CAM_ASPECTRATIO", param.c_str()) == 0) {
            //    found_aspectratio = true;
            //}
            if (strcmp("CAM_PHOTORATIO", param.c_str()) == 0) {
                found_photoratio = true;
            }
        }

        EXPECT_TRUE(found_shutterspd);
        EXPECT_TRUE(found_iso);
        EXPECT_TRUE(found_vidres);
        //EXPECT_TRUE(found_aspectratio);
        EXPECT_TRUE(found_photoratio);
    }
}

TEST(CameraDefinition, E90OptionValues)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    MAVLinkParameters::ParamValue value1;
    value1.set_float(0.02f);

    MAVLinkParameters::ParamValue value2;

    // First try the invalid case.
    EXPECT_FALSE(cd.get_option_value("CAM_BLABLA", "0.02", value2));

    EXPECT_TRUE(cd.get_option_value("CAM_SHUTTERSPD", "0.02", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set_float(2.f);
    EXPECT_TRUE(cd.get_option_value("CAM_SHUTTERSPD", "2", value2));
    EXPECT_TRUE(value1 == value2);

    // Try an invalid shutter speed
    EXPECT_FALSE(cd.get_option_value("CAM_SHUTTERSPD", "100", value2));

    value1.set_uint32(100);
    EXPECT_TRUE(cd.get_option_value("CAM_ISO", "100", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set_uint32(200);
    EXPECT_TRUE(cd.get_option_value("CAM_ISO", "200", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set_float(-2.5f);
    EXPECT_TRUE(cd.get_option_value("CAM_EV", "-2.5", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set_float(1);
    EXPECT_TRUE(cd.get_option_value("CAM_EV", "1", value2));
    EXPECT_TRUE(value1 == value2);

    // Try an invalid EV
    EXPECT_FALSE(cd.get_option_value("CAM_EV", "-42.0", value2));
}

TEST(CameraDefinition, E90SettingHumanReadable)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    std::string description {};
    EXPECT_TRUE(cd.get_setting_str("CAM_SHUTTERSPD", description));
    EXPECT_STREQ(description.c_str(), "Shutter Speed");

    EXPECT_TRUE(cd.get_setting_str("CAM_ISO", description));
    EXPECT_STREQ(description.c_str(), "ISO");

    EXPECT_TRUE(cd.get_setting_str("CAM_EV", description));
    EXPECT_STREQ(description.c_str(), "Exposure Compensation");

    EXPECT_TRUE(cd.get_setting_str("CAM_VIDRES", description));
    EXPECT_STREQ(description.c_str(), "Video Resolution");

    // Try something that doesn't exist as well.
    EXPECT_FALSE(cd.get_setting_str("PIPAPO", description));
    EXPECT_STREQ(description.c_str(), "");

    EXPECT_TRUE(cd.get_setting_str("CAM_VIDRES", description));
    EXPECT_STREQ(description.c_str(), "Video Resolution");
}

TEST(CameraDefinition, E90OptionHumanReadable)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    std::string description {};
    EXPECT_TRUE(cd.get_option_str("CAM_SHUTTERSPD", "0.0025", description));
    EXPECT_STREQ(description.c_str(), "1/400");

    EXPECT_TRUE(cd.get_option_str("CAM_SHUTTERSPD", "0.004", description));
    EXPECT_STREQ(description.c_str(), "1/250");

    EXPECT_TRUE(cd.get_option_str("CAM_SHUTTERSPD", "4", description));
    EXPECT_STREQ(description.c_str(), "4");

    EXPECT_FALSE(cd.get_option_str("CAM_SHUTTERSPD", "0.0", description));
    EXPECT_STREQ(description.c_str(), "");

    EXPECT_TRUE(cd.get_option_str("CAM_ISO", "100", description));
    EXPECT_STREQ(description.c_str(), "100");

    EXPECT_TRUE(cd.get_option_str("CAM_ISO", "200", description));
    EXPECT_STREQ(description.c_str(), "200");

    EXPECT_TRUE(cd.get_option_str("CAM_EV", "-2.5", description));
    EXPECT_STREQ(description.c_str(), "-2.5");

    EXPECT_TRUE(cd.get_option_str("CAM_EV", "1", description));
    EXPECT_STREQ(description.c_str(), "+1");

    EXPECT_TRUE(cd.get_option_str("CAM_WBMODE", "2", description));
    EXPECT_STREQ(description.c_str(), "Sunrise");

    EXPECT_TRUE(cd.get_option_str("CAM_WBMODE", "5", description));
    EXPECT_STREQ(description.c_str(), "Cloudy");

    EXPECT_FALSE(cd.get_option_str("PIPAPO", "123", description));
    EXPECT_STREQ(description.c_str(), "");
}
