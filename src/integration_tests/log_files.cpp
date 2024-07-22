#include <iostream>
#include <future>
#include <fstream>
#include <sstream>

#include "mavsdk.h"
#include "integration_test_helper.h"
#include "plugins/log_files/log_files.h"

using namespace mavsdk;

TEST(HardwareTest, LogFiles)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    // ConnectionResult ret = mavsdk.add_serial_connection("/dev/ttyACM0");
    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto log_files = std::make_shared<LogFiles>(system);

    std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> entry_result =
        log_files->get_entries();

    EXPECT_EQ(entry_result.first, LogFiles::Result::Success);

    if (entry_result.first == LogFiles::Result::Success) {
        unsigned num_downloaded_files = 0;

        for (auto& entry : entry_result.second) {
            float size_mib = entry.size_bytes / 1024.0f / 1024.0f;
            LogInfo() << "Entry " << entry.id << ": "
                      << " at " << entry.date << ", " << size_mib
                      << " MiB, bytes: " << entry.size_bytes;
            std::stringstream file_path_stream;
            file_path_stream << "./logfile_" << entry.id << ".ulog";

            auto prom = std::promise<void>();
            auto fut = prom.get_future();

            log_files->download_log_file_async(
                entry,
                file_path_stream.str(),
                [&prom](LogFiles::Result result, LogFiles::ProgressData progress_data) {
                    if (result == LogFiles::Result::Next) {
                        LogInfo() << "Download progress: " << 100.0f * progress_data.progress;
                    } else {
                        EXPECT_EQ(result, LogFiles::Result::Success);
                        prom.set_value();
                    }
                });

            fut.wait();

            if (++num_downloaded_files >= 2) {
                break;
            }
        }
    }
}

TEST(HardwareTest, LogFilesDownloadFailsIfPathIsDirectory)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    // ConnectionResult ret = mavsdk.add_serial_connection("/dev/ttyACM0");
    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto log_files = std::make_shared<LogFiles>(system);

    std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> entry_result =
        log_files->get_entries();

    EXPECT_EQ(entry_result.first, LogFiles::Result::Success);

    if (entry_result.first == LogFiles::Result::Success) {
        unsigned num_downloaded_files = 0;

        for (auto& entry : entry_result.second) {
            float size_mib = entry.size_bytes / 1024.0f / 1024.0f;
            LogInfo() << "Entry " << entry.id << ": "
                      << " at " << entry.date << ", " << size_mib
                      << " MiB, bytes: " << entry.size_bytes;

            std::stringstream file_path_stream;
            file_path_stream << "./logfile_" << entry.id << ".ulog";

            auto prom = std::promise<void>();
            auto fut = prom.get_future();

            log_files->download_log_file_async(
                entry,
                file_path_stream.str(),
                [&prom](LogFiles::Result result, LogFiles::ProgressData progress_data) {
                    if (result == LogFiles::Result::Next) {
                        LogInfo() << "Download progress: " << 100.0f * progress_data.progress;
                    } else {
                        EXPECT_EQ(result, LogFiles::Result::InvalidArgument);
                        prom.set_value();
                    }
                });

            fut.wait();

            if (++num_downloaded_files >= 2) {
                break;
            }
        }
    }
}

TEST(HardwareTest, LogFilesDownloadFailsIfFileAlreadyExists)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    // ConnectionResult ret = mavsdk.add_serial_connection("/dev/ttyACM0");
    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto log_files = std::make_shared<LogFiles>(system);

    std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> entry_result =
        log_files->get_entries();

    EXPECT_EQ(entry_result.first, LogFiles::Result::Success);

    if (entry_result.first == LogFiles::Result::Success) {
        unsigned num_downloaded_files = 0;

        for (auto& entry : entry_result.second) {
            float size_mib = entry.size_bytes / 1024.0f / 1024.0f;
            LogInfo() << "Entry " << entry.id << ": "
                      << " at " << entry.date << ", " << size_mib
                      << " MiB, bytes: " << entry.size_bytes;
            std::stringstream file_path_stream;
            file_path_stream << "./logfile_" << entry.id << ".ulog";

            std::ofstream ofs(file_path_stream.str());
            ofs << "This file should not be erased by the download\n";
            ofs.close();

            auto prom = std::promise<void>();
            auto fut = prom.get_future();

            log_files->download_log_file_async(
                entry,
                file_path_stream.str(),
                [&prom](LogFiles::Result result, LogFiles::ProgressData progress_data) {
                    if (result == LogFiles::Result::Next) {
                        LogInfo() << "Download progress: " << 100.0f * progress_data.progress;
                    } else {
                        EXPECT_EQ(result, LogFiles::Result::InvalidArgument);
                        prom.set_value();
                    }
                });

            fut.wait();

            if (++num_downloaded_files >= 2) {
                break;
            }
        }
    }
}
