#include <chrono>
#include <future>
#include <format>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>

static void do_camera_operation(int32_t component_id, mavsdk::Camera& camera);

int main(int argc, const char* argv[])
{
    // we run client plugins to act as the GCS
    // to communicate with the camera server plugins.
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};

    auto result = mavsdk.add_any_connection("udpout://127.0.0.1:14030");
    if (result == mavsdk::ConnectionResult::Success) {
        std::cout << "Connected!" << std::endl;
    }

    auto prom = std::promise<std::shared_ptr<mavsdk::System>>{};
    auto fut = prom.get_future();
    mavsdk::Mavsdk::NewSystemHandle handle =
        mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
            auto system = mavsdk.systems().back();

            if (system->has_camera()) {
                std::cout << "Discovered camera from Client" << std::endl;

                // Unsubscribe again as we only want to find one system.
                mavsdk.unsubscribe_on_new_system(handle);
                prom.set_value(system);
            } else {
                std::cout << "No MAVSDK found" << std::endl;
            }
        });

    if (fut.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        std::cout << "No camera found, exiting" << std::endl;
        return -1;
    }

    auto system = fut.get();

    auto camera = mavsdk::Camera{system};

    camera.subscribe_camera_list([](mavsdk::Camera::CameraList list) {
        std::cout << "Cameras found:" << std::endl;
        for (auto& camera : list.cameras) {
            std::cout << std::format("{}\n", camera);
        }
    });

    // Wait for camera to be discovered.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // We expect to find one camera.
    if (camera.camera_list().cameras.size() == 0) {
        std::cerr << "No camera found, exiting.\n";
        return 1;
    }

    if (camera.camera_list().cameras.size() > 1) {
        std::cout << "More than one camera found, using first one discovered.\n";
    }

    const auto component_id = camera.camera_list().cameras[0].component_id;

    camera.subscribe_storage([](mavsdk::Camera::StorageUpdate update) {
        std::cout << "Camera storage update:" << std::endl;
        std::cout << std::format("{}\n", update);
    });

    do_camera_operation(component_id, camera);

    // for test subscribe camera status, so don't end the process
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

void do_camera_operation(int32_t component_id, mavsdk::Camera& camera)
{
    // switch to photo mode to take photo
    auto operation_result = camera.set_mode(component_id, mavsdk::Camera::Mode::Photo);
    std::cout << std::format("Set camera to photo mode result : {}\n", operation_result);

    operation_result = camera.take_photo(component_id);
    std::cout << std::format("Take photo result : {}\n", operation_result);

    // switch to video mode to recording video
    operation_result = camera.set_mode(component_id, mavsdk::Camera::Mode::Video);
    std::cout << std::format("Set camera to video mode result : {}\n", operation_result);

    operation_result = camera.start_video(component_id);
    std::cout << std::format("Start video result : {}\n", operation_result);

    operation_result = camera.stop_video(component_id);
    std::cout << std::format("Stop video result : {}\n", operation_result);

    // the camera can have multi video stream so may need the stream id
    operation_result = camera.start_video_streaming(component_id, 0);
    std::cout << std::format("start video streaming result : {}\n", operation_result);

    operation_result = camera.stop_video_streaming(component_id, 0);
    std::cout << std::format("stop video streaming result : {}\n", operation_result);

    // format the storage with special storage id test
    operation_result = camera.format_storage(component_id, 0);
    std::cout << std::format("format storage result : {}\n", operation_result);

    operation_result = camera.reset_settings(component_id);
    std::cout << std::format("Reset camera settings result : {}\n", operation_result);
}
