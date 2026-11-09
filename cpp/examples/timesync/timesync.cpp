//
// Example to demonstrate how to synchronize with the autopilot's clock and
// convert autopilot timestamps to local time.
//

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mavsdk/mavsdk.hpp>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

// Converts an autopilot timestamp (e.g. a time_usec field) to local system_clock time.
std::chrono::system_clock::time_point
to_local_time(uint64_t autopilot_time_us, const System::TimesyncStatus& status)
{
    const auto autopilot_time = std::chrono::nanoseconds(autopilot_time_us * 1000);
    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            autopilot_time - std::chrono::nanoseconds(status.offset_ns))};
}

int main(int argc, char** argv)
{
    if (argc != 2) {
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

    auto handle = system.value()->subscribe_timesync_status([](System::TimesyncStatus status) {
        if (!status.synced) {
            std::cout << "Not synchronized\n";
            return;
        }
        std::cout << std::fixed << std::setprecision(3)
                  << "Offset to autopilot clock: " << status.offset_ns / 1e9
                  << " s, round trip time: " << status.rtt_ns / 1e6 << " ms\n";
    });

    // Time synchronization is off by default. A TIMESYNC request is sent every 5 seconds.
    system.value()->enable_timesync();

    for (unsigned i = 0; i < 4; ++i) {
        sleep_for(seconds(5));

        const auto status = system.value()->timesync_status();
        if (!status.synced) {
            continue;
        }

        // A timestamp from the autopilot, 10 seconds after it booted.
        const uint64_t autopilot_time_us = 10'000'000;
        const auto local_time =
            std::chrono::system_clock::to_time_t(to_local_time(autopilot_time_us, status));
        std::cout << "Autopilot time 10 s corresponds to local time "
                  << std::put_time(std::localtime(&local_time), "%F %T") << '\n';
    }

    system.value()->unsubscribe_timesync_status(handle);

    return 0;
}
