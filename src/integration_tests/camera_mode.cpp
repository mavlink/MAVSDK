#include <iostream>
#include <future>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "system.h"
#include "camera_test_helpers.h"

using namespace mavsdk;

TEST(CameraTest, SetModeSync)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    bool received_mode_change = false;
    Camera::Mode last_mode = Camera::Mode::Unknown;

    camera->subscribe_mode([&received_mode_change, &last_mode](Camera::Mode mode) {
        LogInfo() << "Got mode: " << int(mode);
        received_mode_change = true;
        last_mode = mode;
    });

    std::vector<Camera::Mode> modes_to_test;
    modes_to_test.push_back(Camera::Mode::Photo);
    modes_to_test.push_back(Camera::Mode::Video);
    modes_to_test.push_back(Camera::Mode::Photo);
    modes_to_test.push_back(Camera::Mode::Video);

    for (auto mode : modes_to_test) {
        auto result = camera->set_mode(mode);
        EXPECT_EQ(result, Camera::Result::Success);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        EXPECT_TRUE(received_mode_change);
        EXPECT_EQ(last_mode, mode);
        received_mode_change = false;
    }
}

TEST(CameraTest, SetModeAsync)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Camera::Mode new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Photo);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Video), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Video);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Photo);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Photo);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Video), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Video);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Video), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Video);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera->set_mode(Camera::Mode::Video), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    new_mode = camera->mode();
    EXPECT_EQ(new_mode, Camera::Mode::Video);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
