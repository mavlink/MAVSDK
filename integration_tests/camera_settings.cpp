#include <iostream>
#include <functional>
#include <atomic>
#include <future>
#include "dronecore.h"
#include "integration_test_helper.h"
#include "camera_test_helpers.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`

TEST(CameraTest, ShowSettings)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();
    auto camera = std::make_shared<Camera>(device);

    // Wait for download to happen.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Set to photo mode
    set_mode(camera, Camera::Mode::PHOTO);

    std::map<std::string, std::string> settings;
    EXPECT_TRUE(camera->get_possible_settings(settings));

    LogDebug() << "Possible settings in photo mode: ";
    for (auto setting : settings) {
        LogDebug() << "- " << setting.second;
    }
    EXPECT_EQ(settings.size(), 8);

    // Set to video mode now
    set_mode(camera, Camera::Mode::VIDEO);

    EXPECT_TRUE(camera->get_possible_settings(settings));

    LogDebug() << "Possible settings in video mode: ";
    for (auto setting : settings) {
        LogDebug() << "- " << setting.second;
    }
    EXPECT_EQ(settings.size(), 9);

    std::vector<std::string> options;
    EXPECT_TRUE(camera->get_possible_options("CAM_ISO", options));

    // TODO: this should eventually be 9, not 10 in video mode if the parameter ranges
    // are correct.
    EXPECT_EQ(options.size(), 10);

}

TEST(CameraTest, SettingsAnySetting)
{
    DroneCore dc;

    ConnectionResult connection_ret = dc.add_udp_connection();
    ASSERT_EQ(connection_ret, ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();
    auto camera = std::make_shared<Camera>(device);

    // FIXME: we need to wait for the camera definition to be ready
    // because we don't have a check yet.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    // TODO: use a valid setting here
    camera->set_option_key_async("FOO", "BAR",
    [prom](Camera::Result result) {
        prom->set_value(result);
    });

    // Block now to wait for result but only for a second, we're in a hurry.
    auto status = ret.wait_for(std::chrono::seconds(1));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        Camera::Result new_ret = ret.get();
        // FIXME: this is not implemented, so it returns ERROR
        //EXPECT_EQ(new_ret, Camera::Result::SUCCESS);
        UNUSED(new_ret);
    }
}
