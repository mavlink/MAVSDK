#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "plugins/ftp_server/ftp_server.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

// Wait for camera list to be populated and the definition to be fetched and parsed.
// Returns true if a non-empty list of setting options is available within the timeout.
static bool wait_for_camera_definition(
    Camera& camera, std::chrono::milliseconds timeout_ms, size_t expected_settings)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout_ms;

    // Wait for camera list first
    while (std::chrono::steady_clock::now() < deadline) {
        if (camera.camera_list().cameras.size() > 0) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (camera.camera_list().cameras.empty()) {
        return false;
    }

    const int component_id = camera.camera_list().cameras[0].component_id;

    while (std::chrono::steady_clock::now() < deadline) {
        auto [result, options] = camera.get_possible_setting_options(component_id);
        if (result == Camera::Result::Success && options.size() == expected_settings) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return false;
}

static std::shared_ptr<System>
wait_for_camera_system(Mavsdk& mavsdk_groundstation, std::chrono::seconds timeout)
{
    auto prom = std::promise<std::shared_ptr<System>>();
    auto fut = prom.get_future();
    std::once_flag flag;

    auto handle = mavsdk_groundstation.subscribe_on_new_system([&]() {
        const auto system = mavsdk_groundstation.systems().back();
        if (system->is_connected() && system->has_camera()) {
            std::call_once(flag, [&]() { prom.set_value(system); });
        }
    });

    if (fut.wait_for(timeout) != std::future_status::ready) {
        mavsdk_groundstation.unsubscribe_on_new_system(handle);
        return nullptr;
    }

    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    return fut.get();
}

TEST(SystemTest, CameraDefinitionUncompressed)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_camera{Mavsdk::Configuration{ComponentType::Camera}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_camera.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_camera.server_component()};
    EXPECT_EQ(ftp_server.set_root_dir("src/mavsdk/plugins/camera/"), FtpServer::Result::Success);

    auto camera_server = CameraServer{mavsdk_camera.server_component()};
    CameraServer::Information information{};
    information.vendor_name = "UVC";
    information.model_name = "Logitech C270HD Webcam";
    information.firmware_version = "4.0.0";
    information.definition_file_version = 2;
    information.definition_file_uri = "mavlinkftp://uvc_camera.xml";
    EXPECT_EQ(camera_server.set_information(information), CameraServer::Result::Success);

    auto system = wait_for_camera_system(mavsdk_groundstation, std::chrono::seconds(2));
    ASSERT_NE(system, nullptr);

    auto camera = Camera{system};
    ASSERT_TRUE(wait_for_camera_definition(camera, std::chrono::seconds(10), 11))
        << "Camera definition (uncompressed) was not loaded within timeout";
}

TEST(SystemTest, CameraDefinitionCompressedXz)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_camera{Mavsdk::Configuration{ComponentType::Camera}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17001"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_camera.add_any_connection("udpout://127.0.0.1:17001"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_camera.server_component()};
    EXPECT_EQ(ftp_server.set_root_dir("src/mavsdk/plugins/camera/"), FtpServer::Result::Success);

    auto camera_server = CameraServer{mavsdk_camera.server_component()};
    CameraServer::Information information{};
    information.vendor_name = "UVC";
    information.model_name = "Logitech C270HD Webcam";
    information.firmware_version = "4.0.0";
    information.definition_file_version = 2;
    information.definition_file_uri = "mavlinkftp://uvc_camera.xml.xz";
    EXPECT_EQ(camera_server.set_information(information), CameraServer::Result::Success);

    auto system = wait_for_camera_system(mavsdk_groundstation, std::chrono::seconds(2));
    ASSERT_NE(system, nullptr);

    auto camera = Camera{system};
    ASSERT_TRUE(wait_for_camera_definition(camera, std::chrono::seconds(10), 11))
        << "Camera definition (xz compressed) was not loaded within timeout";
}
