#include "log.h"
#include "mavsdk.h"
#include <filesystem>
#include <gtest/gtest.h>
#include <chrono>
#include <future>
#include <fstream>
#include <thread>
#include "plugins/ftp/ftp.h"
#include "plugins/ftp_server/ftp_server.h"
#include "fs_helpers.h"
#include "unused.h"

using namespace mavsdk;

static constexpr double reduced_timeout_s = 0.1;

// TODO: make this compatible for Windows using GetTempPath2

static const fs::path temp_dir_provided = "/tmp/mavsdk_systemtest_temp_data/provided";
static const fs::path temp_dir_downloaded = "/tmp/mavsdk_systemtest_temp_data/downloaded";

static const fs::path temp_file = "data.bin";

TEST(SystemTest, FtpDownloadBurstFile)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    // First we try to access the file without the root directory set.
    // We expect that an error as we don't have any permission.
    {
        auto prom = std::promise<Ftp::Result>();
        auto fut = prom.get_future();
        ftp.download_async(
            temp_file.string(),
            temp_dir_downloaded.string(),
            true,
            [&prom](Ftp::Result result, Ftp::ProgressData) { prom.set_value(result); });

        auto future_status = fut.wait_for(std::chrono::seconds(1));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::ProtocolError);
    }

    // Now we set the root dir and expect it to work.
    ftp_server.set_root_dir(temp_dir_provided.string());

    {
        auto prom = std::promise<Ftp::Result>();
        auto fut = prom.get_future();
        ftp.download_async(
            temp_file.string(),
            temp_dir_downloaded.string(),
            true,
            [&prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                if (result != Ftp::Result::Next) {
                    prom.set_value(result);
                } else {
                    LogDebug() << "Download progress: " << progress_data.bytes_transferred << "/"
                               << progress_data.total_bytes << " bytes";
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(1));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::Success);

        EXPECT_TRUE(
            are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, FtpDownloadBurstBigFile)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50000));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    ftp_server.set_root_dir(temp_dir_provided.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    auto prom = std::promise<Ftp::Result>();
    auto fut = prom.get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        true,
        [&prom](Ftp::Result result, Ftp::ProgressData progress_data) {
            if (result != Ftp::Result::Next) {
                prom.set_value(result);
            } else {
                LogDebug() << "Download progress: " << progress_data.bytes_transferred << "/"
                           << progress_data.total_bytes << " bytes";
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(20));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, FtpDownloadBurstBigFileLossy)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 10000));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    unsigned counter = 0;
    auto drop_some = [&counter](mavlink_message_t&) { return counter++ % 5; };

    mavsdk_groundstation.intercept_incoming_messages_async(drop_some);
    mavsdk_groundstation.intercept_outgoing_messages_async(drop_some);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    ftp_server.set_root_dir(temp_dir_provided.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    auto prom = std::promise<Ftp::Result>();
    auto fut = prom.get_future();
    ftp.download_async(
        ("" / temp_file).string(),
        temp_dir_downloaded.string(),
        true,
        [&prom](Ftp::Result result, Ftp::ProgressData progress_data) {
            if (result != Ftp::Result::Next) {
                prom.set_value(result);
            } else {
                LogDebug() << "Download progress: " << progress_data.bytes_transferred << "/"
                           << progress_data.total_bytes << " bytes";
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(30));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    // Before going out of scope, we need to make sure to no longer access the
    // drop_some callback which accesses the local counter variable.
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);
    mavsdk_groundstation.intercept_outgoing_messages_async(nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, FtpDownloadBurstStopAndTryAgain)
{
    constexpr int file_size = 1000;
    constexpr int msg_count = file_size / 255 + 6; // 6 messages for transfer initialization

    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, file_size));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    // Once we received half, we want to stop all traffic.
    int received = 0;
    auto drop_at_some_point_in = [&received, msg_count](mavlink_message_t& message) {
        if (message.msgid == MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            received++;
        }
        if (received >= msg_count / 2) {
            return false;
        }
        return true;
    };

    auto drop_at_some_point_out = [&received, msg_count](mavlink_message_t& message) {
        UNUSED(message);
        if (received >= msg_count / 2) {
            return false;
        }
        return true;
    };

    mavsdk_groundstation.intercept_incoming_messages_async(drop_at_some_point_in);
    mavsdk_groundstation.intercept_outgoing_messages_async(drop_at_some_point_out);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    ftp_server.set_root_dir(temp_dir_provided.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    {
        auto prom = std::promise<Ftp::Result>();
        auto fut = prom.get_future();
        ftp.download_async(
            ("" / temp_file).string(),
            temp_dir_downloaded.string(),
            true,
            [&prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                if (result != Ftp::Result::Next) {
                    prom.set_value(result);
                } else {
                    LogDebug() << "Download progress: " << progress_data.bytes_transferred << "/"
                               << progress_data.total_bytes << " bytes";
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(10));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::Timeout);
    }

    // Before going out of scope, we need to make sure to no longer access the
    // drop_some callback which accesses the local counter variable.
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);
    mavsdk_groundstation.intercept_outgoing_messages_async(nullptr);

    {
        // Now try again
        auto prom = std::promise<Ftp::Result>();
        auto fut = prom.get_future();
        ftp.download_async(
            ("" / temp_file).string(),
            temp_dir_downloaded.string(),
            true,
            [&prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                if (result != Ftp::Result::Next) {
                    prom.set_value(result);
                } else {
                    LogDebug() << "Download progress: " << progress_data.bytes_transferred << "/"
                               << progress_data.total_bytes << " bytes";
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(10));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::Success);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, FtpDownloadBurstFileOutsideOfRoot)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    // Now we set the root dir and expect it to work.
    ftp_server.set_root_dir(temp_dir_provided.string());

    {
        auto prom = std::promise<Ftp::Result>();
        auto fut = prom.get_future();
        ftp.download_async(
            (fs::path("") / ".." / temp_file).string(),
            temp_dir_downloaded.string(),
            true,
            [&prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                UNUSED(progress_data);
                prom.set_value(result);
            });

        auto future_status = fut.wait_for(std::chrono::seconds(1));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::ProtocolError);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
