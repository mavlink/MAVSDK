//
// Example: record incoming MAVLink traffic to a .tlog file.
//
// A .tlog (telemetry log) is the format used by QGroundControl, Mission
// Planner, and ArduPilot.  Each record is simply:
//
//   [8 bytes] big-endian timestamp in microseconds since the Unix epoch
//   [N bytes] raw MAVLink wire packet
//
// The file can be replayed directly in QGC (File > Open Log File) or
// parsed with pymavlink:
//
//   python3 -m pymavlink.tools.mavlogdump output.tlog
//

#include <mavsdk/mavsdk.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <csignal>
#include <mutex>
#include <sstream>
#include <thread>

// mavlink_message_t and mavlink_msg_to_send_buffer() are available because
// <mavsdk/mavsdk.hpp> pulls in the bundled C MAVLink headers transitively
// through mavlink_include.hpp → mavlink/ardupilotmega/mavlink.h.

using namespace mavsdk;

static std::atomic<bool> g_should_exit{false};

void signal_handler(int /*sig*/)
{
    g_should_exit = true;
}

// Write a 64-bit value as big-endian bytes.
static void write_be64(std::ofstream& file, uint64_t value)
{
    std::array<uint8_t, 8> buf{};
    for (int i = 7; i >= 0; --i) {
        buf[static_cast<size_t>(i)] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }
    file.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
}

static std::string default_filename()
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%dT%H-%M-%S") << ".tlog";
    return oss.str();
}

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> [output_file.tlog]\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial    : serial://</path/to/serial/dev>:<baudrate>\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];
    const std::string output_path = (argc == 3) ? argv[2] : default_filename();

    std::ofstream tlog(output_path, std::ios::binary | std::ios::trunc);
    if (!tlog) {
        std::cerr << "Failed to open output file: " << output_path << '\n';
        return 1;
    }
    std::cout << "Recording to: " << output_path << '\n';

    // Counters for summary on exit.
    std::atomic<uint64_t> msg_count{0};
    std::atomic<uint64_t> byte_count{0};

    // We use a mutex so the callback (called from MAVSDK I/O threads) can
    // safely write to the file alongside the main thread's status prints.
    std::mutex file_mutex;

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    // Register the intercept callback *before* adding the connection so we
    // don't miss any early messages (e.g. HEARTBEAT).
    mavsdk.intercept_incoming_messages_async([&](mavlink_message_t& msg) {
        // Serialize to wire format.
        std::array<uint8_t, MAVLINK_MAX_PACKET_LEN> wire{};
        const uint16_t wire_len = mavlink_msg_to_send_buffer(wire.data(), &msg);

        // Timestamp: microseconds since Unix epoch, big-endian.
        const auto now_us =
            static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
                                      std::chrono::system_clock::now().time_since_epoch())
                                      .count());

        {
            std::lock_guard<std::mutex> lock(file_mutex);
            write_be64(tlog, now_us);
            tlog.write(reinterpret_cast<const char*>(wire.data()), wire_len);
        }

        msg_count.fetch_add(1, std::memory_order_relaxed);
        byte_count.fetch_add(static_cast<uint64_t>(wire_len), std::memory_order_relaxed);

        return true; // Never drop messages — we are only observing.
    });

    const ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "Waiting for a system to connect... (Ctrl+C to stop)\n";

    // Print a live status line while recording.
    auto start = std::chrono::steady_clock::now();
    while (!g_should_exit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        const auto elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(
                                   std::chrono::steady_clock::now() - start)
                                   .count();
        const uint64_t msgs = msg_count.load(std::memory_order_relaxed);
        const uint64_t bytes = byte_count.load(std::memory_order_relaxed);

        std::cout << "\r[" << std::setw(6) << elapsed_s << "s]  " << std::setw(8) << msgs
                  << " messages  " << std::setw(8) << bytes << " bytes written   " << std::flush;
    }

    // Flush and close the file cleanly before printing the summary.
    {
        std::lock_guard<std::mutex> lock(file_mutex);
        tlog.flush();
        tlog.close();
    }

    const uint64_t final_msgs = msg_count.load(std::memory_order_relaxed);
    const uint64_t final_bytes = byte_count.load(std::memory_order_relaxed);
    std::cout << "\n\nRecording stopped.\n"
              << "  Messages : " << final_msgs << '\n'
              << "  Bytes    : " << final_bytes << '\n'
              << "  File     : " << output_path << '\n';

    return 0;
}
