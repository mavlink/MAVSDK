#include <iostream>
#include <functional>
#include <vector>
#include <atomic>
#include <future>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"
#include "camera_test_helpers.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`

static void receive_camera_result(Camera::Result result);

static void check_interval_on(std::shared_ptr<Camera> camera, bool on);

static std::atomic<bool> _received_result {false};


TEST(CameraTest, TakePhotoInterval)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();
    auto camera = std::make_shared<Camera>(device);

    // We want to take the pictures in photo mode.
    set_mode(camera, Camera::Mode::PHOTO);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    check_interval_on(camera, false);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    _received_result = false;
    camera->start_photo_interval_async(2.0f, std::bind(&receive_camera_result, _1));

    // Wait for 3 photos
    std::this_thread::sleep_for(std::chrono::seconds(7));

    check_interval_on(camera, true);

    // Wait for another photo
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Then, stop it again.
    camera->stop_photo_interval_async(std::bind(&receive_camera_result, _1));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    check_interval_on(camera, false);

    EXPECT_TRUE(_received_result);
}

void receive_camera_result(Camera::Result result)
{
    _received_result = true;
    EXPECT_EQ(result, Camera::Result::SUCCESS);
}


void check_interval_on(std::shared_ptr<Camera> camera, bool on)
{
    auto prom = std::make_shared<std::promise<void>>();
    auto ret = prom->get_future();

    // Check if status is correct
    camera->get_status_async([prom, on](Camera::Result result, Camera::Status status) {
        EXPECT_EQ(result, Camera::Result::SUCCESS);
        EXPECT_EQ(status.photo_interval_on, on);
        prom->set_value();
    });

    // Block now for a while to wait for result.
    auto status = ret.wait_for(std::chrono::seconds(1));

    EXPECT_EQ(status, std::future_status::ready);
}
