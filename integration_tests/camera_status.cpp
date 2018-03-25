#include "integration_test_helper.h"
#include "dronecore.h"
#include <iostream>
#include <functional>
#include <atomic>
#include "plugins/camera/camera.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`


static void receive_camera_status(Camera::Result result, const Camera::Status &status);

static std::atomic<bool> _received_status {false};



TEST(CameraTest, Status)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();
    auto camera = std::make_shared<Camera>(device);

    camera->get_status_async(std::bind(&receive_camera_status, _1, _2));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_TRUE(_received_status);
}


static void receive_camera_status(Camera::Result result, const Camera::Status &status)
{
    EXPECT_EQ(result, Camera::Result::SUCCESS);
    _received_status = true;

    std::string storage_status_str = "";
    switch (status.storage_status) {
        case Camera::Status::StorageStatus::NOT_AVAILABLE:
            storage_status_str = "not available";
            break;
        case Camera::Status::StorageStatus::UNFORMATTED:
            storage_status_str = "unformatted";
            break;
        case Camera::Status::StorageStatus::FORMATTED:
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
    LogDebug() << "------";
}
