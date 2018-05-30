#include <iostream>
#include <functional>
#include <atomic>
#include <future>
#include "dronecore.h"
#include "system.h"
#include "integration_test_helper.h"
#include "camera_test_helpers.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`

// To run specific tests for Yuneec cameras.
const static bool is_e90 = false;
const static bool is_e50 = false;
const static bool is_et = false;

TEST(CameraTest, ShowSettingsAndOptions)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

    // Wait for download to happen.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (is_e90 || is_e50 || is_et) {
        // Set to photo mode
        set_mode(camera, Camera::Mode::PHOTO);

        std::vector<std::string> settings;
        EXPECT_TRUE(camera->get_possible_settings(settings));

        LogDebug() << "Possible settings in photo mode: ";
        for (auto setting : settings) {
            LogDebug() << "-" << setting;
        }

        if (is_e90) {
            EXPECT_EQ(settings.size(), 9);
        } else if (is_e50) {
            EXPECT_EQ(settings.size(), 6);
        } else if (is_et) {
            EXPECT_EQ(settings.size(), 5);
        }

        set_mode(camera, Camera::Mode::VIDEO);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        EXPECT_TRUE(camera->get_possible_settings(settings));

        LogDebug() << "Possible settings in video mode: ";
        for (auto setting : settings) {
            LogDebug() << "-" << setting;
        }

        if (is_e90) {
            EXPECT_EQ(settings.size(), 6);
        } else if (is_e50) {
            EXPECT_EQ(settings.size(), 5);
        } else if (is_et) {
            EXPECT_EQ(settings.size(), 5);
        }

        std::vector<std::string> options;

        if (is_e90) {
            // Try something that is specific to the camera mode.
            EXPECT_TRUE(camera->get_possible_options("CAM_VIDRES", options));
            EXPECT_EQ(options.size(), 32);
        } else if (is_e50) {
            // Try something that is specific to the camera mode.
            EXPECT_TRUE(camera->get_possible_options("CAM_VIDRES", options));
            EXPECT_EQ(options.size(), 12);
        }

        if (is_e90) {
            // This param is not applicable, so we should get an empty vector back.
            EXPECT_FALSE(camera->get_possible_options("CAM_PHOTOQUAL", options));
            EXPECT_EQ(options.size(), 0);
        }

        // The same should happen with a param that does not exist at all.
        EXPECT_FALSE(camera->get_possible_options("CAM_BLABLA", options));
        EXPECT_EQ(options.size(), 0);

        set_mode(camera, Camera::Mode::PHOTO);

        if (is_e90) {
            // Try something that is specific to the camera mode.
            EXPECT_TRUE(camera->get_possible_options("CAM_PHOTOQUAL", options));
            EXPECT_EQ(options.size(), 4);
        }

        // This param is not applicable, so we should get an empty vector back.
        EXPECT_FALSE(camera->get_possible_options("CAM_VIDRES", options));
        EXPECT_EQ(options.size(), 0);

        // The same should happen with a param that does not exist at all.
        EXPECT_FALSE(camera->get_possible_options("CAM_BLABLA", options));
        EXPECT_EQ(options.size(), 0);
    }
}

TEST(CameraTest, SetSettings)
{
    DroneCore dc;

    ConnectionResult connection_ret = dc.add_udp_connection();
    ASSERT_EQ(connection_ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

    // We need to wait for the camera definition to be ready
    // because we don't have a check yet.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::PHOTO);

    // Try setting garbage first
    EXPECT_EQ(set_setting(camera, "DOES_NOT", "EXIST"), Camera::Result::ERROR);

    if (is_e90) {
        std::string value_set;
        EXPECT_EQ(set_setting(camera, "CAM_PHOTOQUAL", "1"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_PHOTOQUAL", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("1", value_set.c_str());

        EXPECT_EQ(set_setting(camera, "CAM_COLORMODE", "5"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_COLORMODE", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("5", value_set.c_str());

        // Let's check the manual exposure mode first.
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "1"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("1", value_set.c_str());

        // We should now be able to set shutter speed and ISO.
        std::vector<std::string> options;
        // Try something that is specific to the camera mode.
        EXPECT_TRUE(camera->get_possible_options("CAM_SHUTTERSPD", options));
        EXPECT_EQ(options.size(), 19);
        EXPECT_TRUE(camera->get_possible_options("CAM_ISO", options));
        EXPECT_EQ(options.size(), 10);

        // But not EV and metering
        EXPECT_FALSE(camera->get_possible_options("CAM_EV", options));
        EXPECT_EQ(options.size(), 0);
        EXPECT_FALSE(camera->get_possible_options("CAM_METERING", options));
        EXPECT_EQ(options.size(), 0);

        // Now we'll try the same for Auto exposure mode
        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "0"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("0", value_set.c_str());

        // Shutter speed and ISO don't have options in Auto mode.
        EXPECT_FALSE(camera->get_possible_options("CAM_SHUTTERSPD", options));
        EXPECT_EQ(options.size(), 0);
        EXPECT_FALSE(camera->get_possible_options("CAM_ISO", options));
        EXPECT_EQ(options.size(), 0);

        // But not EV and metering
        EXPECT_TRUE(camera->get_possible_options("CAM_EV", options));
        EXPECT_EQ(options.size(), 13);
        EXPECT_TRUE(camera->get_possible_options("CAM_METERING", options));
        EXPECT_EQ(options.size(), 3);

        set_mode(camera, Camera::Mode::VIDEO);

        // This should fail in video mode.
        EXPECT_EQ(set_setting(camera, "CAM_PHOTOQUAL", "1"), Camera::Result::ERROR);

        // Let's check the manual exposure mode first.

        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "1"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("1", value_set.c_str());

        // FIXME: otherwise the camera is too slow
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // But a video setting should work
        EXPECT_EQ(set_setting(camera, "CAM_VIDRES", "30"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_VIDRES", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("30", value_set.c_str());

        // Shutter speed and ISO don't have options in Auto mode.
        EXPECT_TRUE(camera->get_possible_options("CAM_SHUTTERSPD", options));
        EXPECT_EQ(options.size(), 15);

        // FIXME: otherwise the camera is too slow
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // But a video setting should work
        EXPECT_EQ(set_setting(camera, "CAM_VIDRES", "0"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_VIDRES", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("0", value_set.c_str());

        // Shutter speed and ISO don't have options in Auto mode.
        EXPECT_TRUE(camera->get_possible_options("CAM_SHUTTERSPD", options));
        EXPECT_EQ(options.size(), 12);

        // Back to auto exposure mode
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "0"), Camera::Result::SUCCESS);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::SUCCESS);
        EXPECT_STREQ("0", value_set.c_str());

        // Check human readable strings too.
        std::string description;
        EXPECT_FALSE(camera->get_setting_str("BLABLIBLU", description));
        EXPECT_STREQ(description.c_str(), "");

        EXPECT_TRUE(camera->get_setting_str("CAM_EXPMODE", description));
        EXPECT_STREQ(description.c_str(), "Exposure Mode");

        EXPECT_TRUE(camera->get_setting_str("CAM_SHUTTERSPD", description));
        EXPECT_STREQ(description.c_str(), "Shutter Speed");
    }
}
