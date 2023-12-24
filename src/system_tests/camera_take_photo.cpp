#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "log.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, CameraTakePhoto)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    Mavsdk mavsdk_camera{Mavsdk::Configuration{Mavsdk::ComponentType::Camera}};

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(mavsdk_camera.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto camera_server = CameraServer{mavsdk_camera.server_component()};
    camera_server.subscribe_take_photo([&camera_server](int32_t index) {
        LogInfo() << "Let's take photo " << index;

        CameraServer::CaptureInfo info;
        info.index = index;
        info.is_success = true;

        camera_server.respond_take_photo(CameraServer::CameraFeedback::Ok, info);
    });

    auto prom = std::promise<std::shared_ptr<System>>();
    auto fut = prom.get_future();
    std::once_flag flag;

    auto handle = mavsdk_groundstation.subscribe_on_new_system([&]() {
        const auto system = mavsdk_groundstation.systems().back();
        if (system->is_connected() && system->has_camera()) {
            std::call_once(flag, [&]() { prom.set_value(system); });
        }
    });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    auto system = fut.get();

    auto camera = Camera{system};
    return;

    // We want to take the picture in photo mode.
    // EXPECT_EQ(camera.set_mode(Camera::Mode::Photo), Camera::Result::Success);

    auto received_captured_info_prom = std::promise<void>{};
    auto received_captured_info_fut = received_captured_info_prom.get_future();

    Camera::CaptureInfoHandle capture_handle = camera.subscribe_capture_info(
        [&camera, &received_captured_info_prom, &capture_handle](Camera::CaptureInfo capture_info) {
            LogInfo() << "Received captured info for image: " << capture_info.index;
            // Unsubscribe again to prevent double setting promise.
            camera.unsubscribe_capture_info(capture_handle);
            received_captured_info_prom.set_value();
        });

    EXPECT_EQ(camera.take_photo(), Camera::Result::Success);
    ASSERT_EQ(
        received_captured_info_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    received_captured_info_fut.get();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
