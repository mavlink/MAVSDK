#include "camera_definition.h"
#include "log.h"
#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>

using namespace mavsdk;

static const std::string e90_unit_test_file = "src/mavsdk/plugins/camera/e90_unit_test.xml";

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
        std::unordered_map<std::string, ParamValue> settings{};
        EXPECT_TRUE(cd.get_all_settings(settings));
        EXPECT_EQ(settings.size(), 17);

        EXPECT_EQ(settings["CAM_MODE"].get<uint32_t>(), 1);
        EXPECT_EQ(settings["CAM_WBMODE"].get<uint32_t>(), 0);
        EXPECT_EQ(settings["CAM_EXPMODE"].get<uint32_t>(), 0);
        EXPECT_FLOAT_EQ(settings["CAM_SHUTTERSPD"].get<float>(), 0.016666f);
        EXPECT_EQ(settings["CAM_ISO"].get<uint32_t>(), 100);
        EXPECT_FLOAT_EQ(settings["CAM_EV"].get<float>(), 0.0f);
        EXPECT_EQ(settings["CAM_VIDRES"].get<uint32_t>(), 0);
        EXPECT_EQ(settings["CAM_VIDFMT"].get<uint32_t>(), 1);
        EXPECT_EQ(settings["CAM_PHOTORATIO"].get<uint8_t>(), 1);
        EXPECT_EQ(settings["CAM_METERING"].get<uint32_t>(), 0);
        EXPECT_EQ(settings["CAM_COLORMODE"].get<uint32_t>(), 1);
        EXPECT_EQ(settings["CAM_FLICKER"].get<uint32_t>(), 0);
        EXPECT_EQ(settings["CAM_PHOTOFMT"].get<uint32_t>(), 0);
        EXPECT_EQ(settings["CAM_PHOTOQUAL"].get<uint32_t>(), 1);
        EXPECT_EQ(settings["CAM_IMAGEDEWARP"].get<uint8_t>(), 0);
        EXPECT_EQ(settings["CAM_COLORENCODE"].get<uint32_t>(), 0);
        EXPECT_EQ(settings["CAM_CUSTOMWB"].get<uint16_t>(), 5500);
    }

    // Get only settings for video mode.
    {
        ParamValue value;
        value.set<uint32_t>(1);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    {
        std::unordered_map<std::string, ParamValue> settings{};
        EXPECT_TRUE(cd.get_possible_settings(settings));
        EXPECT_EQ(settings.size(), 6);
        // LogDebug() << "Found settings:";
        // for (const auto &setting : settings) {
        //     LogDebug() << " - " << setting.first;
        // }
    }

    // Get only settings for photo mode.
    {
        ParamValue value;
        value.set<uint32_t>(0);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    {
        std::unordered_map<std::string, ParamValue> settings{};
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
        ParamValue value;
        ASSERT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get<uint32_t>(), 0);
    }

    {
        // We can only set CAM_COLORMODE in photo mode
        ParamValue value;
        value.set<uint32_t>(0);
        ASSERT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    {
        // Set WBMODE to 1
        ParamValue value;
        value.set<uint32_t>(1);
        EXPECT_TRUE(cd.set_setting("CAM_WBMODE", value));
    }

    {
        // Check if WBMODE was correctly set
        ParamValue value;
        ASSERT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get<uint32_t>(), 1);
    }

    {
        // Interleave COLORMODE, first check default
        ParamValue value;
        ASSERT_TRUE(cd.get_setting("CAM_COLORMODE", value));
        EXPECT_EQ(value.get<uint32_t>(), 1);
    }

    {
        // Then set COLORMODE to 5
        ParamValue value;
        value.set<uint32_t>(5);
        ASSERT_TRUE(cd.set_setting("CAM_COLORMODE", value));
    }

    {
        // COLORMODE should now be 5
        ParamValue value;
        ASSERT_TRUE(cd.get_setting("CAM_COLORMODE", value));
        EXPECT_EQ(value.get<uint32_t>(), 5);
    }

    {
        // WBMODE should still be 1
        ParamValue value;
        ASSERT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get<uint32_t>(), 1);
    }

    {
        // Change WBMODE to 7
        ParamValue value;
        value.set<uint32_t>(7);
        ASSERT_TRUE(cd.set_setting("CAM_WBMODE", value));
    }

    {
        // And check WBMODE again
        ParamValue value;
        ASSERT_TRUE(cd.get_setting("CAM_WBMODE", value));
        EXPECT_EQ(value.get<uint32_t>(), 7);
    }
}

