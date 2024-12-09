#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "log.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, CameraVideoStreamSettings)
{
    constexpr auto example_rtsp_url = "rtsp://127.0.0.1:8554/live";

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};

    Mavsdk mavsdk_camera{Mavsdk::Configuration{ComponentType::Camera}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_camera.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto camera_server = CameraServer{mavsdk_camera.server_component()};

    CameraServer::Information information{};
    information.vendor_name = "StreamingCameras";
    information.model_name = "River";
    information.firmware_version = "1.0.0";
    information.definition_file_version = 0;
    information.definition_file_uri = "";
    camera_server.set_information(information);

    CameraServer::VideoStreaming video_streaming{};
    video_streaming.rtsp_uri = example_rtsp_url;
    video_streaming.has_rtsp_server = true;

    camera_server.set_video_streaming(video_streaming);

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

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bool got_info = false;
    camera.subscribe_video_stream_info([&](const Camera::VideoStreamUpdate& update) {
        EXPECT_EQ(update.video_stream_info.settings.uri, example_rtsp_url);
        got_info = true;
    });

    // We expect to find one camera.
    EXPECT_EQ(camera.camera_list().cameras.size(), 1);
    for (unsigned i = 0; i < 60; ++i) {
        if (got_info) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    EXPECT_TRUE(got_info);
}
