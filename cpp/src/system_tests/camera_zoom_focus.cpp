#include "mavsdk.hpp"
#include "plugins/camera/camera.hpp"
#include "plugins/camera_server/camera_server.hpp"
#include "log.hpp"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Camera, Zoom)
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
    information.vendor_name = "ZoomCameras";
    information.model_name = "Zoom Cam";
    information.firmware_version = "1.2.3";
    information.definition_file_version = 0;
    information.definition_file_uri = "";
    camera_server.set_information(information);

    auto zoom_in_start_handle =
        camera_server.subscribe_zoom_in_start([&camera_server](int32_t index) {
            LogInfo("Zoom in start {}", index);

            camera_server.respond_zoom_in_start(CameraServer::CameraFeedback::Ok);
        });

    auto zoom_out_start_handle =
        camera_server.subscribe_zoom_out_start([&camera_server](int32_t index) {
            LogInfo("Zoom out start {}", index);

            camera_server.respond_zoom_out_start(CameraServer::CameraFeedback::Ok);
        });

    auto zoom_stop_handle = camera_server.subscribe_zoom_stop([&camera_server](int32_t index) {
        LogInfo("Zoom stop {}", index);

        camera_server.respond_zoom_stop(CameraServer::CameraFeedback::Ok);
    });

    auto zoom_range_handle = camera_server.subscribe_zoom_range([&camera_server](float zoom_level) {
        LogInfo("Zoom range requested: {}", zoom_level);

        camera_server.respond_zoom_range(CameraServer::CameraFeedback::Ok);
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

    EXPECT_EQ(
        camera.zoom_in_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.zoom_out_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.zoom_stop(camera.camera_list().cameras[0].component_id), Camera::Result::Success);

    EXPECT_EQ(
        camera.zoom_range(camera.camera_list().cameras[0].component_id, 50.0f),
        Camera::Result::Success);

    camera_server.unsubscribe_zoom_in_start(zoom_in_start_handle);
    EXPECT_EQ(
        camera.zoom_in_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_zoom_out_start(zoom_out_start_handle);
    EXPECT_EQ(
        camera.zoom_out_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_zoom_stop(zoom_stop_handle);
    EXPECT_EQ(
        camera.zoom_stop(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_zoom_range(zoom_range_handle);
    EXPECT_EQ(
        camera.zoom_range(camera.camera_list().cameras[0].component_id, 50.0f),
        Camera::Result::ActionUnsupported);
}

TEST(Camera, Focus)
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
    information.vendor_name = "FocusCameras";
    information.model_name = "Focus Cam";
    information.firmware_version = "3.2.1";
    information.definition_file_version = 0;
    information.definition_file_uri = "";
    camera_server.set_information(information);

    auto focus_in_step_handle =
        camera_server.subscribe_focus_in_step([&camera_server](int32_t index) {
            LogInfo("Focus in step {}", index);

            camera_server.respond_focus_in_step(CameraServer::CameraFeedback::Ok);
        });

    auto focus_out_step_handle =
        camera_server.subscribe_focus_out_step([&camera_server](int32_t index) {
            LogInfo("Focus out step {}", index);

            camera_server.respond_focus_out_step(CameraServer::CameraFeedback::Ok);
        });

    auto focus_in_start_handle =
        camera_server.subscribe_focus_in_start([&camera_server](int32_t index) {
            LogInfo("Focus in start {}", index);

            camera_server.respond_focus_in_start(CameraServer::CameraFeedback::Ok);
        });

    auto focus_out_start_handle =
        camera_server.subscribe_focus_out_start([&camera_server](int32_t index) {
            LogInfo("Focus out start {}", index);

            camera_server.respond_focus_out_start(CameraServer::CameraFeedback::Ok);
        });

    auto focus_stop_handle = camera_server.subscribe_focus_stop([&camera_server](int32_t index) {
        LogInfo("Focus stop {}", index);

        camera_server.respond_focus_stop(CameraServer::CameraFeedback::Ok);
    });

    float last_focus_level = 0;
    auto focus_range_handle =
        camera_server.subscribe_focus_range([&camera_server, &last_focus_level](float focus_level) {
            LogInfo("Focus range requested: {}", focus_level);
            last_focus_level = focus_level;
            camera_server.respond_focus_range(CameraServer::CameraFeedback::Ok);
        });

    float last_focus_distance_m = 0;
    auto focus_meters_handle = camera_server.subscribe_focus_meters(
        [&camera_server, &last_focus_distance_m](float focus_distance_m) {
            LogInfo("Focus meters requested: {}", focus_distance_m);
            last_focus_distance_m = focus_distance_m;
            camera_server.respond_focus_meters(CameraServer::CameraFeedback::Ok);
        });

    auto focus_auto_handle = camera_server.subscribe_focus_auto([&camera_server](int32_t index) {
        LogInfo("Focus auto {}", index);

        camera_server.respond_focus_auto(CameraServer::CameraFeedback::Ok);
    });

    auto focus_auto_single_handle =
        camera_server.subscribe_focus_auto_single([&camera_server](int32_t index) {
            LogInfo("Focus auto single {}", index);

            camera_server.respond_focus_auto_single(CameraServer::CameraFeedback::Ok);
        });

    auto focus_auto_continuous_handle =
        camera_server.subscribe_focus_auto_continuous([&camera_server](int32_t index) {
            LogInfo("Focus auto continuous {}", index);

            camera_server.respond_focus_auto_continuous(CameraServer::CameraFeedback::Ok);
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

    EXPECT_EQ(
        camera.focus_in_step(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_out_step(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_in_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_out_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_stop(camera.camera_list().cameras[0].component_id), Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_range(camera.camera_list().cameras[0].component_id, 50.0f),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_range(camera.camera_list().cameras[0].component_id, 0.0f),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_range(camera.camera_list().cameras[0].component_id, 100.0f),
        Camera::Result::Success);

    // Camera clamps values out of range.
    EXPECT_EQ(
        camera.focus_range(camera.camera_list().cameras[0].component_id, 101.0f),
        Camera::Result::Success);

    EXPECT_EQ(last_focus_level, 100.0f);

    EXPECT_EQ(
        camera.focus_range(camera.camera_list().cameras[0].component_id, -1.0f),
        Camera::Result::Success);

    EXPECT_EQ(last_focus_level, 0.0f);

    EXPECT_EQ(
        camera.focus_meters(camera.camera_list().cameras[0].component_id, 4.2f),
        Camera::Result::Success);

    EXPECT_EQ(last_focus_distance_m, 4.2f);

    // A negative distance makes no sense and is rejected.
    EXPECT_EQ(
        camera.focus_meters(camera.camera_list().cameras[0].component_id, -1.0f),
        Camera::Result::Denied);

    EXPECT_EQ(last_focus_distance_m, 4.2f);

    EXPECT_EQ(
        camera.focus_auto(camera.camera_list().cameras[0].component_id), Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_auto_single(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    EXPECT_EQ(
        camera.focus_auto_continuous(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    camera_server.unsubscribe_focus_in_step(focus_in_step_handle);
    EXPECT_EQ(
        camera.focus_in_step(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_out_step(focus_out_step_handle);
    EXPECT_EQ(
        camera.focus_out_step(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_in_start(focus_in_start_handle);
    EXPECT_EQ(
        camera.focus_in_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_out_start(focus_out_start_handle);
    EXPECT_EQ(
        camera.focus_out_start(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_stop(focus_stop_handle);
    EXPECT_EQ(
        camera.focus_stop(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_range(focus_range_handle);
    EXPECT_EQ(
        camera.focus_range(camera.camera_list().cameras[0].component_id, 50.0f),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_meters(focus_meters_handle);
    EXPECT_EQ(
        camera.focus_meters(camera.camera_list().cameras[0].component_id, 4.2f),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_auto(focus_auto_handle);
    EXPECT_EQ(
        camera.focus_auto(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_auto_single(focus_auto_single_handle);
    EXPECT_EQ(
        camera.focus_auto_single(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
    camera_server.unsubscribe_focus_auto_continuous(focus_auto_continuous_handle);
    EXPECT_EQ(
        camera.focus_auto_continuous(camera.camera_list().cameras[0].component_id),
        Camera::Result::ActionUnsupported);
}