TEST(CameraDefinition, E90ShowOptions)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    cd.assume_default_settings();

    {
        std::vector<ParamValue> values;
        EXPECT_TRUE(cd.get_all_options("CAM_WBMODE", values));
        EXPECT_EQ(values.size(), 8);
    }

    {
        std::vector<ParamValue> values;
        EXPECT_TRUE(cd.get_all_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 19);
    }

    {
        // Currently not applicable because exposure mode is in Auto.
        std::vector<ParamValue> values;
        EXPECT_FALSE(cd.get_possible_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 0);
    }

    {
        // Set exposure mode to manual
        ParamValue value;
        value.set<uint32_t>(1);
        EXPECT_TRUE(cd.set_setting("CAM_EXPMODE", value));

        // Currently not applicable because exposure mode is in Auto.
        std::vector<ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_SHUTTERSPD", values));
        EXPECT_EQ(values.size(), 12);
    }

    {
        // Test VIDRES without range restrictions in h.264.
        std::vector<ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_VIDRES", values));
        EXPECT_EQ(values.size(), 32);
    }

    {
        // Set it to one that is allowed.
        ParamValue value;
        value.set<uint32_t>(3);
        EXPECT_TRUE(cd.set_setting("CAM_VIDRES", value));
    }

    {
        // Now switch to HEVC
        ParamValue value;
        value.set<uint32_t>(3);
        EXPECT_TRUE(cd.set_setting("CAM_VIDFMT", value));
    }

    {
        // Test VIDRES with range restrictions in HEVC.
        std::vector<ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_VIDRES", values));
        EXPECT_EQ(values.size(), 26);
    }

    {
        // Then one that is allowed.
        ParamValue value;
        value.set<uint32_t>(5);
        EXPECT_TRUE(cd.set_setting("CAM_VIDRES", value));
    }

    {
        // Back to h.264
        ParamValue value;
        value.set<uint32_t>(1);
        EXPECT_TRUE(cd.set_setting("CAM_VIDFMT", value));
    }

    {
        // Test VIDRES without range restrictions in h.264.
        std::vector<ParamValue> values;
        EXPECT_TRUE(cd.get_possible_options("CAM_VIDRES", values));
        EXPECT_EQ(values.size(), 32);
    }

    {
        // And 4K 60 Hz is now allowed again.
        ParamValue value;
        value.set<uint32_t>(0);
        EXPECT_TRUE(cd.set_setting("CAM_VIDRES", value));
    }
}

TEST(CameraDefinition, E90SettingsToUpdate)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    {
        std::vector<std::pair<std::string, ParamValue>> params;
        cd.get_unknown_params(params);
        EXPECT_EQ(params.size(), 17);
    }

    {
        ParamValue value;
        value.set<uint32_t>(1);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    // Now that we set one param it should be less that we need to fetch.
    {
        std::vector<std::pair<std::string, ParamValue>> params;
        cd.get_unknown_params(params);
        EXPECT_EQ(params.size(), 16);
    }

    cd.set_all_params_unknown();

    {
        std::vector<std::pair<std::string, ParamValue>> params;
        cd.get_unknown_params(params);
        EXPECT_EQ(params.size(), 17);
    }
}

