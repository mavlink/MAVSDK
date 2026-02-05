//
// Example how to stream logs from PX4 or ArduPilot with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/base64.h>
#include <mavsdk/plugins/log_streaming/log_streaming.h>
#include <mavsdk/plugins/param/param.h>

#include <atomic>
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
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n"
              << "--drop   To drop some of the messages" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
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

    // Wait for autopilot type to be determined (from heartbeat)
    Autopilot autopilot_type = Autopilot::Unknown;
    for (unsigned i = 0; i < 30; ++i) {
        autopilot_type = system.value()->autopilot_type();
        if (autopilot_type != Autopilot::Unknown) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    bool is_ardupilot = autopilot_type == Autopilot::ArduPilot;

    std::cout << "Autopilot type: " << autopilot_type << std::endl;

    // For ArduPilot, check LOG_BACKEND_TYPE parameter
    if (is_ardupilot) {
        auto param = Param{system.value()};
        auto log_backend_result = param.get_param_int("LOG_BACKEND_TYPE");
        if (log_backend_result.first == Param::Result::Success) {
            int log_backend_type = log_backend_result.second;
            // Check if MAVLink backend is enabled (bit 1 = value 2)
            if ((log_backend_type & 2) == 0) {
                std::cerr << "Error: ArduPilot LOG_BACKEND_TYPE=" << log_backend_type
                          << " does not have MAVLink logging enabled (bit 1).\n"
                          << "Set LOG_BACKEND_TYPE to include 2 (e.g., 3 for File+MAVLink)."
                          << std::endl;
                return 1;
            }
            std::cout << "LOG_BACKEND_TYPE=" << log_backend_type << " (MAVLink logging enabled)"
                      << std::endl;
        } else {
            std::cerr << "Warning: Could not read LOG_BACKEND_TYPE parameter" << std::endl;
        }
    }

    // Create file to log to.
    auto now = std::chrono::system_clock::now();
    const auto time_point = std::chrono::system_clock::to_time_t(now);
    struct tm timeinfo;
#ifdef _WIN32
    // Windows version
    localtime_s(&timeinfo, &time_point);
#else
    // POSIX version (Linux, macOS, etc.)
    localtime_r(&time_point, &timeinfo);
#endif
    std::stringstream ss;
    // ArduPilot uses .bin (DataFlash), PX4 uses .ulg (ULog)
    ss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S") << (is_ardupilot ? ".bin" : ".ulg");
    std::string filename = ss.str();
    // Open the file
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Could not open file with name '" << ss.str() << "'";
        return 1;
    }

    // Instantiate plugins.
    auto log_streaming = LogStreaming{system.value()};

    std::atomic<size_t> bytes_written{0};
    std::atomic<size_t> bytes_written_since_last{0};

    log_streaming.subscribe_log_streaming_raw([&](LogStreaming::LogStreamingRaw raw) {
        const auto bytes = mavsdk::base64_decode(raw.data_base64);
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        bytes_written += bytes.size();
        bytes_written_since_last += bytes.size();
    });

    auto result = log_streaming.start_log_streaming();
    if (result != LogStreaming::Result::Success) {
        std::cerr << "Log streaming start failed." << std::endl;
        return 1;
    }

    std::cout << "Log streaming started, writing to " << filename << std::endl;

    // Run for 60 seconds, printing stats every 10 seconds
    for (unsigned i = 0; i < 6; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        double rate_kib_per_second = (bytes_written_since_last / 1024.0) / 10.0;
        std::cout << "Stats: " << std::fixed << std::setprecision(2)
                  << bytes_written / 1024.0 / 1024.0 << " MiB total, " << rate_kib_per_second
                  << " KiB/s" << std::endl;

        bytes_written_since_last = 0;
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
