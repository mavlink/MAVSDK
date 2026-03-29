//
// This is an example how to reconnect a connection on error.
//

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <mavsdk/mavsdk.h>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << std::format("Usage : {} <connection_url>\nConnection URL format should be :\n For TCP server: tcpin://<our_ip>:<port>\n For TCP client: tcpout://<remote_ip>:<port>\n For UDP server: udpin://<our_ip>:<port>\n For UDP client: udpout://<remote_ip>:<port>\n For Serial : serial://</path/to/serial/dev>:<baudrate>]\nFor example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n", bin_name);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    // Keep track of our connections
    struct ConnectionEntry {
        std::string url;
        Mavsdk::ConnectionHandle handle;
        bool connected;
    };
    std::mutex connections_mutex;
    std::vector<ConnectionEntry> connections;

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    // Add all connections.
    for (int i = 1; i < argc; ++i) {
        ConnectionEntry entry{};
        entry.url = argv[i];
        entry.connected = false;
        connections.push_back(entry);
    }

    // Subscribe to connection errors.
    // When an error happens, remove the connection and mark it as disconnected.
    mavsdk.subscribe_connection_errors([&](Mavsdk::ConnectionError connection_error) {
        std::cout << std::format("Connection error: {}\n", connection_error.error_description);

        mavsdk.remove_connection(connection_error.connection_handle);

        std::lock_guard lock(connections_mutex);
        auto it = std::find_if(connections.begin(), connections.end(), [&](ConnectionEntry& entry) {
            return entry.handle == connection_error.connection_handle;
        });

        assert(it != connections.end());

        std::cout << std::format("Removed connection: '{}'\n", it->url);
        it->connected = false;
    });

    while (true) {
        {
            // Got through connections and try to add them.
            std::lock_guard lock(connections_mutex);
            for (auto& entry : connections) {
                if (!entry.connected) {
                    std::cout << std::format("Try adding connection '{}'\n", entry.url);
                    auto result = mavsdk.add_any_connection_with_handle(entry.url);

                    if (result.first != ConnectionResult::Success) {
                        std::cout << std::format("Adding connection '{}'failed: {}\n", entry.url, result.first);
                    } else {
                        entry.handle = result.second;
                        entry.connected = true;
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
