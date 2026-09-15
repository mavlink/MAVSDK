#include "mavsdk.hpp"
#include "plugins/camera/camera.hpp"
#include "plugins/camera_server/camera_server.hpp"
#include "log.hpp"
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Camera, TakePhoto)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};

    Mavsdk mavsdk_camera{Mavsdk::Configuration{ComponentType::Camera}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_camera.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto camera_server = std::make_shared<CameraServer>(mavsdk_camera.server_component());
    std::weak_ptr<CameraServer> camera_server_weak = camera_server;

    CameraServer::Information information{};
    information.vendor_name = "CoolCameras";
    information.model_name = "Frozen Super";
    information.firmware_version = "4.0.0";
    information.definition_file_version = 1;
    information.definition_file_uri = "";
    camera_server->set_information(information);

    camera_server->subscribe_take_photo([camera_server_weak](int32_t index) {
        auto server = camera_server_weak.lock();
        if (!server) {
            return;
        }

        LogInfo("Let's take photo {}", index);

        CameraServer::CaptureInfo info;
        info.index = index;
        info.is_success = true;

        server->respond_take_photo(CameraServer::CameraFeedback::Ok, info);
    });

    camera_server->subscribe_set_mode([camera_server_weak](CameraServer::Mode mode) {
        auto server = camera_server_weak.lock();
        if (!server) {
            return;
        }

        LogInfo("Set mode to {}", to_string(mode));
        server->respond_set_mode(CameraServer::CameraFeedback::Ok);
    });

    auto prom = std::make_shared<std::promise<std::shared_ptr<System>>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto handle =
        mavsdk_groundstation.subscribe_on_new_system([prom, flag, &mavsdk_groundstation]() {
            const auto system = mavsdk_groundstation.systems().back();
            if (system->is_connected() && system->has_camera()) {
                std::call_once(*flag, [&]() { prom->set_value(system); });
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    auto system = fut.get();

    auto camera = Camera{system};

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    // We expect to find one camera.
    ASSERT_EQ(camera.camera_list().cameras.size(), 1);

    auto received_captured_info_prom = std::make_shared<std::promise<void>>();
    auto received_captured_info_fut = received_captured_info_prom->get_future();
    auto received_captured_info_flag = std::make_shared<std::once_flag>();

    auto capture_handle =
        camera.subscribe_capture_info([received_captured_info_prom, received_captured_info_flag](
                                          Camera::CaptureInfo capture_info) {
            LogInfo("Received captured info for image: {}", capture_info.index);
            std::call_once(
                *received_captured_info_flag, [&]() { received_captured_info_prom->set_value(); });
        });

    EXPECT_EQ(
        camera.take_photo(camera.camera_list().cameras[0].component_id), Camera::Result::Success);
    ASSERT_EQ(
        received_captured_info_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    received_captured_info_fut.get();
    camera.unsubscribe_capture_info(capture_handle);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
