#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "plugins/ftp_server/ftp_server.h"
#include "log.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <plugins/param_server/param_server.h>

using namespace mavsdk;

TEST(SystemTest, CameraDefinition)
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

    auto param_server = ParamServer{mavsdk_camera.server_component()};
    EXPECT_EQ(param_server.provide_param_int("CAM_MODE", 0), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("BRIGHTNESS", 128), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("CONTRAST", 32), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("SATURATION", 32), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("GAIN", 64), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("SHARPNESS", 24), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("BACKLIGHT", 0), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("POWER_MODE", 0), ParamServer::Result::Success);

    // Don't use the default for these two.
    EXPECT_EQ(param_server.provide_param_int("WB_MODE", 0), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("WB_TEMP", 5000), ParamServer::Result::Success);

    EXPECT_EQ(param_server.provide_param_int("EXP_MODE", 3), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("EXP_ABSOLUTE", 166), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("EXP_PRIORITY", 1), ParamServer::Result::Success);

    auto prom = std::promise<std::shared_ptr<System>>();
    auto fut = prom.get_future();
    std::once_flag flag;

    auto handle = mavsdk_groundstation.subscribe_on_new_system([&]() {
        const auto system = mavsdk_groundstation.systems().back();
        if (system->is_connected() && system->has_camera()) {
            std::call_once(flag, [&]() { prom.set_value(system); });
        }
    });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    auto system = fut.get();

    auto camera = Camera{system};

    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_EQ(camera.camera_list().cameras.size(), 1);
    auto possible_setting_options = camera.get_possible_setting_options(1);
    ASSERT_EQ(possible_setting_options.first, Camera::Result::Success);

    auto wb_mode = camera.get_setting(1, Camera::Setting{"WB_MODE"});
    EXPECT_EQ(wb_mode.first, Camera::Result::Success);
    EXPECT_EQ(wb_mode.second.option.option_id, "0");

    EXPECT_EQ(possible_setting_options.second.size(), 11);

    auto current_setting = camera.get_current_settings(1);
    EXPECT_EQ(current_setting.first, Camera::Result::Success);
    EXPECT_EQ(current_setting.second.size(), 11);

    // Now we change a setting which means the WB_TEMP param is now excluded from our options.

    Camera::Setting new_setting{};
    new_setting.setting_id = "WB_MODE";
    new_setting.option = Camera::Option{"1"};
    EXPECT_EQ(camera.set_setting(1, new_setting), Camera::Result::Success);

    possible_setting_options = camera.get_possible_setting_options(1);
    EXPECT_EQ(possible_setting_options.second.size(), 10);

    current_setting = camera.get_current_settings(1);
    EXPECT_EQ(current_setting.first, Camera::Result::Success);
    EXPECT_EQ(current_setting.second.size(), 10);
}
