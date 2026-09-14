#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <future>
#include <string>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/camera_server/camera_server.hpp>
#include <mavsdk/plugins/ftp_server/ftp_server.hpp>
#include <mavsdk/plugins/param_server/param_server.hpp>

static constexpr const char* kDefinitionFile = "camera_definition.xml";

static void subscribe_camera_operation(mavsdk::CameraServer& camera_server);
static void provide_camera_settings(mavsdk::ParamServer& param_server);

static void usage(const std::string& bin_name)
{
    std::cerr << "Usage: " << bin_name << " [definition dir]\n"
              << '\n'
              << "The definition dir is served over MAVLink FTP and must contain\n"
              << kDefinitionFile << ". It is resolved against the working directory\n"
              << "and defaults to it, so pass the dir explicitly when running the\n"
              << "binary from elsewhere (e.g. " << bin_name << " ../).\n";
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        usage(argv[0]);
        return 1;
    }
    const std::string definition_dir = (argc == 2) ? argv[1] : ".";

    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::Camera}};

    // 14030 is the default camera port for PX4 SITL
    auto result = mavsdk.add_any_connection("udpin://0.0.0.0:14030");
    if (result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Could not establish connection: " << result << std::endl;
        return 1;
    }
    std::cout << "Created camera server connection" << std::endl;

    auto camera_server = mavsdk::CameraServer{mavsdk.server_component()};

    // The camera definition is fetched by the ground station over MAVLink FTP. The root dir is
    // resolved against the working directory, so say where we ended up looking rather than
    // leaving a bare "file doesn't exist" from the FTP server later on.
    const auto definition_path = std::filesystem::absolute(definition_dir) / kDefinitionFile;
    if (!std::filesystem::exists(definition_path)) {
        std::cerr << "Camera definition not found: " << definition_path << '\n';
        usage(argv[0]);
        return 1;
    }

    auto ftp_server = mavsdk::FtpServer{mavsdk.server_component()};
    const auto ftp_result = ftp_server.set_root_dir(definition_dir);
    if (ftp_result != mavsdk::FtpServer::Result::Success) {
        std::cerr << "Could not serve '" << definition_dir << "': " << ftp_result << std::endl;
        return 1;
    }
    std::cout << "Serving camera definition " << definition_path << std::endl;

    // The settings described by the definition file are exchanged as extended parameters
    auto param_server = mavsdk::ParamServer{mavsdk.server_component()};
    provide_camera_settings(param_server);

    // First add all subscriptions. This defines the camera capabilities.
    subscribe_camera_operation(camera_server);

    // TODO: this state is not guaranteed, e.g. a new system appears
    // while a capture is in progress
    camera_server.set_in_progress(false);

    // Finally call set_information() to "activate" the camera plugin.

    mavsdk::CameraServer::Information information{};

    information.vendor_name = "MAVSDK";
    information.model_name = "Example Camera Server";
    information.firmware_version = "1.0.0";
    information.focal_length_mm = 3.0;
    information.horizontal_sensor_size_mm = 3.68f;
    information.vertical_sensor_size_mm = 2.76f;
    information.horizontal_resolution_px = 3280;
    information.vertical_resolution_px = 2464;
    information.lens_id = 0;
    information.definition_file_version = 1;
    // "mftp" is the scheme ground stations use to fetch this over MAVLink FTP
    information.definition_file_uri = std::string("mftp://") + kDefinitionFile;
    information.image_in_video_mode_supported = false;
    information.video_in_image_mode_supported = false;

    auto ret = camera_server.set_information(information);

    if (ret != mavsdk::CameraServer::Result::Success) {
        std::cerr << "Failed to set camera info, exiting" << std::endl;
        return 2;
    }

    // works as a server and never quit
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

// The settings from camera_definition.xml. Keep the names and defaults in sync with it:
// a ground station reads the definition to build its UI, then reads and writes these
// values over the extended parameter protocol.
static void provide_camera_settings(mavsdk::ParamServer& param_server)
{
    param_server.provide_param_int("CAM_MODE", 1); // Video
    param_server.provide_param_int("CAM_EXPMODE", 0); // Auto
    param_server.provide_param_float("CAM_SHUTTERSPD", 0.016666f); // 1/60
    param_server.provide_param_int("CAM_APERTURE", 2); // f/4
    param_server.provide_param_float("CAM_EV", 0.0f);

    // A real camera would apply the setting to the hardware here. The server has already stored
    // the new value, so don't call provide_param_*() again from here - that counts as another
    // change and the subscription would retrigger itself forever.
    param_server.subscribe_changed_param_int([](mavsdk::ParamServer::IntParam param) {
        std::cout << "Camera setting changed: " << param.name << " = " << param.value << std::endl;
    });
    param_server.subscribe_changed_param_float([](mavsdk::ParamServer::FloatParam param) {
        std::cout << "Camera setting changed: " << param.name << " = " << param.value << std::endl;
    });
}

// sample for camera current status
std::chrono::steady_clock::time_point start_video_time;
bool is_recording_video = false;
bool is_capture_in_progress = false;
int32_t image_count = 0;

