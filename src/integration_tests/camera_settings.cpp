#include <algorithm>
#include <iostream>
#include <functional>
#include <atomic>
#include <future>
#include "mavsdk.h"
#include "system.h"
#include "integration_test_helper.h"
#include "camera_test_helpers.h"
#include "unused.h"

using namespace mavsdk;

// To run specific tests for Yuneec cameras.
const static bool is_e90 = false;
const static bool is_e50 = false;
const static bool is_et = false;

void contains_num_options(
    const std::vector<Camera::SettingOptions>& settings, std::string setting_id, unsigned num)
{
    const auto it =
        std::find_if(settings.begin(), settings.end(), [&setting_id](Camera::SettingOptions elem) {
            return elem.setting_id == setting_id;
        });
    const bool found = it != settings.end();
    if (num > 0) {
        EXPECT_TRUE(found);
        if (found) {
            EXPECT_EQ(it->options.size(), num);
        }
    } else {
        EXPECT_FALSE(found);
    }
}

TEST(CameraTest, ShowSettingsAndOptions)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    // Wait for download to happen.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (is_e90 || is_e50 || is_et) {
        // Set to photo mode
        EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        auto settings = camera->possible_setting_options();
        EXPECT_GT(settings.size(), 0);

        LogDebug() << "Possible settings in photo mode: ";
        for (auto setting : settings) {
            LogDebug() << "- " << setting;
        }

        if (is_e90) {
            EXPECT_EQ(settings.size(), 10);
        } else if (is_e50) {
            EXPECT_EQ(settings.size(), 6);
        } else if (is_et) {
            EXPECT_EQ(settings.size(), 5);
        }

        EXPECT_EQ(camera->set_mode(Camera::Mode::Video), Camera::Result::Success);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        settings = camera->possible_setting_options();
        EXPECT_GT(settings.size(), 0);

        LogDebug() << "Possible settings in video mode: ";
        for (auto setting : settings) {
            LogDebug() << "-" << setting;
        }

        if (is_e90) {
            EXPECT_EQ(settings.size(), 8);
        } else if (is_e50) {
            EXPECT_EQ(settings.size(), 5);
        } else if (is_et) {
            EXPECT_EQ(settings.size(), 5);
        }

        if (is_e90) {
            // Try something that is specific to the camera mode.
            contains_num_options(settings, "CAM_VIDRES", 32);
        } else if (is_e50) {
            // Try something that is specific to the camera mode.
            contains_num_options(settings, "CAM_VIDRES", 12);
        }

        if (is_e90) {
            // This param is not applicable, so we should get an empty vector back.
            contains_num_options(settings, "CAM_PHOTOQUAL", 0);
        }

        // The same should happen with a param that does not exist at all.
        contains_num_options(settings, "CAM_BLABLA", 0);

        EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);

        if (is_e90) {
            // Try something that is specific to the camera mode.
            contains_num_options(settings, "CAM_PHOTOQUAL", 4);
        }

        // This param is not applicable, so we should get an empty vector back.
        contains_num_options(settings, "CAM_VIDRES", 0);

        // The same should happen with a param that does not exist at all.
        contains_num_options(settings, "CAM_BLABLA", 0);
    }
}

