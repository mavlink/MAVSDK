//
// Example how to stream ulog from PX4 with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/base64.h>
#include <mavsdk/plugins/log_streaming/log_streaming.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> [--drop]\n"
              << '\n'
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540" << '\n'
              << "--drop   To drop some of the messages" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    // To simulate message drops.
    if (argc == 3 && std::string(argv[2]) == "--drop") {
        std::cout << "Dropping some messages" << std::endl;
        unsigned counter = 0;
        mavsdk.intercept_incoming_messages_async(
            [&](const mavlink_message_t&) { return counter++ % 10 != 0; });
    }

    // Create file to log to.
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert time_t to tm struct
    struct tm buf;
    localtime_r(&in_time_t, &buf);

    // Create a stringstream to hold the filename
    std::stringstream ss;

    // Format the time into the stringstream
    ss << std::put_time(&buf, "%Y-%m-%d_%H-%M-%S") << ".ulg";

    // Convert stringstream to string to use as filename
    std::string filename = ss.str();

    // Open the file
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Could not open file with name '" << ss.str() << "'";
        return 1;
    }

    // Instantiate plugins.
    auto log_streaming = LogStreaming{system.value()};

    size_t bytes_written = 0;
    size_t bytes_written_since_last = 0;
    auto last_time = std::chrono::steady_clock::now();

    log_streaming.subscribe_log_streaming_raw([&](LogStreaming::LogStreamingRaw raw) {
        const auto bytes = mavsdk::base64_decode(raw.data_base64);
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        bytes_written += bytes.size();
        bytes_written_since_last += bytes.size();

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last_time;

        if (elapsed.count() >= 1.0) {
            // More than or equal to one second has passed
            double seconds = elapsed.count();
            double rate_kib_per_second = (bytes_written_since_last / 1024.0) / seconds;

            std::cout << "Wrote " << std::setprecision(3) << bytes_written / 1024.0 / 1024.0
                      << " MiB (" << rate_kib_per_second << " KiB/s)" << std::endl;

            // Reset timer and counter
            last_time = now;
            bytes_written_since_last = 0;
        }
    });

    auto result = log_streaming.start_log_streaming();
    if (result != LogStreaming::Result::Success) {
        std::cerr << "Log streaming start failed." << std::endl;
        return 1;
    }

    for (unsigned i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    result = log_streaming.stop_log_streaming();
    if (result != LogStreaming::Result::Success) {
        std::cerr << "Log streaming stop failed." << std::endl;
        return 1;
    }

    // Give it time to wrap up all logging data
    std::this_thread::sleep_for(std::chrono::seconds(3));

    file.close();

    return 0;
}
