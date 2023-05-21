#include <chrono>
#include <future>
#include <iostream>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>

int main(int argc, const char* argv[])
{
    // we run client plugins to act as the GCS
    // to communicate with the camera server plugins.
    mavsdk::Mavsdk mavsdk;
    mavsdk::Mavsdk::Configuration configuration(
        mavsdk::Mavsdk::Configuration::UsageType::GroundStation);
    mavsdk.set_configuration(configuration);

    auto result = mavsdk.add_any_connection("udp://:14030");
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
    camera.subscribe_information([](mavsdk::Camera::Information info) {
        std::cout << "Camera information:" << std::endl;
        std::cout << info << std::endl;
    });

    auto take_photo_result = camera.take_photo();
    std::cout << "Take phto return : " << take_photo_result << std::endl;

    return 0;
}
