#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "log.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, CameraMode)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_camera{Mavsdk::Configuration{ComponentType::Camera}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_camera.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto camera_server = CameraServer{mavsdk_camera.server_component()};

    CameraServer::Information information{};
    information.vendor_name = "CoolCameras";
    information.model_name = "Frozen Super";
    information.firmware_version = "4.0.0";
    information.definition_file_version = 0;
    information.definition_file_uri = "";
    camera_server.set_information(information);

    // Add proper synchronization
    std::mutex mode_mutex;
    std::condition_variable mode_cv;
    CameraServer::Mode mode{CameraServer::Mode::Unknown};
    bool mode_updated = false;

    auto mode_handle = camera_server.subscribe_set_mode([&](CameraServer::Mode new_mode) {
        LogInfo() << "Set mode to " << new_mode;
        {
            std::lock_guard<std::mutex> lock(mode_mutex);
            mode = new_mode;
            mode_updated = true;
        }
        mode_cv.notify_one();
        camera_server.respond_set_mode(CameraServer::CameraFeedback::Ok);
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

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    // We expect to find one camera.
    ASSERT_EQ(camera.camera_list().cameras.size(), 1);

    // Reset the mode_updated flag
    {
        std::lock_guard<std::mutex> lock(mode_mutex);
        mode_updated = false;
    }

    EXPECT_EQ(
        camera.set_mode(camera.camera_list().cameras[0].component_id, Camera::Mode::Photo),
        Camera::Result::Success);

    // Wait for the mode to be updated with proper synchronization
    {
        std::unique_lock<std::mutex> lock(mode_mutex);
        EXPECT_TRUE(mode_cv.wait_for(lock, std::chrono::seconds(1), [&] { return mode_updated; }));
        EXPECT_EQ(mode, CameraServer::Mode::Photo);
    }

    // Reset the mode_updated flag
    {
        std::lock_guard<std::mutex> lock(mode_mutex);
        mode_updated = false;
    }

    EXPECT_EQ(
        camera.set_mode(camera.camera_list().cameras[0].component_id, Camera::Mode::Video),
        Camera::Result::Success);

    // Wait for the mode to be updated with proper synchronization
    {
        std::unique_lock<std::mutex> lock(mode_mutex);
        EXPECT_TRUE(mode_cv.wait_for(lock, std::chrono::seconds(1), [&] { return mode_updated; }));
        EXPECT_EQ(mode, CameraServer::Mode::Video);
    }

    auto ret = camera.get_mode(camera.camera_list().cameras[0].component_id);
    EXPECT_EQ(ret.first, Camera::Result::Success);
    EXPECT_EQ(ret.second, Camera::Mode::Video);

    camera_server.unsubscribe_set_mode(mode_handle);
}
