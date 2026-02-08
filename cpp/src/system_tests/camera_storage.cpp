#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "plugins/ftp_server/ftp_server.h"
#include "log.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, CameraStorage)
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
    information.vendor_name = "UVC";
    information.model_name = "Some random camera";
    information.firmware_version = "1.0.0";
    information.definition_file_uri = "";
    EXPECT_EQ(camera_server.set_information(information), CameraServer::Result::Success);

    camera_server.subscribe_storage_information([&](int32_t) {
        CameraServer::StorageInformation storage_info{};
        storage_info.available_storage_mib = 1000.0f;
        storage_info.storage_status = CameraServer::StorageInformation::StorageStatus::Formatted;
        camera_server.respond_storage_information(CameraServer::CameraFeedback::Ok, storage_info);
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

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    auto system = fut.get();

    auto camera = Camera{system};

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    EXPECT_EQ(camera.camera_list().cameras.size(), 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    camera.subscribe_storage([](const Camera::StorageUpdate& update) {
        EXPECT_EQ(update.storage.available_storage_mib, 1000.0f);
        EXPECT_EQ(update.storage.storage_status, Camera::Storage::StorageStatus::Formatted);
    });

    bool got_storage = false;
    for (unsigned i = 0; i < 60; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto ret = camera.get_storage(camera.camera_list().cameras[0].component_id);
        if (ret.first == Camera::Result::Unavailable) {
            continue;
        }
        EXPECT_EQ(ret.first, Camera::Result::Success);
        EXPECT_EQ(ret.second.available_storage_mib, 1000.0f);
        EXPECT_EQ(ret.second.storage_status, Camera::Storage::StorageStatus::Formatted);
        got_storage = true;
    }
    EXPECT_TRUE(got_storage);
}
