//
// Example how to download logfiles with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/log_files/log_files.h>

#include <chrono>
#include <functional>
#include <future>
#include <format>
#include <iostream>
#include <cstring>
#include <string>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << std::format(
        "Usage : {} <connection_url> [--rm]\nConnection URL format should be :\n For TCP server: tcpin://<our_ip>:<port>\n For TCP client: tcpout://<remote_ip>:<port>\n For UDP server: udpin://<our_ip>:<port>\n For UDP client: udpout://<remote_ip>:<port>\n For Serial : serial://</path/to/serial/dev>:<baudrate>]\nFor example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n\nTo remove log files after all downloads completed,\nplease add the --rm argument\n",
        bin_name);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    /* parse arguments */
    bool remove_log_files = false;

    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--rm") {
            remove_log_files = true;
        }
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << std::format("Connection failed: {}\n", connection_result);
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    // Instantiate plugins.

    auto log_files = LogFiles{system.value()};

    // Determine file extension based on autopilot type
    std::string file_extension = ".bin"; // Default for ArduPilot and unknown
    if (system.value()->autopilot_type() == Autopilot::Px4) {
        file_extension = ".ulg";
    }

    auto get_entries_result = log_files.get_entries();
    if (get_entries_result.first == LogFiles::Result::Success) {
        bool download_failure = false;
        for (auto entry : get_entries_result.second) {
            std::cerr << std::format("Got log file with ID {} and date {}\n", entry.id, entry.date);

            auto prom = std::promise<LogFiles::Result>{};
            auto future_result = prom.get_future();
            const auto start_time = std::chrono::steady_clock::now();
            log_files.download_log_file_async(
                entry,
                std::string("log-") + entry.date + file_extension,
                [&prom, &entry, start_time](
                    LogFiles::Result result, LogFiles::ProgressData progress) {
                    if (result == LogFiles::Result::Next) {
                        const auto now = std::chrono::steady_clock::now();
                        const double elapsed_s =
                            std::chrono::duration<double>(now - start_time).count();
                        const double downloaded = progress.progress * entry.size_bytes;
                        const double speed = elapsed_s > 0.0 ? downloaded / elapsed_s : 0.0;

                        std::cerr << std::format(
                                         "\r  {:.1f}% ({}/{} KiB)  {:.1f} KiB/s    ",
                                         progress.progress * 100.0,
                                         static_cast<unsigned>(downloaded / 1024),
                                         entry.size_bytes / 1024,
                                         speed / 1024.0)
                                  << std::flush;
                    } else {
                        if (result == LogFiles::Result::Success) {
                            const auto now = std::chrono::steady_clock::now();
                            const double elapsed_s =
                                std::chrono::duration<double>(now - start_time).count();
                            const double speed =
                                elapsed_s > 0.0 ? entry.size_bytes / elapsed_s : 0.0;
                            std::cerr << std::format(
                                "\r  100.0% ({}/{} KiB)  {:.1f} KiB/s  done in {:.1f}s    \n",
                                entry.size_bytes / 1024,
                                entry.size_bytes / 1024,
                                speed / 1024.0,
                                elapsed_s);
                        }
                        prom.set_value(result);
                    }
                });

            auto result = future_result.get();
            if (result != LogFiles::Result::Success) {
                download_failure = true;
                std::cerr << std::format("LogFiles::download_log_file failed: {}\n", result);
            }
        }
        if (!download_failure && remove_log_files) {
            /*
             * If you want to be sure the log has been deleted, call get_entries again
             * that there are no log files present anymore
             *
             * TODO: provide a more reliable solution
             */
            log_files.erase_all_log_files();
        }
    } else {
        std::cerr << std::format("LogFiles::get_entries failed: {}\n", get_entries_result.first);
        return 1;
    }

    return 0;
}
