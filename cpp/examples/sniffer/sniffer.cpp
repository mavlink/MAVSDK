//
// Example how to intercept and display MAVLink messages in JSON format.
//

#include <mavsdk/mavsdk.h>
#include <iostream>
#include <thread>
#include <set>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <map>
#include <algorithm>

using namespace mavsdk;

enum class DisplayMode {
    List, // Show message names only
    Stat, // Show refreshing message statistics
    All, // Show all messages with JSON fields
    Selective // Show only specified message types
};

void usage(const std::string& bin_name)
{
    std::cerr << "Usage:\n"
              << "  " << bin_name << " <connection_url> list\n"
              << "  " << bin_name << " <connection_url> stat\n"
              << "  " << bin_name << " <connection_url> all\n"
              << "  " << bin_name << " <connection_url> <message_name1> [message_name2] ...\n\n"
              << "Examples:\n"
              << "  " << bin_name << " udpin://0.0.0.0:14540 list\n"
              << "  " << bin_name << " udpin://0.0.0.0:14540 stat\n"
              << "  " << bin_name << " udpin://0.0.0.0:14540 all\n"
              << "  " << bin_name << " udpin://0.0.0.0:14540 GPS_RAW_INT HEARTBEAT\n\n"
              << "Connection URL format:\n"
              << "  TCP server : tcpin://<our_ip>:<port>\n"
              << "  TCP client : tcpout://<remote_ip>:<port>\n"
              << "  UDP server : udpin://<our_ip>:<port>\n"
              << "  UDP client : udpout://<remote_ip>:<port>\n"
              << "  Serial     : serial://</path/to/serial/dev>:<baudrate>\n";
}

std::string get_timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    std::string connection_url = argv[1];
    std::string mode_arg = argv[2];

    DisplayMode mode;
    std::set<std::string> requested_messages;

    if (mode_arg == "list") {
        mode = DisplayMode::List;
    } else if (mode_arg == "stat") {
        mode = DisplayMode::Stat;
    } else if (mode_arg == "all") {
        mode = DisplayMode::All;
    } else {
        mode = DisplayMode::Selective;
        // Add all remaining arguments as message names
        for (int i = 2; i < argc; ++i) {
            requested_messages.insert(argv[i]);
        }
        if (requested_messages.empty()) {
            std::cerr << "Error: No message names specified for selective mode\n";
            usage(argv[0]);
            return 1;
        }
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Connected, waiting for messages..." << std::endl;
    if (mode == DisplayMode::List) {
        std::cout << "Mode: List message names only" << std::endl;
    } else if (mode == DisplayMode::Stat) {
        std::cout << "Mode: Show message statistics (refreshing)" << std::endl;
    } else if (mode == DisplayMode::All) {
        std::cout << "Mode: Show all messages with JSON fields" << std::endl;
    } else {
        std::cout << "Mode: Show messages: ";
        for (const auto& msg : requested_messages) {
            std::cout << msg << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Press Ctrl+C to exit\n" << std::endl;

    // Statistics tracking for stat mode
    std::map<std::string, size_t> message_counts;
    auto start_time = std::chrono::steady_clock::now();

    auto handle = mavsdk.subscribe_incoming_messages_json([&](Mavsdk::MavlinkMessage message) {
        bool should_display = false;

        switch (mode) {
            case DisplayMode::List:
            case DisplayMode::All:
                should_display = true;
                break;
            case DisplayMode::Stat:
                // Track statistics for stat mode
                message_counts[message.message_name]++;
                break;
            case DisplayMode::Selective:
                should_display =
                    requested_messages.find(message.message_name) != requested_messages.end();
                break;
        }

        if (should_display) {
            std::cout << "[" << get_timestamp() << "] ";
            std::cout << "(" << message.system_id << ":" << message.component_id << ") ";
            std::cout << message.message_name;

            if (mode == DisplayMode::All || mode == DisplayMode::Selective) {
                if (!message.fields_json.empty()) {
                    std::cout << ": " << message.fields_json;
                }
            }
            std::cout << std::endl;
        }

        return true; // Always let messages through
    });

    if (mode == DisplayMode::Stat) {
        // For stat mode, refresh the display every second
        auto last_display_update = std::chrono::steady_clock::now();
        int lines_printed = 0;

        while (true) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed =
                std::chrono::duration_cast<std::chrono::seconds>(now - last_display_update);

            if (elapsed.count() >= 1) {
                // Clear previous output (move cursor up and clear lines)
                if (lines_printed > 0) {
                    std::cout << "\033[" << lines_printed << "A"; // Move cursor up
                    for (int i = 0; i < lines_printed; i++) {
                        std::cout << "\033[K\n"; // Clear line and move to next
                    }
                    std::cout << "\033[" << lines_printed << "A"; // Move cursor back up
                }

                // Create sorted vector of message statistics
                std::vector<std::pair<std::string, size_t>> sorted_messages;
                for (const auto& pair : message_counts) {
                    sorted_messages.push_back(pair);
                }
                std::sort(
                    sorted_messages.begin(),
                    sorted_messages.end(),
                    [](const auto& a, const auto& b) { return a.first < b.first; });

                // Print header
                std::cout << std::endl;
                std::cout << std::left << std::setw(35) << "Message Type" << std::setw(10)
                          << "Count" << std::setw(10) << "Hz" << std::endl;
                std::cout << std::string(55, '-') << std::endl;
                lines_printed = 3;

                // Calculate elapsed time since start for Hz calculation
                auto elapsed_since_start =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

                // Print statistics
                for (const auto& pair : sorted_messages) {
                    const std::string& msg_name = pair.first;
                    size_t count = pair.second;
                    double hz =
                        elapsed_since_start > 0 ? (count * 1000.0 / elapsed_since_start) : 0.0;

                    std::cout << std::left << std::setw(35) << msg_name << std::setw(10) << count
                              << std::setw(10) << std::fixed << std::setprecision(1) << hz
                              << std::endl;
                    lines_printed++;
                }

                std::cout << std::flush;
                last_display_update = now;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        // For other modes, just sleep
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    mavsdk.unsubscribe_incoming_messages_json(handle);
    return 0;
}
