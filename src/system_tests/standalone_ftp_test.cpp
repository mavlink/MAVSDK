#include "log.h"
#include "mavsdk.h"
#include "plugins/ftp/ftp.h"
#include "plugins/ftp_server/ftp_server.h"
#include "fs_helpers.h"
#include <filesystem>
#include <chrono>
#include <thread>
#include <iostream>

using namespace mavsdk;
namespace fs = std::filesystem;

static constexpr double reduced_timeout_s = 0.1;
static const fs::path temp_dir_provided = "/tmp/mavsdk_systemtest_temp_data/provided";
static const fs::path temp_file = "data.bin";

bool test_ftp_remove_file()
{
    std::cout << "Test 1: FtpRemoveFile..." << std::endl;

    if (!create_temp_file(temp_dir_provided / temp_file, 50)) {
        std::cout << "Failed to create temp file" << std::endl;
        return false;
    }

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    if (mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add groundstation connection" << std::endl;
        return false;
    }

    if (mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add autopilot connection" << std::endl;
        return false;
    }

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    if (!maybe_system) {
        std::cout << "No autopilot found" << std::endl;
        return false;
    }
    auto system = maybe_system.value();

    if (!system->has_autopilot()) {
        std::cout << "System does not have autopilot" << std::endl;
        return false;
    }

    auto ftp = Ftp{system};

    // First try to remove the file without the root directory set.
    // We expect an error as we don't have any permission.
    if (ftp.remove_file(temp_file.string()) != Ftp::Result::ProtocolError) {
        std::cout << "Expected ProtocolError without root dir set" << std::endl;
        return false;
    }

    // Now set the root dir and expect it to work.
    ftp_server.set_root_dir(temp_dir_provided.string());

    if (ftp.remove_file(temp_file.string()) != Ftp::Result::Success) {
        std::cout << "Failed to remove file" << std::endl;
        return false;
    }

    if (file_exists(temp_dir_provided / temp_file)) {
        std::cout << "File still exists after removal" << std::endl;
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

bool test_ftp_remove_file_that_does_not_exist()
{
    std::cout << "Test 2: FtpRemoveFileThatDoesNotExist..." << std::endl;

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    if (mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add groundstation connection" << std::endl;
        return false;
    }

    if (mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add autopilot connection" << std::endl;
        return false;
    }

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    if (!maybe_system) {
        std::cout << "No autopilot found" << std::endl;
        return false;
    }
    auto system = maybe_system.value();

    if (!system->has_autopilot()) {
        std::cout << "System does not have autopilot" << std::endl;
        return false;
    }

    // Don't create the file, empty the directory instead.
    if (!reset_directories(temp_dir_provided)) {
        std::cout << "Failed to reset directories" << std::endl;
        return false;
    }

    auto ftp = Ftp{system};

    ftp_server.set_root_dir(temp_dir_provided.string());

    if (ftp.remove_file(temp_file.string()) != Ftp::Result::FileDoesNotExist) {
        std::cout << "Expected FileDoesNotExist for non-existent file" << std::endl;
        return false;
    }

    if (file_exists(temp_dir_provided / temp_file)) {
        std::cout << "File should not exist" << std::endl;
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

bool test_ftp_remove_file_outside_of_root()
{
    std::cout << "Test 3: FtpRemoveFileOutsideOfRoot..." << std::endl;

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    if (mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add groundstation connection" << std::endl;
        return false;
    }

    if (mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add autopilot connection" << std::endl;
        return false;
    }

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    if (!maybe_system) {
        std::cout << "No autopilot found" << std::endl;
        return false;
    }
    auto system = maybe_system.value();

    if (!system->has_autopilot()) {
        std::cout << "System does not have autopilot" << std::endl;
        return false;
    }

    // Don't create the file, empty the directory instead.
    if (!reset_directories(temp_dir_provided)) {
        std::cout << "Failed to reset directories" << std::endl;
        return false;
    }

    auto ftp = Ftp{system};

    ftp_server.set_root_dir(temp_dir_provided.string());

    if (ftp.remove_file((fs::path("..") / temp_file).string()) != Ftp::Result::ProtocolError) {
        std::cout << "Expected ProtocolError for file outside root" << std::endl;
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

int main()
{
    // Use default signal handling to allow core dumps

    std::cout << "Running standalone FTP remove file tests..." << std::endl;

    if (!test_ftp_remove_file()) {
        std::cout << "FtpRemoveFile test failed" << std::endl;
        return 1;
    }

    if (!test_ftp_remove_file_that_does_not_exist()) {
        std::cout << "FtpRemoveFileThatDoesNotExist test failed" << std::endl;
        return 1;
    }

    if (!test_ftp_remove_file_outside_of_root()) {
        std::cout << "FtpRemoveFileOutsideOfRoot test failed" << std::endl;
        return 1;
    }

    std::cout << "All standalone FTP remove file tests completed successfully" << std::endl;
    return 0;
}