TEST(CameraTest, SetSettings)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult connection_ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(connection_ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    // We need to wait for the camera definition to be ready
    // because we don't have a check yet.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);

    // Try setting garbage first
    EXPECT_EQ(set_setting(camera, "DOES_NOT", "EXIST"), Camera::Result::Error);

    if (is_e90) {
        std::string value_set;
        EXPECT_EQ(set_setting(camera, "CAM_PHOTOQUAL", "1"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_PHOTOQUAL", value_set), Camera::Result::Success);
        EXPECT_STREQ("1", value_set.c_str());

        EXPECT_EQ(set_setting(camera, "CAM_COLORMODE", "5"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_COLORMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("5", value_set.c_str());

        EXPECT_EQ(set_setting(camera, "CAM_COLORMODE", "1"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_COLORMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("1", value_set.c_str());

        EXPECT_EQ(set_setting(camera, "CAM_COLORMODE", "3"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_COLORMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("3", value_set.c_str());

        // Let's check the manual exposure mode first.
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "1"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("1", value_set.c_str());

        auto settings = camera->possible_setting_options();

        // Try something that is specific to the camera mode.
        contains_num_options(settings, "CAM_SHUTTERSPD", 19);
        contains_num_options(settings, "CAM_ISO", 10);

        // But not EV and metering
        contains_num_options(settings, "CAM_EV", 0);
        contains_num_options(settings, "CAM_METERING", 0);

        // Now we'll try the same for Auto exposure mode
        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "0"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("0", value_set.c_str());

        settings = camera->possible_setting_options();

        // Shutter speed and ISO don't have options in Auto mode.
        contains_num_options(settings, "CAM_SHUTTERSPD", 0);
        contains_num_options(settings, "CAM_ISO", 0);

        // But not EV and metering
        contains_num_options(settings, "CAM_EV", 13);
        contains_num_options(settings, "CAM_METERING", 3);

        EXPECT_EQ(camera->set_mode(Camera::Mode::Video), Camera::Result::Success);

        UNUSED(camera->possible_setting_options());

        // This should fail in video mode.
        EXPECT_EQ(set_setting(camera, "CAM_PHOTOQUAL", "1"), Camera::Result::Error);

        // Let's check the manual exposure mode first.

        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "1"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("1", value_set.c_str());

        // FIXME: otherwise the camera is too slow
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // But a video setting should work
        EXPECT_EQ(set_setting(camera, "CAM_VIDRES", "30"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_VIDRES", value_set), Camera::Result::Success);
        EXPECT_STREQ("30", value_set.c_str());

        settings = camera->possible_setting_options();

        // Shutter speed and ISO don't have options in Auto mode.
        contains_num_options(settings, "CAM_SHUTTERSPD", 15);

        // FIXME: otherwise the camera is too slow
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // But a video setting should work
        EXPECT_EQ(set_setting(camera, "CAM_VIDRES", "0"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_VIDRES", value_set), Camera::Result::Success);
        EXPECT_STREQ("0", value_set.c_str());

        settings = camera->possible_setting_options();

        // Shutter speed and ISO don't have options in Auto mode.
        contains_num_options(settings, "CAM_SHUTTERSPD", 12);

        // Back to auto exposure mode
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "0"), Camera::Result::Success);
        EXPECT_EQ(get_setting(camera, "CAM_EXPMODE", value_set), Camera::Result::Success);
        EXPECT_STREQ("0", value_set.c_str());
    }
}

static void
receive_current_settings(bool& subscription_called, const std::vector<Camera::Setting>& settings)
{
    LogDebug() << "Received current options:";
    EXPECT_TRUE(settings.size() > 0);
    for (auto& setting : settings) {
        LogDebug() << "Got setting '" << setting.setting_description << "' with selected option '"
                   << setting.option.option_description << "'";

        // Check human readable strings too.
        if (setting.setting_id == "CAM_SHUTTERSPD") {
            EXPECT_STREQ(setting.setting_description.c_str(), "Shutter Speed");
        } else if (setting.setting_id == "CAM_EXPMODE") {
            EXPECT_STREQ(setting.setting_description.c_str(), "Exposure Mode");
        }
    }
    subscription_called = true;
}

TEST(CameraTest, SubscribeCurrentSettings)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult connection_ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(connection_ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    // We need to wait for the camera definition to be ready
    // because we don't have a check yet.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Reset exposure mode
    EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "0"), Camera::Result::Success);

    bool subscription_called = false;
    camera->subscribe_current_settings(
        [&subscription_called](const std::vector<Camera::Setting>& settings) {
            receive_current_settings(subscription_called, settings);
        });

    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(subscription_called);

    subscription_called = false;
    EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "1"), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(subscription_called);
}

static void receive_possible_setting_options(
    bool& subscription_called, const std::vector<Camera::SettingOptions>& settings_options)
{
    LogDebug() << "Received possible options:";
    EXPECT_TRUE(settings_options.size() > 0);
    for (auto& setting_options : settings_options) {
        LogDebug() << "Got setting '" << setting_options.setting_description << "' with options:";

        // Check human readable strings too.
        if (setting_options.setting_id == "CAM_SHUTTERSPD") {
            EXPECT_STREQ(setting_options.setting_description.c_str(), "Shutter Speed");
        } else if (setting_options.setting_id == "CAM_EXPMODE") {
            EXPECT_STREQ(setting_options.setting_description.c_str(), "Exposure Mode");
        }

        EXPECT_TRUE(setting_options.options.size() > 0);
        for (auto& option : setting_options.options) {
            LogDebug() << " - '" << option.option_description << "'";
            if (setting_options.setting_id == "CAM_SHUTTERSPD" && option.option_id == "0.0025") {
                EXPECT_STREQ(option.option_description.c_str(), "1/400");
            } else if (setting_options.setting_id == "CAM_WBMODE" && option.option_id == "2") {
                EXPECT_STREQ(option.option_description.c_str(), "Sunrise");
            }
        }
    }
    subscription_called = true;
}

TEST(CameraTest, SubscribePossibleSettings)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult connection_ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(connection_ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    // We need to wait for the camera definition to be ready
    // because we don't have a check yet.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Reset exposure mode
    EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "0"), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    bool subscription_called = false;
    camera->subscribe_possible_setting_options(
        [&subscription_called](const std::vector<Camera::SettingOptions>& possible_settings) {
            receive_possible_setting_options(subscription_called, possible_settings);
        });

    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(subscription_called);

    subscription_called = false;
    EXPECT_EQ(set_setting(camera, "CAM_EXPMODE", "1"), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(subscription_called);
}
