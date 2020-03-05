#include <cstring>
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <atomic>
#include <future>

#include "integration_test_helper.h"
#include "global_include.h"
#include "mavsdk.h"
#include "system.h"
#include "plugins/mavlink_ftp/mavlink_ftp.h"

#ifdef ENABLE_MAVLINK_PASSTHROUGH
#include <random>
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
static std::default_random_engine random_engine;
static std::uniform_real_distribution<> distribution(0.0, 1.0);
#endif

using namespace mavsdk;

static std::atomic<uint32_t> _bytes_transferred{0};
static std::atomic<uint32_t> _file_size{0};

void reset_server(std::shared_ptr<MavlinkFTP>& mavlink_ftp)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    mavlink_ftp->reset_async([prom](MavlinkFTP::Result result) { prom->set_value(result); });
    future_result.get();
}

void create_test_file(std::string file_name, uint32_t size)
{
    std::string str(size, 'X');
    std::ofstream of(file_name, std::fstream::trunc | std::fstream::binary);
    of << str;
    of.close();
}

void test_create_directory(std::shared_ptr<MavlinkFTP> mavlink_ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    mavlink_ftp->create_directory_async(
        path, [prom](MavlinkFTP::Result result) { prom->set_value(result); });

    MavlinkFTP::Result result = future_result.get();
    EXPECT_TRUE(
        result == MavlinkFTP::Result::SUCCESS || result == MavlinkFTP::Result::FILE_EXISTS ||
        result == MavlinkFTP::Result::FILE_PROTECTED);
}

void test_list_directory(std::shared_ptr<MavlinkFTP> mavlink_ftp, const std::string& path)
{
    auto prom =
        std::make_shared<std::promise<std::pair<MavlinkFTP::Result, std::vector<std::string>>>>();
    auto future_result = prom->get_future();
    mavlink_ftp->list_directory_async(
        path, [prom](MavlinkFTP::Result result, std::vector<std::string> list) {
            prom->set_value(std::pair<MavlinkFTP::Result, std::vector<std::string>>(result, list));
        });

    std::pair<MavlinkFTP::Result, std::vector<std::string>> result = future_result.get();
    EXPECT_EQ(result.first, MavlinkFTP::Result::SUCCESS);
    EXPECT_GT(result.second.size(), 0);
}

MavlinkFTP::Result
test_remove_directory(std::shared_ptr<MavlinkFTP> mavlink_ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    mavlink_ftp->remove_directory_async(
        path, [prom](MavlinkFTP::Result result) { prom->set_value(result); });

    return future_result.get();
}

MavlinkFTP::Result
test_remove_file(std::shared_ptr<MavlinkFTP> mavlink_ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    mavlink_ftp->remove_file_async(
        path, [prom](MavlinkFTP::Result result) { prom->set_value(result); });

    return future_result.get();
}

void test_download(
    std::shared_ptr<MavlinkFTP> mavlink_ftp,
    const std::string& remote_file,
    const std::string& local_path)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    mavlink_ftp->download_async(
        remote_file,
        local_path,
        [](uint32_t bytes_transferred, uint32_t file_size) {
            int percentage = bytes_transferred * 100 / file_size;
            std::cout << "\rDownloading [" << std::setw(3) << percentage << "%] "
                      << bytes_transferred << " of " << file_size;
            if (bytes_transferred >= file_size) {
                std::cout << std::endl;
            }
            _bytes_transferred = bytes_transferred;
            _file_size = file_size;
        },
        [prom](MavlinkFTP::Result result) { prom->set_value(result); });

    MavlinkFTP::Result result = future_result.get();
    EXPECT_EQ(result, MavlinkFTP::Result::SUCCESS);
    EXPECT_GT(_file_size, 0);
    EXPECT_EQ(_bytes_transferred, _file_size);
}