TEST(CameraDefinition, E90SettingsCauseUpdates)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    cd.assume_default_settings();

    {
        std::vector<std::pair<std::string, ParamValue>> params;
        cd.get_unknown_params(params);
        EXPECT_EQ(params.size(), 0);
        for (const auto& param : params) {
            LogInfo() << param.first;
        }
    }

    {
        ParamValue value;
        value.set<uint32_t>(0);
        EXPECT_TRUE(cd.set_setting("CAM_MODE", value));
    }

    // Now that we set one param it should be less that we need to fetch.
    {
        std::vector<std::pair<std::string, ParamValue>> params;
        cd.get_unknown_params(params);
        EXPECT_EQ(params.size(), 4);

        // TODO: improve the poor man's vector search.
        bool found_shutterspd = false;
        bool found_iso = false;
        bool found_vidres = false;
        // bool found_aspectratio = false;
        bool found_photoratio = false;

        for (const auto& param : params) {
            if (param.first == "CAM_SHUTTERSPD") {
                found_shutterspd = true;
            }
            if (param.first == "CAM_ISO") {
                found_iso = true;
            }
            if (param.first == "CAM_VIDRES") {
                found_vidres = true;
            }
            // We don't yet handle ASPECTRATIO
            // if (param.first == "CAM_ASPECTRATIO") {
            //    found_aspectratio = true;
            //}
            if (param.first == "CAM_PHOTORATIO") {
                found_photoratio = true;
            }
        }

        EXPECT_TRUE(found_shutterspd);
        EXPECT_TRUE(found_iso);
        EXPECT_TRUE(found_vidres);
        // EXPECT_TRUE(found_aspectratio);
        EXPECT_TRUE(found_photoratio);
    }
}

TEST(CameraDefinition, E90OptionValues)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(e90_unit_test_file));

    ParamValue value1;
    value1.set<float>(0.02f);

    ParamValue value2;

    // First try the invalid case.
    EXPECT_FALSE(cd.get_option_value("CAM_BLABLA", "0.02", value2));

    EXPECT_TRUE(cd.get_option_value("CAM_SHUTTERSPD", "0.02", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set<float>(2.f);
    EXPECT_TRUE(cd.get_option_value("CAM_SHUTTERSPD", "2", value2));
    EXPECT_TRUE(value1 == value2);

    // Try an invalid shutter speed
    EXPECT_FALSE(cd.get_option_value("CAM_SHUTTERSPD", "100", value2));

    value1.set<uint32_t>(100);
    EXPECT_TRUE(cd.get_option_value("CAM_ISO", "100", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set<uint32_t>(200);
    EXPECT_TRUE(cd.get_option_value("CAM_ISO", "200", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set<float>(-2.5f);
    EXPECT_TRUE(cd.get_option_value("CAM_EV", "-2.5", value2));
    EXPECT_TRUE(value1 == value2);

    value1.set<float>(1);
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

    std::string description{};
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

    std::string description{};
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

static const std::string uvc_unit_test_file = "src/mavsdk/plugins/camera/uvc_unit_test.xml";

TEST(CameraDefinition, UVCLoadInfoFile)
{
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(uvc_unit_test_file));
    EXPECT_STREQ(cd.get_vendor().c_str(), "Logitech C270HD Webcam");
    EXPECT_STREQ(cd.get_model().c_str(), "UVC Camera");
}

TEST(CameraDefinition, UVCLoadInfoString)
{
    std::ifstream file_stream(uvc_unit_test_file);
    ASSERT_TRUE(file_stream.is_open());

    // Read whole file into string
    std::string content;
    std::getline(file_stream, content, '\0');

    CameraDefinition cd;
    ASSERT_TRUE(cd.load_string(content));
    EXPECT_STREQ(cd.get_vendor().c_str(), "Logitech C270HD Webcam");
    EXPECT_STREQ(cd.get_model().c_str(), "UVC Camera");
}

TEST(CameraDefinition, UVCCheckDefaultSettings)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(uvc_unit_test_file));

    cd.assume_default_settings();

    std::unordered_map<std::string, ParamValue> settings{};
    EXPECT_TRUE(cd.get_all_settings(settings));
    EXPECT_EQ(settings.size(), 13);

    EXPECT_TRUE(cd.is_setting_range("brightness"));
    EXPECT_FALSE(cd.is_setting_range("exp-mode"));

    EXPECT_EQ(settings["camera-mode"].get<uint32_t>(), 1);
    EXPECT_EQ(settings["brightness"].get<int32_t>(), 128);
    EXPECT_EQ(settings["contrast"].get<int32_t>(), 32);
    EXPECT_EQ(settings["saturation"].get<int32_t>(), 32);
    EXPECT_EQ(settings["gain"].get<int32_t>(), 64);
    EXPECT_EQ(settings["sharpness"].get<int32_t>(), 24);
    EXPECT_EQ(settings["backlight"].get<int32_t>(), 0);
    EXPECT_EQ(settings["power-mode"].get<int32_t>(), 0);
    EXPECT_EQ(settings["wb-mode"].get<int32_t>(), 1);
    EXPECT_EQ(settings["wb-temp"].get<int32_t>(), 4000);
    EXPECT_EQ(settings["exp-mode"].get<int32_t>(), 3);
    EXPECT_EQ(settings["exp-absolute"].get<int32_t>(), 166);
    EXPECT_EQ(settings["exp-priority"].get<int32_t>(), 1);
}

TEST(CameraDefinition, UVCCheckPossibleSettings)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(uvc_unit_test_file));

    cd.assume_default_settings();

    // Get only settings for WB mode Auto.
    {
        ParamValue value;
        value.set<int32_t>(1);
        EXPECT_TRUE(cd.set_setting("wb-mode", value));
    }

    // And exposure mode aperture priority.
    {
        ParamValue value;
        value.set<int32_t>(3);
        EXPECT_TRUE(cd.set_setting("exp-mode", value));
    }

    std::unordered_map<std::string, ParamValue> settings{};
    EXPECT_TRUE(cd.get_possible_settings(settings));
    EXPECT_EQ(settings.size(), 10);

    // With exposure mode manual we will have one more setting.
    {
        ParamValue value;
        value.set<int32_t>(1);
        EXPECT_TRUE(cd.set_setting("exp-mode", value));
    }

    EXPECT_TRUE(cd.get_possible_settings(settings));
    EXPECT_EQ(settings.size(), 11);
}

TEST(CameraDefinition, UVCSetRangeSetting)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(uvc_unit_test_file));

    {
        ParamValue value;
        value.set<int32_t>(200);
        EXPECT_TRUE(cd.set_setting("brightness", value));
    }

    {
        // Try too big.
        ParamValue value;
        value.set<int32_t>(400);
        EXPECT_FALSE(cd.set_setting("brightness", value));
    }

    {
        // Try too small.
        ParamValue value;
        value.set<int32_t>(-100);
        EXPECT_FALSE(cd.set_setting("brightness", value));
    }
}

