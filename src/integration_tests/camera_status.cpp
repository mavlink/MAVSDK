#include "integration_test_helper.h"
#include "mavsdk.h"
#include <iostream>
#include <functional>
#include <atomic>
#include "plugins/camera/camera.h"

using namespace mavsdk;

static void print_camera_status(const Camera::Status& status);

TEST(CameraTest, Status)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_camera());
    auto camera = std::make_shared<Camera>(system);

    bool received_status = false;
    camera->subscribe_status([&received_status](Camera::Status status) {
        received_status = true;
        print_camera_status(status);
    });
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_TRUE(received_status);
}

static void print_camera_status(const Camera::Status& status)
{
    std::string storage_status_str = "";
    switch (status.storage_status) {
        case Camera::Status::StorageStatus::NotAvailable:
            storage_status_str = "not available";
            break;
        case Camera::Status::StorageStatus::Unformatted:
            storage_status_str = "unformatted";
            break;
        case Camera::Status::StorageStatus::Formatted:
            storage_status_str = "formatted";
            break;
        case Camera::Status::StorageStatus::NotSupported:
            storage_status_str = "not supported";
            break;
    }

    LogDebug() << "Status";
    LogDebug() << "------";
    LogDebug() << "Video: " << (status.video_on ? "on" : "off");
    LogDebug() << "Photo interval: " << (status.photo_interval_on ? "on" : "off");
    LogDebug() << "Storage status: " << storage_status_str;
    LogDebug() << "Used: " << status.used_storage_mib / 1024.0f << " GiB";
    LogDebug() << "Available: " << status.available_storage_mib / 1024.0f << " GiB";
    LogDebug() << "Total: " << status.total_storage_mib / 1024.0f << " GiB";
    LogDebug() << "Folder: " << status.media_folder_name;
    LogDebug() << "------";
}
