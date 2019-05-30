#include <iostream>
#include <future>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecode_sdk.h"
#include "system.h"
#include "camera_test_helpers.h"

using namespace dronecode_sdk;

TEST(CameraTest, SetModeSync)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto &system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

    bool received_mode_change = false;
    Camera::Mode last_mode = Camera::Mode::UNKNOWN;

    camera->subscribe_mode([&received_mode_change, &last_mode](Camera::Mode mode) {
        LogInfo() << "Got mode: " << int(mode);
        received_mode_change = true;
        last_mode = mode;
    });

    std::vector<Camera::Mode> modes_to_test;
    modes_to_test.push_back(Camera::Mode::PHOTO);
    modes_to_test.push_back(Camera::Mode::VIDEO);
    modes_to_test.push_back(Camera::Mode::PHOTO);
    modes_to_test.push_back(Camera::Mode::VIDEO);

    for (auto mode : modes_to_test) {
        auto result = camera->set_mode(mode);
        EXPECT_EQ(result, Camera::Result::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        EXPECT_TRUE(received_mode_change);
        EXPECT_EQ(last_mode, mode);
        received_mode_change = false;
    }
}

TEST(CameraTest, SetModeAsync)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

    set_mode_async(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Camera::Mode new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode_async(camera, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode_async(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode_async(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode_async(camera, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode_async(camera, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode_async(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
