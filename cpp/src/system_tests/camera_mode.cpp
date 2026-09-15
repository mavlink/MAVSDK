#include "mavsdk.hpp"
#include "plugins/camera/camera.hpp"
#include "plugins/camera_server/camera_server.hpp"
#include "log.hpp"
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Camera, Mode)
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
    information.definition_file_version = 0;
    information.definition_file_uri = "";
    camera_server->set_information(information);

    // Add proper synchronization
    struct ModeState {
        std::mutex mutex;
        std::condition_variable cv;
        CameraServer::Mode mode{CameraServer::Mode::Unknown};
        bool updated = false;
    };
    auto mode_state = std::make_shared<ModeState>();

    auto mode_handle = camera_server->subscribe_set_mode(
        [mode_state, camera_server_weak](CameraServer::Mode new_mode) {
            auto server = camera_server_weak.lock();
            if (!server) {
                return;
            }

            LogInfo("Set mode to {}", to_string(new_mode));
            {
                std::lock_guard<std::mutex> lock(mode_state->mutex);
                mode_state->mode = new_mode;
                mode_state->updated = true;
            }
            mode_state->cv.notify_one();
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

    // Reset the updated flag
    {
        std::lock_guard<std::mutex> lock(mode_state->mutex);
        mode_state->updated = false;
    }

    EXPECT_EQ(
        camera.set_mode(camera.camera_list().cameras[0].component_id, Camera::Mode::Photo),
        Camera::Result::Success);

    // Wait for the mode to be updated with proper synchronization
    {
        std::unique_lock<std::mutex> lock(mode_state->mutex);
        EXPECT_TRUE(mode_state->cv.wait_for(
            lock, std::chrono::seconds(1), [&] { return mode_state->updated; }));
        EXPECT_EQ(mode_state->mode, CameraServer::Mode::Photo);
    }

    // Reset the updated flag
    {
        std::lock_guard<std::mutex> lock(mode_state->mutex);
        mode_state->updated = false;
    }

    EXPECT_EQ(
        camera.set_mode(camera.camera_list().cameras[0].component_id, Camera::Mode::Video),
        Camera::Result::Success);

    // Wait for the mode to be updated with proper synchronization
    {
        std::unique_lock<std::mutex> lock(mode_state->mutex);
        EXPECT_TRUE(mode_state->cv.wait_for(
            lock, std::chrono::seconds(1), [&] { return mode_state->updated; }));
        EXPECT_EQ(mode_state->mode, CameraServer::Mode::Video);
    }

    auto ret = camera.get_mode(camera.camera_list().cameras[0].component_id);
    EXPECT_EQ(ret.first, Camera::Result::Success);
    EXPECT_EQ(ret.second, Camera::Mode::Video);

    camera_server->unsubscribe_set_mode(mode_handle);
}
