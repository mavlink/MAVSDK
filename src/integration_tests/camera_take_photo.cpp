#include <iostream>
#include <functional>
#include <vector>
#include <atomic>
#include <future>
#include "integration_test_helper.h"
#include "global_include.h"
#include "mavsdk.h"
#include "system.h"
#include "camera_test_helpers.h"

using namespace mavsdk;
using namespace std::placeholders; // for `_1`

static void receive_camera_result(Camera::Result result);

static void receive_capture_info(Camera::CaptureInfo capture_info);

static std::atomic<bool> _received_result{false};
static std::atomic<bool> _received_capture_info{false};

TEST(CameraTest, TakePhotoSingle)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System& system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // We want to take the picture in photo mode.
    set_mode_async(camera, Camera::Mode::Photo);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    camera->capture_info_async(std::bind(&receive_capture_info, _1));

    camera->take_photo_async(std::bind(&receive_camera_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_TRUE(_received_capture_info);
}

TEST(CameraTest, TakePhotosMultiple)
{
    Mavsdk dc;

    const int num_photos_to_take = 3;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System& system = dc.system();
    ASSERT_TRUE(system.has_camera());
    auto camera = std::make_shared<Camera>(system);

    // We want to take the picture in photo mode.
    set_mode_async(camera, Camera::Mode::Photo);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    camera->capture_info_async(std::bind(&receive_capture_info, _1));

    for (unsigned i = 0; i < num_photos_to_take; ++i) {
        camera->take_photo_async(std::bind(&receive_camera_result, _1));
        LogDebug() << "taking picture: " << i;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        EXPECT_TRUE(_received_capture_info);
        _received_capture_info = false;
    }
}

void receive_camera_result(Camera::Result result)
{
    _received_result = true;
    EXPECT_EQ(result, Camera::Result::Success);
}

void receive_capture_info(Camera::CaptureInfo capture_info)
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

    _received_capture_info = true;
}
