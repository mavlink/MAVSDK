#include "mavsdk.h"
#include "system_tests_helper.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"

using namespace mavsdk;

TEST(SystemTest, TakePhoto)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});

    Mavsdk mavsdk_camera;
    mavsdk_camera.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::Camera});

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(mavsdk_camera.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto camera_server =
        CameraServer{mavsdk_camera.server_component_by_type(Mavsdk::ServerComponentType::Camera)};
    camera_server.subscribe_take_photo([&camera_server](int32_t index) {
        LogInfo() << "Let's take photo " << index;

        CameraServer::CaptureInfo info;
        info.index = index;
        info.is_success = true;

        camera_server.respond_take_photo(CameraServer::TakePhotoFeedback::Ok, info);
    });

    // Wait for systems to connect via heartbeat.
    auto fut = wait_for_first_system_detected(mavsdk_groundstation);
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    auto system = fut.get();

    ASSERT_TRUE(system->has_camera());

    auto camera = Camera{system};

    // We want to take the picture in photo mode.
    // EXPECT_EQ(camera.set_mode(Camera::Mode::Photo), Camera::Result::Success);

    auto received_captured_info_prom = std::promise<void>{};
    auto received_captured_info_fut = received_captured_info_prom.get_future();

    camera.subscribe_capture_info(
        [&camera, &received_captured_info_prom](Camera::CaptureInfo capture_info) {
            LogInfo() << "Received captured info for image: " << capture_info.index;
            // Unsubscribe again to prevent double setting promise.
            camera.subscribe_capture_info(nullptr);
            received_captured_info_prom.set_value();
        });

    EXPECT_EQ(camera.take_photo(), Camera::Result::Success);
    ASSERT_EQ(
        received_captured_info_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    received_captured_info_fut.get();
}
