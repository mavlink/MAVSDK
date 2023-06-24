#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera_server/camera_server.h>

static void subscribe_camera_operation(mavsdk::CameraServer& camera_server);

int main(int argc, char** argv)
{
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::Mavsdk::ComponentType::Camera}};

    // 14030 is the default camera port for PX4 SITL
    auto result = mavsdk.add_any_connection("udp://127.0.0.1:14030");
    if (result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Could not establish connection: " << result << std::endl;
        return 1;
    }
    std::cout << "Created camera server connection" << std::endl;

    auto camera_server = mavsdk::CameraServer{mavsdk.server_component()};

    // First add all subscriptions. This defines the camera capabilities.
    subscribe_camera_operation(camera_server);

    // TODO: this state is not guaranteed, e.g. a new system appears
    // while a capture is in progress
    camera_server.set_in_progress(false);

    // Finally call set_information() to "activate" the camera plugin.

    auto ret = camera_server.set_information({
        .vendor_name = "MAVSDK",
        .model_name = "Example Camera Server",
        .firmware_version = "1.0.0",
        .focal_length_mm = 3.0,
        .horizontal_sensor_size_mm = 3.68,
        .vertical_sensor_size_mm = 2.76,
        .horizontal_resolution_px = 3280,
        .vertical_resolution_px = 2464,
        .lens_id = 0,
        .definition_file_version = 0, // TODO: add this
        .definition_file_uri = "", // TODO: implement this using MAVLink FTP
    });

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

        camera_server.respond_take_photo(
            mavsdk::CameraServer::CameraFeedback::Ok,
            mavsdk::CameraServer::CaptureInfo{
                .position = position,
                .attitude_quaternion = attitude,
                .time_utc_us = static_cast<uint64_t>(timestamp),
                .is_success = success,
                .index = index,
                .file_url = {},
            });

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
            capture_status.recording_time_s =
                std::chrono::duration_cast<std::chrono::seconds>(current_time - start_video_time)
                    .count();
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
}