TEST(CameraDefinition, UVCCheckSettingHumanReadable)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(uvc_unit_test_file));

    std::string description{};
    EXPECT_TRUE(cd.get_setting_str("brightness", description));
    EXPECT_STREQ(description.c_str(), "Brightness");

    EXPECT_TRUE(cd.get_setting_str("wb-mode", description));
    EXPECT_STREQ(description.c_str(), "White Balance Mode");

    EXPECT_TRUE(cd.get_setting_str("exp-priority", description));
    EXPECT_STREQ(description.c_str(), "Exposure Auto Priority");
}

TEST(CameraDefinition, UVCCheckOptionHumanReadable)
{
    // Run this from root.
    CameraDefinition cd;
    ASSERT_TRUE(cd.load_file(uvc_unit_test_file));

    std::string description{};
    // TODO: range params are currently a bit weird like that.
    EXPECT_TRUE(cd.get_option_str("brightness", "0", description));
    EXPECT_STREQ(description.c_str(), "min");
    EXPECT_TRUE(cd.get_option_str("brightness", "225", description));
    EXPECT_STREQ(description.c_str(), "max");
    EXPECT_TRUE(cd.get_option_str("brightness", "1", description));
    EXPECT_STREQ(description.c_str(), "step");

    EXPECT_TRUE(cd.get_option_str("wb-mode", "0", description));
    EXPECT_STREQ(description.c_str(), "Manual Mode");

    EXPECT_TRUE(cd.get_option_str("exp-priority", "1", description));
    EXPECT_STREQ(description.c_str(), "ON");
}
