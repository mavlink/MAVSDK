#include "integration_test_helper.h"
#include "mavsdk.h"
#include <algorithm>
#include <iostream>
#include <functional>
#include <atomic>
#include "plugins/camera/camera.h"

using namespace mavsdk;

TEST(CameraTest, CaptureInfo)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = Camera{system};

    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_EQ(camera.take_photo(), Camera::Result::Success);

    bool received_capture_info = false;
    camera.subscribe_capture_info([&received_capture_info](Camera::CaptureInfo capture_info) {
        received_capture_info = true;
        LogInfo() << capture_info;
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_TRUE(received_capture_info);
}
