#include "integration_test_helper.h"
#include "mavsdk.h"
#include <iostream>
#include <functional>
#include <atomic>
#include "plugins/camera/camera.h"

using namespace mavsdk;
using namespace std::placeholders; // for `_1`

static void receive_camera_status(const Camera::Status status);
static void print_camera_status(const Camera::Status status);

static std::atomic<bool> _received_status{false};

TEST(CameraTest, Status)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System& system = dc.system();
    auto camera = std::make_shared<Camera>(system);

    camera->status_async(std::bind(&receive_camera_status, _1));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_TRUE(_received_status);
}

static void receive_camera_status(const Camera::Status status)
{
    _received_status = true;
    print_camera_status(status);
}

static void print_camera_status(const Camera::Status status)
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
