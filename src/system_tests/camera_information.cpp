#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "log.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

static constexpr auto vendor_name = "TEST_CAMERA_NAME";
static constexpr auto model_name = "TEST_CAMERA_MODE_NAME";
static constexpr auto firmware_version = "1.2.3.4";
static constexpr auto focal_length_mm = 2.0;
static constexpr auto horizontal_sensor_size_mm = 3.14;
static constexpr auto vertical_sensor_size_mm = 2.55;
static constexpr auto horizontal_resolution_px = 1920;
static constexpr auto vertical_resolution_px = 1080;
static constexpr auto lens_id = 1;
static constexpr auto definition_file_version = 12;
static constexpr auto definition_file_uri = "ftp://192.168.0.1/test.xml";
static std::vector<mavsdk::CameraServer::Information::CameraCapFlags> camera_cap_flags = {
    mavsdk::CameraServer::Information::CameraCapFlags::CaptureImage,
    mavsdk::CameraServer::Information::CameraCapFlags::CaptureVideo,
    mavsdk::CameraServer::Information::CameraCapFlags::HasModes,
    mavsdk::CameraServer::Information::CameraCapFlags::HasVideoStream,
};

TEST(SystemTest, CameraInformation)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    Mavsdk mavsdk_camera{Mavsdk::Configuration{Mavsdk::ComponentType::Camera}};

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(mavsdk_camera.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto camera_server = CameraServer{mavsdk_camera.server_component()};
    auto ret = camera_server.set_information({
        .vendor_name = vendor_name,
        .model_name = model_name,
        .firmware_version = firmware_version,
        .focal_length_mm = focal_length_mm,
        .horizontal_sensor_size_mm = horizontal_sensor_size_mm,
        .vertical_sensor_size_mm = vertical_sensor_size_mm,
        .horizontal_resolution_px = horizontal_resolution_px,
        .vertical_resolution_px = vertical_resolution_px,
        .lens_id = lens_id,
        .definition_file_version = definition_file_version,
        .definition_file_uri = definition_file_uri,
        .camera_cap_flags = camera_cap_flags,
    });
    ASSERT_EQ(ret, CameraServer::Result::Success);

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

    camera.subscribe_information([](mavsdk::Camera::Information info) {
        LogDebug() << info;
        ASSERT_EQ(info.vendor_name, vendor_name);
        ASSERT_EQ(info.model_name, model_name);
        ASSERT_EQ(info.firmware_version, firmware_version);
        ASSERT_FLOAT_EQ(info.focal_length_mm, focal_length_mm);
        ASSERT_FLOAT_EQ(info.horizontal_sensor_size_mm, horizontal_sensor_size_mm);
        ASSERT_FLOAT_EQ(info.vertical_sensor_size_mm, vertical_sensor_size_mm);
        ASSERT_EQ(info.horizontal_resolution_px, horizontal_resolution_px);
        ASSERT_EQ(info.vertical_resolution_px, vertical_resolution_px);
        ASSERT_EQ(info.lens_id, lens_id);
        ASSERT_EQ(info.definition_file_version, definition_file_version);
        ASSERT_EQ(info.definition_file_uri, definition_file_uri);
        ASSERT_EQ(info.camera_cap_flags.size(), camera_cap_flags.size());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
