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

using namespace mavsdk;

static constexpr double reduced_timeout_s = 0.1;

// TODO: make this compatible for Windows using GetTempPath2

static const fs::path temp_dir_provided = "/tmp/mavsdk_systemtest_temp_data/provided";

static const fs::path temp_file = "data.bin";

TEST(SystemTest, FtpRemoveFile)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));

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

    // First we try to remove the file without the root directory set.
    // We expect an error as we don't have any permission.
    EXPECT_EQ(ftp.remove_file(temp_file.string()), Ftp::Result::ProtocolError);

    // Now we set the root dir and expect it to work.
    ftp_server.set_root_dir(temp_dir_provided.string());

    EXPECT_EQ(ftp.remove_file(temp_file.string()), Ftp::Result::Success);

    EXPECT_FALSE(file_exists(temp_dir_provided / temp_file));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, FtpRemoveFileThatDoesNotExist)
{
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

    // Don't create the file, empty the directory instead.
    ASSERT_TRUE(reset_directories(temp_dir_provided));

    auto ftp = Ftp{system};

    ftp_server.set_root_dir(temp_dir_provided.string());

    EXPECT_EQ(ftp.remove_file(temp_file.string()), Ftp::Result::FileDoesNotExist);

    EXPECT_FALSE(file_exists(temp_dir_provided / temp_file));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, FtpRemoveFileOutsideOfRoot)
{
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

    // Don't create the file, empty the directory instead.
    ASSERT_TRUE(reset_directories(temp_dir_provided));

    auto ftp = Ftp{system};

    ftp_server.set_root_dir(temp_dir_provided.string());

    EXPECT_EQ(ftp.remove_file((fs::path("..") / temp_file).string()), Ftp::Result::ProtocolError);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
