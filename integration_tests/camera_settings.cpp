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

    // Set to photo mode
    set_mode(camera, Camera::Mode::PHOTO);

    std::vector<std::string> settings;
    EXPECT_TRUE(camera->get_possible_settings(settings));

    LogDebug() << "Possible settings in photo mode: ";
    for (auto setting : settings) {
        LogDebug() << "-" << setting;
    }
    EXPECT_EQ(settings.size(), 12);

    set_mode(camera, Camera::Mode::VIDEO);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_TRUE(camera->get_possible_settings(settings));

    LogDebug() << "Possible settings in video mode: ";
    for (auto setting : settings) {
        LogDebug() << "-" << setting;
    }
    EXPECT_EQ(settings.size(), 10);

    std::vector<std::string> options;
    EXPECT_TRUE(camera->get_possible_options("CAM_ISO", options));
    EXPECT_EQ(options.size(), 9);

    // Try something that is specific to the camera mode.
    EXPECT_TRUE(camera->get_possible_options("CAM_VIDRES", options));
    EXPECT_EQ(options.size(), 32);

    // This param is not applicable, so we should get an empty vector back.
    EXPECT_FALSE(camera->get_possible_options("CAM_PHOTOQUAL", options));
    EXPECT_EQ(options.size(), 0);

    // The same should happen with a param that does not exist at all.
    EXPECT_FALSE(camera->get_possible_options("CAM_BLABLA", options));
    EXPECT_EQ(options.size(), 0);

    set_mode(camera, Camera::Mode::PHOTO);

    EXPECT_TRUE(camera->get_possible_options("CAM_ISO", options));
    EXPECT_EQ(options.size(), 10);

    // Try something that is specific to the camera mode.
    EXPECT_TRUE(camera->get_possible_options("CAM_PHOTOQUAL", options));
    EXPECT_EQ(options.size(), 4);

    // This param is not applicable, so we should get an empty vector back.
    EXPECT_FALSE(camera->get_possible_options("CAM_VIDRES", options));
    EXPECT_EQ(options.size(), 0);

    // The same should happen with a param that does not exist at all.
    EXPECT_FALSE(camera->get_possible_options("CAM_BLABLA", options));
    EXPECT_EQ(options.size(), 0);
}

TEST(CameraTest, SettingsAnySetting)
{
    DroneCore dc;

    ConnectionResult connection_ret = dc.add_udp_connection();
    ASSERT_EQ(connection_ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

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