static void subscribe_camera_operation(mavsdk::CameraServer& camera_server)
{
    camera_server.subscribe_take_photo([&camera_server](int32_t index) {
        camera_server.set_in_progress(true);

        is_capture_in_progress = true;

        std::cout << "taking a picture (" << +index << ")..." << std::endl;

        // TODO : actually capture image here
        // simulating with delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // TODO: populate with telemetry data
        auto position = mavsdk::CameraServer::Position{};
        auto attitude = mavsdk::CameraServer::Quaternion{};

        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();
        auto success = true;
        image_count++;
        camera_server.set_in_progress(false);

        mavsdk::CameraServer::CaptureInfo capture_info{};
        capture_info.position = position;
        capture_info.attitude_quaternion = attitude;
        capture_info.time_utc_us = static_cast<uint64_t>(timestamp);
        capture_info.is_success = success;
        capture_info.index = index;
        capture_info.file_url = "";

        camera_server.respond_take_photo(mavsdk::CameraServer::CameraFeedback::Ok, capture_info);

        is_capture_in_progress = false;
    });

    camera_server.subscribe_start_video([&camera_server](int32_t stream_id) {
        std::cout << "Start video record" << std::endl;
        is_recording_video = true;
        start_video_time = std::chrono::steady_clock::now();
        camera_server.respond_start_video(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_stop_video([&camera_server](int32_t stream_id) {
        std::cout << "Stop video record" << std::endl;
        is_recording_video = false;
        camera_server.respond_stop_video(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_start_video_streaming([&camera_server](int32_t stream_id) {
        std::cout << "Start video streaming " << stream_id << std::endl;
        camera_server.respond_start_video_streaming(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_stop_video_streaming([&camera_server](int32_t stream_id) {
        std::cout << "Stop video streaming " << stream_id << std::endl;
        camera_server.respond_stop_video_streaming(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_set_mode([&camera_server](mavsdk::CameraServer::Mode mode) {
        std::cout << "Set camera mode " << mode << std::endl;
        camera_server.respond_set_mode(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_storage_information([&camera_server](int32_t storage_id) {
        mavsdk::CameraServer::StorageInformation storage_information;
        constexpr int kTotalStorage = 4 * 1024 * 1024;
        storage_information.total_storage_mib = kTotalStorage;
        storage_information.used_storage_mib = 100;
        storage_information.available_storage_mib =
            kTotalStorage - storage_information.used_storage_mib;
        storage_information.storage_status =
            mavsdk::CameraServer::StorageInformation::StorageStatus::Formatted;
        storage_information.storage_type =
            mavsdk::CameraServer::StorageInformation::StorageType::Microsd;

        camera_server.respond_storage_information(
            mavsdk::CameraServer::CameraFeedback::Ok, storage_information);
    });

    camera_server.subscribe_capture_status([&camera_server](int32_t reserved) {
        mavsdk::CameraServer::CaptureStatus capture_status;
        capture_status.image_count = image_count;
        capture_status.image_status =
            is_capture_in_progress ?
                mavsdk::CameraServer::CaptureStatus::ImageStatus::CaptureInProgress :
                mavsdk::CameraServer::CaptureStatus::ImageStatus::Idle;
        capture_status.video_status =
            is_recording_video ?
                mavsdk::CameraServer::CaptureStatus::VideoStatus::CaptureInProgress :
                mavsdk::CameraServer::CaptureStatus::VideoStatus::Idle;
        auto current_time = std::chrono::steady_clock::now();
        if (is_recording_video) {
            capture_status.recording_time_s = static_cast<float>(
                std::chrono::duration_cast<std::chrono::seconds>(current_time - start_video_time)
                    .count());
        }
        camera_server.respond_capture_status(
            mavsdk::CameraServer::CameraFeedback::Ok, capture_status);
    });

    camera_server.subscribe_format_storage([&camera_server](int storage_id) {
        std::cout << "format storage with id : " << storage_id << std::endl;
        camera_server.respond_format_storage(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_reset_settings([&camera_server](int camera_id) {
        std::cout << "reset camera settings" << std::endl;
        camera_server.respond_reset_settings(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_zoom_in_start([&camera_server](int camera_id) {
        std::cout << "Zoom in start" << std::endl;
        camera_server.respond_zoom_in_start(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_zoom_out_start([&camera_server](int camera_id) {
        std::cout << "Zoom out start" << std::endl;
        camera_server.respond_zoom_out_start(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_zoom_stop([&camera_server](int camera_id) {
        std::cout << "Zoom stop" << std::endl;
        camera_server.respond_zoom_stop(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_zoom_range([&camera_server](float zoom_level) {
        std::cout << "Zoom range requested: " << zoom_level << "%" << std::endl;
        camera_server.respond_zoom_range(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_in_step([&camera_server](int camera_id) {
        std::cout << "Focus in step" << std::endl;
        camera_server.respond_focus_in_step(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_out_step([&camera_server](int camera_id) {
        std::cout << "Focus out step" << std::endl;
        camera_server.respond_focus_out_step(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_in_start([&camera_server](int camera_id) {
        std::cout << "Focus in start" << std::endl;
        camera_server.respond_focus_in_start(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_out_start([&camera_server](int camera_id) {
        std::cout << "Focus out start" << std::endl;
        camera_server.respond_focus_out_start(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_stop([&camera_server](int camera_id) {
        std::cout << "Focus stop" << std::endl;
        camera_server.respond_focus_stop(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_range([&camera_server](float focus_level) {
        std::cout << "Focus range requested: " << focus_level << "%" << std::endl;
        camera_server.respond_focus_range(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_meters([&camera_server](float focus_distance_m) {
        std::cout << "Focus meters requested: " << focus_distance_m << "m" << std::endl;
        camera_server.respond_focus_meters(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_auto([&camera_server](int camera_id) {
        std::cout << "Focus auto" << std::endl;
        camera_server.respond_focus_auto(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_auto_single([&camera_server](int camera_id) {
        std::cout << "Focus auto single" << std::endl;
        camera_server.respond_focus_auto_single(mavsdk::CameraServer::CameraFeedback::Ok);
    });

    camera_server.subscribe_focus_auto_continuous([&camera_server](int camera_id) {
        std::cout << "Focus auto continuous" << std::endl;
        camera_server.respond_focus_auto_continuous(mavsdk::CameraServer::CameraFeedback::Ok);
    });
}