void test_upload(
    std::shared_ptr<MavlinkFTP> mavlink_ftp,
    const std::string& local_file,
    const std::string& remote_folder)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    _bytes_transferred = _file_size = 0;
    mavlink_ftp->upload_async(
        local_file,
        remote_folder,
        [local_file](uint32_t bytes_transferred, uint32_t file_size) {
            int percentage = bytes_transferred * 100 / file_size;
            std::cout << "\rUploading " << local_file << "[" << std::setw(3) << percentage << "%] "
                      << bytes_transferred << " of " << file_size;
            if (bytes_transferred == file_size) {
                std::cout << std::endl;
            }
            _bytes_transferred = bytes_transferred;
            _file_size = file_size;
        },
        [prom](MavlinkFTP::Result result) { prom->set_value(result); });

    MavlinkFTP::Result result = future_result.get();
    EXPECT_GT(_file_size, 0);
    EXPECT_EQ(_bytes_transferred, _file_size);
    EXPECT_EQ(result, MavlinkFTP::Result::SUCCESS);
}

void test_rename(
    std::shared_ptr<MavlinkFTP> mavlink_ftp, const std::string& from, const std::string& to)
{
    auto prom = std::make_shared<std::promise<MavlinkFTP::Result>>();
    auto future_result = prom->get_future();
    mavlink_ftp->rename_async(
        from, to, [prom](MavlinkFTP::Result result) { prom->set_value(result); });

    MavlinkFTP::Result result = future_result.get();
    EXPECT_EQ(result, MavlinkFTP::Result::SUCCESS);
}

void test_crc32(
    std::shared_ptr<MavlinkFTP> mavlink_ftp,
    const std::string& local_file,
    const std::string& remote_file)
{
    uint32_t crc;
    MavlinkFTP::Result r = mavlink_ftp->calc_local_file_crc32(local_file, crc);
    EXPECT_EQ(r, MavlinkFTP::Result::SUCCESS);

    auto prom = std::make_shared<std::promise<std::pair<MavlinkFTP::Result, uint32_t>>>();
    auto future_result = prom->get_future();
    mavlink_ftp->calc_file_crc32_async(
        remote_file, [prom](MavlinkFTP::Result res, uint32_t checksum) {
            prom->set_value(std::pair<MavlinkFTP::Result, uint32_t>(res, checksum));
        });

    std::pair<MavlinkFTP::Result, uint32_t> result = future_result.get();
    EXPECT_EQ(result.first, MavlinkFTP::Result::SUCCESS);
    EXPECT_EQ(result.second, crc);
}

TEST(MavlinkFTPTest, ListDirectory)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);
    System& system = mavsdk.system();
    auto mavlink_ftp = std::make_shared<MavlinkFTP>(system);
    mavlink_ftp->set_timeout(50);
    mavlink_ftp->set_retries(10);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Reset server in case there are stale open sessions
    reset_server(mavlink_ftp);

    test_list_directory(mavlink_ftp, "/");
}

TEST(MavlinkFTPTest, DownloadFile)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);
    System& system = mavsdk.system();
    auto mavlink_ftp = std::make_shared<MavlinkFTP>(system);
    mavlink_ftp->set_timeout(50);
    mavlink_ftp->set_retries(10);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Reset server in case there are stale open sessions
    reset_server(mavlink_ftp);

#ifdef ENABLE_MAVLINK_PASSTHROUGH
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);
    mavlink_passthrough->intercept_incoming_messages_async([this](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.01);
    });
    mavlink_passthrough->intercept_outgoing_messages_async([this](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.01);
    });
#endif

    test_download(mavlink_ftp, "/dataman", ".");
    remove("dataman");
}

TEST(MavlinkFTPTest, UploadFiles)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);
    System& system = mavsdk.system();
    auto mavlink_ftp = std::make_shared<MavlinkFTP>(system);
    mavlink_ftp->set_timeout(50);
    mavlink_ftp->set_retries(10);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Reset server in case there are stale open sessions
    reset_server(mavlink_ftp);

#ifdef ENABLE_MAVLINK_PASSTHROUGH
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);
    mavlink_passthrough->intercept_incoming_messages_async([this](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.01);
    });
    mavlink_passthrough->intercept_outgoing_messages_async([this](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.1);
    });
