//
// Example to show the flight modes of a vehicle and to switch between them.
//
// Without a mode name, the available modes are listed. With a mode name, the vehicle is switched
// to that mode.
//

#include <chrono>
#include <future>
#include <iostream>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/modes/modes.hpp>
#include <string>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> [mode name]\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n"
              << "Without a mode name, the available modes are listed.\n";
}

void print_mode(const Modes::Mode& mode)
{
    std::cout << "  " << mode.name;
    if (mode.is_standard) {
        std::cout << " [standard]";
    }
    if (mode.is_advanced) {
        std::cout << " [advanced]";
    }
    if (mode.is_auto_mode) {
        std::cout << " [auto]";
    }
    if (!mode.is_user_selectable) {
        std::cout << " [not user selectable]";
    }
    std::cout << '\n';
}

int main(int argc, char** argv)
{
    if (argc != 2 && argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto modes = Modes{system.value()};

    // The modes are downloaded from the vehicle in the background. Autopilots that don't
    // support the standard modes protocol get a list of the modes known for them.
    std::cout << "Waiting for available modes...\n";
    for (unsigned i = 0; modes.available_modes().empty(); ++i) {
        if (i == 10) {
            std::cerr << "No modes available\n";
            return 1;
        }
        sleep_for(seconds(1));
    }

    if (argc == 2) {
        std::cout << "Available modes:\n";
        unsigned num_hidden = 0;
        for (const auto& mode : modes.available_modes()) {
            // Modes that can't be selected right now, e.g. unregistered external modes.
            if (!mode.is_user_selectable) {
                ++num_hidden;
                continue;
            }
            print_mode(mode);
        }
        if (num_hidden > 0) {
            std::cout << "  (" << num_hidden << " not user selectable)\n";
        }

        // The current mode can arrive after the list of modes.
        for (unsigned i = 0; modes.current_mode().name.empty(); ++i) {
            if (i == 30) {
                std::cerr << "Current mode unknown\n";
                return 1;
            }
            sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Current mode: " << modes.current_mode().name << '\n';
        return 0;
    }

    const std::string name{argv[2]};

    std::promise<void> mode_reached;
    bool reached = false;
    auto handle = modes.subscribe_current_mode([&](Modes::Mode mode) {
        std::cout << "Current mode: " << mode.name << '\n';
        if (mode.name == name && !reached) {
            reached = true;
            mode_reached.set_value();
        }
    });

    std::cout << "Switching to " << name << "...\n";
    const Modes::Result result = modes.set_mode(name);
    if (result != Modes::Result::Success) {
        std::cerr << "Switching to " << name << " failed: " << result << '\n';
        modes.unsubscribe_current_mode(handle);
        return 1;
    }

    if (mode_reached.get_future().wait_for(seconds(3)) != std::future_status::ready) {
        std::cerr << "Vehicle did not report " << name << " as current mode\n";
        modes.unsubscribe_current_mode(handle);
        return 1;
    }

    modes.unsubscribe_current_mode(handle);
    return 0;
}
