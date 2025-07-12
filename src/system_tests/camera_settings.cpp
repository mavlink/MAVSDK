#include "mavsdk.h"
#include "plugins/camera/camera.h"
#include "plugins/camera_server/camera_server.h"
#include "plugins/ftp_server/ftp_server.h"
#include <future>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <plugins/param_server/param_server.h>

using namespace mavsdk;

TEST(SystemTest, CameraSettings)
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

    const auto reset_params = [&]() {
        EXPECT_EQ(param_server.provide_param_int("CAM_MODE", 0), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("BRIGHTNESS", 128), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("CONTRAST", 32), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("SATURATION", 32), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("GAIN", 64), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("SHARPNESS", 24), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("BACKLIGHT", 0), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("POWER_MODE", 0), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("WB_MODE", 1), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("EXP_MODE", 3), ParamServer::Result::Success);
        EXPECT_EQ(
            param_server.provide_param_int("EXP_ABSOLUTE", 166), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("WB_TEMP", 4000), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("EXP_PRIORITY", 1), ParamServer::Result::Success);
    };
    reset_params();

    // Don't use the default for these two.
    EXPECT_EQ(param_server.provide_param_int("WB_MODE", 0), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int("WB_TEMP", 5000), ParamServer::Result::Success);

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

    // Wait for camera list to be populated before proceeding
    for (unsigned i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (camera.camera_list().cameras.size() > 0) {
            break;
        }
    }

    ASSERT_EQ(camera.camera_list().cameras.size(), 1);

    std::pair<Camera::Result, std::vector<Camera::SettingOptions>> possible_setting_options;

    for (size_t i = 0; i < 50; i++) {
        // In some setups it takes longer for the params to propagate.
        // But we want to end the test as early as possible. So we check regularly
        // for a period of time and exit early if the check passes.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        possible_setting_options =
            camera.get_possible_setting_options(camera.camera_list().cameras[0].component_id);
        if (possible_setting_options.first == Camera::Result::Success &&
            possible_setting_options.second.size() == 11) {
            break;
        }
    }

    ASSERT_EQ(possible_setting_options.first, Camera::Result::Success);
    EXPECT_EQ(possible_setting_options.second.size(), 11);

    auto wb_mode = camera.get_setting(
        camera.camera_list().cameras[0].component_id, Camera::Setting{"WB_MODE"});
    EXPECT_EQ(wb_mode.first, Camera::Result::Success);
    EXPECT_EQ(wb_mode.second.option.option_id, "0");

    std::pair<Camera::Result, std::vector<Camera::Setting>> current_setting;

    for (size_t i = 0; i < 50; i++) {
        // In some setups it takes longer for the params to propagate.
        // But we want to end the test as early as possible. So we check regularly
        // for a period of time and exit early if the check passes.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        current_setting = camera.get_current_settings(camera.camera_list().cameras[0].component_id);
        if (current_setting.first == Camera::Result::Success &&
            current_setting.second.size() == 11) {
            break;
        }
    }

    EXPECT_EQ(current_setting.first, Camera::Result::Success);
    EXPECT_EQ(current_setting.second.size(), 11);

    // Now we change a setting which means the WB_TEMP param is now excluded from our options.
    Camera::Setting new_setting{};
    new_setting.setting_id = "WB_MODE";
    new_setting.option = Camera::Option{"1"};
    EXPECT_EQ(
        camera.set_setting(camera.camera_list().cameras[0].component_id, new_setting),
        Camera::Result::Success);

    possible_setting_options =
        camera.get_possible_setting_options(camera.camera_list().cameras[0].component_id);
    EXPECT_EQ(possible_setting_options.second.size(), 10);

    current_setting = camera.get_current_settings(camera.camera_list().cameras[0].component_id);
    EXPECT_EQ(current_setting.first, Camera::Result::Success);
    EXPECT_EQ(current_setting.second.size(), 10);

    camera_server.subscribe_reset_settings([&](uint32_t) {
        reset_params();
        EXPECT_EQ(
            camera_server.respond_reset_settings(CameraServer::CameraFeedback::Ok),
            CameraServer::Result::Success);
    });

    // And reset settings again.
    EXPECT_EQ(
        camera.reset_settings(camera.camera_list().cameras[0].component_id),
        Camera::Result::Success);

    std::pair<Camera::Result, Camera::Setting> wb_temp;

    for (size_t i = 0; i < 10; i++) {
        // In some setups it takes longer for the param changes to propagate.
        // But we want to end the test as early as possible. So we check regularly
        // for a period of time and exit early if the check passes.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        wb_temp = camera.get_setting(
            camera.camera_list().cameras[0].component_id, Camera::Setting{"WB_TEMP"});
        if (wb_temp.first == Camera::Result::Success && wb_temp.second.option.option_id == "4000") {
            break;
        }
    }

    EXPECT_EQ(wb_temp.first, Camera::Result::Success);
    EXPECT_EQ(wb_temp.second.option.option_id, "4000");
}

