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
    information.vendor_name = "CoolCameras";
    information.model_name = "Frozen Super";
    information.firmware_version = "4.0.0";
    information.definition_file_version = 16;
    information.definition_file_uri = "mavlinkftp://e90_unit_test.xml";
    EXPECT_EQ(camera_server.set_information(information), CameraServer::Result::Success);

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

    for (auto setting_option : possible_setting_options.second) {
        std::cout << setting_option.setting_id << "->" << setting_option.setting_description
                  << std::endl;
        for (auto option : setting_option.options) {
            std::cout << "Option: " << option.option_id << "->" << option.option_description
                      << std::endl;
        }
    }
}
