//
// Example to demonstrate how to change camera settings.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(std::string bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

struct CurrentSettings {
    mutable std::mutex mutex{};
    std::vector<Camera::Setting> settings;
};

enum class Input { PrintCurrentSettings, ChangeCameraMode, ChangeSetting, Quit };

void show_settings(const CurrentSettings& current_settings)
{
    std::cout << "Current settings:\n";
    {
        std::lock_guard<std::mutex> lock(current_settings.mutex);
        for (const auto& setting : current_settings.settings) {
            std::cout << "  - " << setting.setting_description << ": "
                      << setting.option.option_description << '\n';
        }
    }
}

void change_camera_mode(Camera& camera)
{
    while (true) {
        std::cout << "Possible modes:\n"
                  << "  1: Photo\n"
                  << "  2: Video\n"
                  << "\n"
                  << "  -> ";

        std::string input;
        std::cin >> input;

        if (input == "1") {
            camera.set_mode(Camera::Mode::Photo);
            return;
        } else if (input == "2") {
            camera.set_mode(Camera::Mode::Video);
            return;
        }
    }
}

bool choose_setting(
    Camera& camera, const std::string& setting_id, const std::string& current_option_id)
{
    const auto setting_options = camera.possible_setting_options();

    for (const auto& setting_option : setting_options) {
        if (setting_option.setting_id == setting_id) {
            std::cout << "Options for " << setting_option.setting_description << ":\n";
            unsigned index = 1;
            for (const auto& option : setting_option.options) {
                std::cout << "  " << index++ << ": " << option.option_description
                          << (option.option_id == current_option_id ? " (currently set)" : "")
                          << '\n';
            }

            std::cout << "\n"
                      << "  -> ";

            std::string input;
            std::cin >> input;

            int input_int;
            try {
                input_int = std::stoi(input);
            } catch (std::invalid_argument&) {
                return false;
            }
            // Convert input to be zero based
            --input_int;

            if (input_int < 0 ||
                static_cast<std::size_t>(input_int) >= setting_option.options.size()) {
                return false;
            }

            Camera::Setting setting{};
            setting.setting_id = setting_id;
            setting.option.option_id = setting_option.options[input_int].option_id;

            camera.set_setting(setting);
            return true;
        }
    }

    return false;
}

void change_camera_setting(Camera& camera, const CurrentSettings& current_settings)
{
    while (true) {
        std::cout << "Possible settings:\n";
        // We get copy of current settings to make sure it does not change while
        // we are using it.
        std::vector<Camera::Setting> temp_settings;
        {
            std::lock_guard<std::mutex> lock(current_settings.mutex);
            temp_settings = current_settings.settings;
        }

        unsigned index = 1;
        for (const auto& setting : temp_settings) {
            std::cout << "  " << index++ << ' ' << setting.setting_description << '\n';
        }
        std::cout << "\n"
                  << "  -> ";

        std::string input;
        std::cin >> input;

        int input_int;
        try {
            input_int = std::stoi(input);
        } catch (std::invalid_argument&) {
            continue;
        }

        // Convert input to be zero based
        --input_int;

        if (input_int < 0 || static_cast<std::size_t>(input_int) >= temp_settings.size()) {
            continue;
        }

        if (choose_setting(
                camera,
                temp_settings[input_int].setting_id,
                temp_settings[input_int].option.option_id)) {
            break;
        }
    }
}

Input get_input()
{
    while (true) {
        std::cout << "Usage:\n"
                  << "  p    print current (changeable) camera settings\n"
                  << "  m    change camera mode\n"
                  << "  s    change a setting\n"
                  << "  q    quit this program\n"
                  << "\n"
                  << "  -> ";

        std::string input;
        std::cin >> input;

        if (input == "p") {
            return Input::PrintCurrentSettings;
        } else if (input == "m") {
            return Input::ChangeCameraMode;
        } else if (input == "s") {
            return Input::ChangeSetting;
        } else if (input == "q") {
            return Input::Quit;
        }
    }

    // Should never happen.
    return Input::Quit;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has a
    // camera, we decide to use it.
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        if (system->has_camera()) {
            std::cout << "Discovered camera\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No camera found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
    auto telemetry = Telemetry{system};
    auto camera = Camera{system};

    CurrentSettings current_settings;

    // Subscribe to current settings and cache them.
    camera.subscribe_current_settings([&current_settings](std::vector<Camera::Setting> settings) {
        std::lock_guard<std::mutex> lock(current_settings.mutex);
        current_settings.settings = settings;
    });

    while (true) {
        switch (get_input()) {
            case Input::PrintCurrentSettings:
                show_settings(current_settings);
                break;
            case Input::ChangeCameraMode:
                change_camera_mode(camera);
                break;
            case Input::ChangeSetting:
                change_camera_setting(camera, current_settings);
                break;
            case Input::Quit:
                return 0;
        }
    }

    return 0;
}