TEST(SystemTest, CameraSettingsAsync)
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

    const auto reset_params = [&]() {
        EXPECT_EQ(param_server.provide_param_int("CAM_MODE", 0), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("BRIGHTNESS", 128), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("CONTRAST", 32), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("SATURATION", 32), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("GAIN", 64), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("SHARPNESS", 24), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("BACKLIGHT", 0), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("POWER_MODE", 0), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("WB_MODE", 1), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("EXP_MODE", 3), ParamServer::Result::Success);
        EXPECT_EQ(
            param_server.provide_param_int("EXP_ABSOLUTE", 166), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("WB_TEMP", 4000), ParamServer::Result::Success);
        EXPECT_EQ(param_server.provide_param_int("EXP_PRIORITY", 1), ParamServer::Result::Success);
    };
    reset_params();

    auto prom = std::promise<std::shared_ptr<System>>();
    auto fut = prom.get_future();
    std::once_flag flag;

    auto handle = mavsdk_groundstation.subscribe_on_new_system([&]() {
        const auto system = mavsdk_groundstation.systems().back();
        if (system->is_connected() && system->has_camera()) {
            std::call_once(flag, [&]() { prom.set_value(system); });
        }
    });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    auto system = fut.get();

    auto camera = Camera{system};

    // Wait for camera list to be populated before proceeding
    for (unsigned i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (camera.camera_list().cameras.size() > 0) {
            break;
        }
    }

    ASSERT_EQ(camera.camera_list().cameras.size(), 1);

    std::promise<void> prom_found_wb_temp;
    auto fut_found_wb_temp = prom_found_wb_temp.get_future();

    Camera::CurrentSettingsHandle handle_settings =
        camera.subscribe_current_settings([&](const Camera::CurrentSettingsUpdate& update) {
            for (auto& setting : update.current_settings) {
                if (setting.setting_id == "WB_TEMP") {
                    camera.unsubscribe_current_settings(handle_settings);
                    prom_found_wb_temp.set_value();
                }
            }
        });

    // Set white balance mode to manual, so we have the WB_TEMP param available.
    Camera::Setting setting{};
    setting.setting_id = "WB_MODE";
    setting.option.option_id = "0";
    EXPECT_EQ(
        camera.set_setting(camera.camera_list().cameras[0].component_id, setting),
        Camera::Result::Success);

    EXPECT_EQ(fut_found_wb_temp.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    std::promise<void> prom_found_exp_absolute;
    auto fut_found_exp_absolute = prom_found_exp_absolute.get_future();
    camera.subscribe_possible_setting_options(
        [&](const Camera::PossibleSettingOptionsUpdate& update) {
            for (auto& setting_options : update.setting_options) {
                if (setting_options.setting_id == "EXP_ABSOLUTE") {
                    prom_found_exp_absolute.set_value();
                }
            }
        });

    // Set exposure mode to manual to have EXP_ABSOLUTE show up.
    setting.setting_id = "EXP_MODE";
    setting.option.option_id = "1";
    EXPECT_EQ(
        camera.set_setting(camera.camera_list().cameras[0].component_id, setting),
        Camera::Result::Success);

    EXPECT_EQ(fut_found_exp_absolute.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}
