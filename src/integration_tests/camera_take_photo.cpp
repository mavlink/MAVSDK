#include "integration_test_helper.h"
#include "mavsdk.h"
#include "system.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"

using namespace mavsdk;

static void receive_capture_info(Camera::CaptureInfo capture_info, bool& received_capture_info);

TEST(CameraTest, TakePhotoSingle)
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

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk_groundstation.systems().size(), 1);
    auto system = mavsdk_groundstation.systems().at(0);
    ASSERT_TRUE(system->has_camera());

    auto camera = Camera{system};

    // We want to take the picture in photo mode.
    EXPECT_EQ(camera.set_mode(Camera::Mode::Photo), Camera::Result::Success);

    bool received_capture_info = false;
    camera.subscribe_capture_info([&received_capture_info](Camera::CaptureInfo capture_info) {
        receive_capture_info(capture_info, received_capture_info);
    });

    EXPECT_EQ(camera.take_photo(), Camera::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(received_capture_info);
}

TEST(CameraTest, TakePhotosMultiple)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    const int num_photos_to_take = 3;

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    // We want to take the picture in photo mode.
    EXPECT_EQ(camera->set_mode(Camera::Mode::Photo), Camera::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    bool received_capture_info = false;
    camera->subscribe_capture_info([&received_capture_info](Camera::CaptureInfo capture_info) {
        receive_capture_info(capture_info, received_capture_info);
    });

    for (unsigned i = 0; i < num_photos_to_take; ++i) {
        const auto result = camera->take_photo();
        EXPECT_EQ(result, Camera::Result::Success);
        LogDebug() << "taking picture: " << i;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        EXPECT_TRUE(received_capture_info);
        received_capture_info = false;
    }
}

void receive_capture_info(Camera::CaptureInfo capture_info, bool& received_capture_info)
{
    LogInfo() << "New capture at " << capture_info.position.latitude_deg << ", "
              << capture_info.position.longitude_deg << ", "
              << capture_info.position.absolute_altitude_m << " m, "
              << capture_info.position.relative_altitude_m << " m (relative to home).";
    LogInfo() << "Time: " << capture_info.time_utc_us << " us.";
    LogInfo() << "Attitude: " << capture_info.attitude_quaternion.w << ", "
              << capture_info.attitude_quaternion.x << ", " << capture_info.attitude_quaternion.y
              << ", " << capture_info.attitude_quaternion.z << ".";
    LogInfo() << "Result: " << (capture_info.is_success ? "success" : "fail") << ".";
    LogInfo() << "Saved to " << capture_info.file_url << " (" << capture_info.index << ").";

    received_capture_info = true;
}
