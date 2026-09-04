//
// Example how to implement a UDP connection yourself (Linux/POSIX) and feed
// the bytes into MAVSDK using the raw API:
//
//   - Mavsdk::pass_received_raw_bytes() for bytes coming in from the socket
//   - Mavsdk::subscribe_raw_bytes_to_be_sent() for bytes going out to the socket
//
// This is useful if you need control over the socket that MAVSDK doesn't give
// you, e.g. binding the local (source) port of a UDP "out" connection to a
// fixed port instead of letting the OS pick an ephemeral one.
//
// Usage: raw_udp <remote_ip> <remote_port> [local_port]
//
// Example: talk to PX4 SITL on 127.0.0.1:14550 using local port 13550:
//
//     ./raw_udp 127.0.0.1 14550 13550
//

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

using namespace mavsdk;

namespace {

class UdpLink {
public:
    // Opens a UDP socket bound to local_port (0 means "let the OS pick") and
    // "connected" to remote_ip:remote_port, meaning we only send to, and
    // receive from, that endpoint.
    bool open(const std::string& remote_ip, uint16_t remote_port, uint16_t local_port)
    {
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_fd < 0) {
            std::cerr << "socket failed: " << strerror(errno) << '\n';
            return false;
        }

        // Bind our local (source) port. This is the part that MAVSDK's
        // udpout:// doesn't let you choose right now.
        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_port = htons(local_port);

        if (bind(_fd, reinterpret_cast<sockaddr*>(&local), sizeof(local)) < 0) {
            std::cerr << "bind to port " << local_port << " failed: " << strerror(errno) << '\n';
            close(_fd);
            _fd = -1;
            return false;
        }

        sockaddr_in remote{};
        remote.sin_family = AF_INET;
        remote.sin_port = htons(remote_port);
        if (inet_pton(AF_INET, remote_ip.c_str(), &remote.sin_addr) != 1) {
            std::cerr << "invalid remote IP: " << remote_ip << '\n';
            close(_fd);
            _fd = -1;
            return false;
        }

        // connect() on a UDP socket just fixes the peer, so we can use
        // send()/recv() and get ICMP errors reported back to us.
        if (connect(_fd, reinterpret_cast<sockaddr*>(&remote), sizeof(remote)) < 0) {
            std::cerr << "connect failed: " << strerror(errno) << '\n';
            close(_fd);
            _fd = -1;
            return false;
        }

        return true;
    }

    // Called from the MAVSDK sender thread(s), one MAVLink message at a time.
    void send_bytes(const char* bytes, size_t length)
    {
        std::lock_guard<std::mutex> lock(_send_mutex);
        if (::send(_fd, bytes, length, 0) < 0) {
            // Don't spam: with UDP this typically means nobody is listening yet.
            std::cerr << "send failed: " << strerror(errno) << '\n';
        }
    }

    // Blocking receive loop, to be run in its own thread. Each datagram can
    // contain one or more MAVLink messages, which is fine for
    // pass_received_raw_bytes().
    void receive_loop(Mavsdk& mavsdk, const std::atomic<bool>& should_exit)
    {
        // A timeout so we notice should_exit even without traffic.
        timeval timeout{};
        timeout.tv_sec = 1;
        setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        std::array<char, 2048> buffer{};

        while (!should_exit) {
            const auto received = recv(_fd, buffer.data(), buffer.size(), 0);
            if (received < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                    continue;
                }
                // E.g. ECONNREFUSED if the other side isn't up yet.
                continue;
            }

            // Hand the bytes over to MAVSDK for parsing.
            mavsdk.pass_received_raw_bytes(buffer.data(), static_cast<size_t>(received));
        }
    }

    void close_socket()
    {
        if (_fd >= 0) {
            close(_fd);
            _fd = -1;
        }
    }

private:
    int _fd{-1};
    std::mutex _send_mutex{};
};

} // namespace

int main(int argc, char** argv)
{
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <remote_ip> <remote_port> [local_port]\n";
        return 1;
    }

    const std::string remote_ip = argv[1];
    const auto remote_port = static_cast<uint16_t>(std::stoi(argv[2]));
    const auto local_port = static_cast<uint16_t>(argc == 4 ? std::stoi(argv[3]) : 0);

    UdpLink link;
    if (!link.open(remote_ip, remote_port, local_port)) {
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    // This tells MAVSDK that we do the transport ourselves.
    const auto result = mavsdk.add_any_connection("raw://");
    if (result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << result << '\n';
        return 1;
    }

    // Outgoing: MAVSDK gives us the bytes, we put them on the wire.
    auto handle = mavsdk.subscribe_raw_bytes_to_be_sent(
        [&link](const char* bytes, size_t length) { link.send_bytes(bytes, length); });

    // Incoming: our thread reads the socket and passes bytes to MAVSDK.
    std::atomic<bool> should_exit{false};
    std::thread receive_thread([&]() { link.receive_loop(mavsdk, should_exit); });

    std::cout << "Waiting for system...\n";
    auto maybe_system = mavsdk.first_autopilot(10.0);
    if (!maybe_system) {
        std::cerr << "No autopilot found.\n";
        should_exit = true;
        receive_thread.join();
        link.close_socket();
        return 1;
    }
    auto system = maybe_system.value();
    std::cout << "Connected to system " << static_cast<int>(system->get_system_id()) << '\n';

    // From here on, everything works like with any other connection.
    auto telemetry = Telemetry{system};
    telemetry.subscribe_position([](Telemetry::Position position) {
        std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(30));

    mavsdk.unsubscribe_raw_bytes_to_be_sent(handle);
    should_exit = true;
    receive_thread.join();
    link.close_socket();

    return 0;
}
