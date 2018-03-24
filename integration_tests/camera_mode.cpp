#include <iostream>
#include <future>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"
#include "camera_test_helpers.h"


using namespace dronecore;


TEST(CameraTest, ChangeMode)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();
    auto camera = std::make_shared<Camera>(device);

    set_mode(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Camera::Mode new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::PHOTO_SURVEY);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO_SURVEY);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::PHOTO_SURVEY);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO_SURVEY);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::VIDEO);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    set_mode(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = get_mode(camera);
    EXPECT_EQ(new_mode, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
