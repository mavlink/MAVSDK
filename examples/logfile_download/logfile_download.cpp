//
// Example how to download logfiles with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/log_files/log_files.h>

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name
              << " <connection_url> [--rm]\n"
              << '\n'
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n"
              << '\n'
              << "To remove log files after all downloads completed,\n"
              << "please add the --rm argument"
              << std::endl;
}

std::shared_ptr<System> get_system(Mavsdk& mavsdk)
{
    std::cerr << "Waiting to discover system...\n";
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
    // system after around 3 seconds max, surely.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found.\n";
        return {};
    }

    // Get discovered system now.
    return fut.get();
}

int main(int argc, char** argv)
{
    if (argc > 3) {
        usage(argv[0]);
        return 1;
    }

    /* parse arguments */
    bool remove_log_files = false;

    for (int i=2; i<argc; ++i) {
        if (argv[i] == "--rm") {
            remove_log_files = true;
	}
    }

    Mavsdk mavsdk;
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    auto system = get_system(mavsdk);
    if (!system) {
        return 1;
    }

    // Instantiate plugins.

    auto log_files = LogFiles{system};

    auto get_entries_result = log_files.get_entries();
    if (get_entries_result.first == LogFiles::Result::Success) {
        bool download_failure = false;
        for(auto entry : get_entries_result.second) {
            std::cerr << "Got log file with ID " << entry.id << " and date " << entry.date << std::endl;
            LogFiles::Result result = log_files.download_log_file(entry, std::string("log-") + entry.date + ".ulg");
            if (result != LogFiles::Result::Success) {
                download_failure = true;
                std::cerr << "LogFiles::download_log_file failed: " << result << std::endl;
            }
        }
        if (!download_failure && remove_log_files) {
            log_files.erase_log_files();
        }
    } else {
        std::cerr << "LogFiles::get_entries failed: " << get_entries_result.first << std::endl;
        return 1;
    }

    return 0;
}
