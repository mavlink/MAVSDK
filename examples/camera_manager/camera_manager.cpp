//
// Example of a camera manager with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/param_server/param_server.h>

#include <chrono>
#include <future>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <root_dir>\n"
              << '\n'
              << "    Start mavlink camera_manager with camera definition file in <root_dir>" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    Mavsdk::Configuration configuration(Mavsdk::Configuration::UsageType::Camera);
    mavsdk.set_configuration(configuration);
    ConnectionResult connection_result = mavsdk.add_any_connection("udp://:24547");
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Error setting up Mavlink FTP server.\n";
        return 1;
    }

    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();
    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        // Unsubscribe again as we only want to find one system.
        mavsdk.subscribe_on_new_system(nullptr);
        prom.set_value(system);
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 minutes max, surely.
    if (fut.wait_for(seconds(180)) == std::future_status::timeout) {
        std::cerr << "No GCS found.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    auto ftp_server = Ftp{system};
    ftp_server.set_root_directory(argv[1]);

    auto parameter_server = ParamServer{system};
    parameter_server.provide_param_int("CAM_ISO", 1);
    parameter_server.provide_param_float("CAM_EV", -0.3f);

    auto mavlink_passthrough = MavlinkPassthrough{system};
    mavlink_passthrough.subscribe_message_async(
        MAVLINK_MSG_ID_COMMAND_LONG,
	[&](const mavlink_message_t& msg) {
	    mavlink_command_long_t cmd;
            mavlink_msg_command_long_decode(&msg, &cmd);
	    switch (cmd.command) {
	    case MAV_CMD_REQUEST_CAMERA_INFORMATION:
	        mavlink_camera_information_t camera_information;
                camera_information.time_boot_ms = 0;
                strncpy((char*)camera_information.vendor_name, "Foo Industries", 32);
                strncpy((char*)camera_information.model_name, "T100", 32);
                camera_information.firmware_version = 0;
                camera_information.focal_length = 16;
                camera_information.sensor_size_h = 23.2;
                camera_information.sensor_size_v = 15.4;
                camera_information.resolution_h = 5456;
                camera_information.resolution_v = 3632;
                camera_information.flags =                            
                        CAMERA_CAP_FLAGS_CAPTURE_VIDEO |
                        CAMERA_CAP_FLAGS_CAPTURE_IMAGE |
                        CAMERA_CAP_FLAGS_HAS_MODES |
                        CAMERA_CAP_FLAGS_CAN_CAPTURE_IMAGE_IN_VIDEO_MODE |
                        CAMERA_CAP_FLAGS_CAN_CAPTURE_VIDEO_IN_IMAGE_MODE |
                        CAMERA_CAP_FLAGS_HAS_BASIC_ZOOM | 
                        CAMERA_CAP_FLAGS_HAS_BASIC_FOCUS |
                        CAMERA_CAP_FLAGS_HAS_VIDEO_STREAM;
                camera_information.cam_definition_version = 0;
                strncpy(camera_information.cam_definition_uri, "mftp://infos/camera_info.xml", 140);
		std::cout << "Sending CAMERA_INFORMATION" << std::endl;
		mavlink_message_t send_msg;
		mavlink_msg_camera_information_encode(mavlink_passthrough.get_our_sysid(), mavlink_passthrough.get_our_compid(), &send_msg, &camera_information);
		mavlink_passthrough.send_message(send_msg);
	        break;
	    };
	}
    );

    std::cout << "Mavlink FTP server running.\n"
              << '\n'
              << "Component ID: " << static_cast<int>(ftp_server.get_our_compid()) << '\n';

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