#endif

    test_create_directory(mavlink_ftp, "/test");

    const int number_of_files = 100;
    for (int i = 0; i < number_of_files; i++) {
        std::string file_name = "test_file_" + std::to_string(i);
        create_test_file(file_name, 1024);
        test_upload(mavlink_ftp, file_name, "/test");
        test_crc32(mavlink_ftp, file_name, "/test/" + file_name);
        remove(file_name.c_str());
    }

    {
        auto prom = std::make_shared<
            std::promise<std::pair<MavlinkFTP::Result, std::vector<std::string>>>>();
        auto future_result = prom->get_future();
        mavlink_ftp->list_directory_async(
            "/test", [prom](MavlinkFTP::Result result, std::vector<std::string> list) {
                prom->set_value(
                    std::pair<MavlinkFTP::Result, std::vector<std::string>>(result, list));
            });

        std::pair<MavlinkFTP::Result, std::vector<std::string>> result = future_result.get();
        int count = 0;
        for (auto entry : result.second) {
            if (entry.rfind("Ftest_file_", 0) == 0) {
                count++;
            }
        }
        EXPECT_EQ(result.first, MavlinkFTP::Result::SUCCESS);
        EXPECT_GT(result.second.size(), 0);
        EXPECT_EQ(count, number_of_files);
    }
}

TEST(MavlinkFTPTest, TestServer)
{
    ConnectionResult ret;

    Mavsdk mavsdk_gcs;
    Mavsdk::Configuration config_gcs(Mavsdk::Configuration::UsageType::GroundStation);
    mavsdk_gcs.set_configuration(config_gcs);
    ret = mavsdk_gcs.add_udp_connection(24550);
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);
    System& system_gcs = mavsdk_gcs.system();

    Mavsdk mavsdk_cc;
    Mavsdk::Configuration config_cc(Mavsdk::Configuration::UsageType::GroundStation);
    mavsdk_cc.set_configuration(config_cc);
    ret = mavsdk_cc.setup_udp_remote("127.0.0.1", 24550);
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);
    System& system_cc = mavsdk_cc.system();

#ifdef ENABLE_MAVLINK_PASSTHROUGH
    auto mavlink_passthrough_cc = std::make_shared<MavlinkPassthrough>(system_cc);
    mavlink_passthrough_cc->intercept_incoming_messages_async([this](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.1);
    });
    mavlink_passthrough_cc->intercept_outgoing_messages_async([this](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.1);
    });
#endif

    auto mavlink_ftp_server = std::make_shared<MavlinkFTP>(system_cc);
    mavlink_ftp_server->set_root_dir(".");
    uint8_t server_comp_id = mavlink_ftp_server->get_our_compid();

    auto mavlink_ftp_client = std::make_shared<MavlinkFTP>(system_gcs);
    mavlink_ftp_client->set_target_component_id(server_comp_id);
    mavlink_ftp_client->set_timeout(50);
    mavlink_ftp_client->set_retries(10);

    test_list_directory(mavlink_ftp_client, "/");

    mavlink_ftp_server->set_root_dir(".");
    test_create_directory(mavlink_ftp_client, "test");

    std::string file_name1 = "ftp_data_file1";
    std::string file_name2 = "ftp_data_file2";
    create_test_file(file_name1, 100000);

    test_upload(mavlink_ftp_client, file_name1, "test");

    test_crc32(mavlink_ftp_client, file_name1, "test/" + file_name1);

    test_rename(mavlink_ftp_client, "test/" + file_name1, "test/" + file_name2);

    test_download(mavlink_ftp_client, "test/" + file_name2, ".");

    uint32_t crc1;
    MavlinkFTP::Result res1 = mavlink_ftp_client->calc_local_file_crc32(file_name1, crc1);
    EXPECT_EQ(res1, MavlinkFTP::Result::SUCCESS);

    uint32_t crc2;
    MavlinkFTP::Result res2 = mavlink_ftp_client->calc_local_file_crc32(file_name2, crc2);
    EXPECT_EQ(res2, MavlinkFTP::Result::SUCCESS);

    EXPECT_EQ(crc1, crc2);
    remove(file_name1.c_str());
    remove(file_name2.c_str());

    MavlinkFTP::Result result = test_remove_file(mavlink_ftp_client, "test/" + file_name1);
    EXPECT_EQ(result, MavlinkFTP::Result::FILE_DOES_NOT_EXIST);

    result = test_remove_file(mavlink_ftp_client, "test/" + file_name2);
    EXPECT_EQ(result, MavlinkFTP::Result::SUCCESS);

    result = test_remove_directory(mavlink_ftp_client, "test");
    EXPECT_EQ(result, MavlinkFTP::Result::SUCCESS);
}
