#include <cstring>
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <atomic>
#include <future>

#include "integration_test_helper.h"
#include "mavsdk.h"
#include "system.h"
#include "plugins/ftp/ftp.h"

#include <random>
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
static std::default_random_engine random_engine;
static std::uniform_real_distribution<> distribution(0.0, 1.0);

using namespace mavsdk;

static std::atomic<uint32_t> _bytes_transferred{0};
static std::atomic<uint32_t> _file_size{0};

void reset_server(std::shared_ptr<Ftp>& ftp)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->reset_async([prom](Ftp::Result result) { prom->set_value(result); });
    future_result.get();
}

void create_test_file(std::string file_name, uint32_t size)
{
    std::string str(size, 'X');
    std::ofstream of(file_name, std::fstream::trunc | std::fstream::binary);
    of << str;
    of.close();
}

void test_create_directory(std::shared_ptr<Ftp> ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->create_directory_async(path, [prom](Ftp::Result result) { prom->set_value(result); });

    Ftp::Result result = future_result.get();
    EXPECT_TRUE(
        result == Ftp::Result::Success || result == Ftp::Result::FileExists ||
        result == Ftp::Result::FileProtected);
}

void test_list_directory(std::shared_ptr<Ftp> ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<std::pair<Ftp::Result, std::vector<std::string>>>>();
    auto future_result = prom->get_future();
    ftp->list_directory_async(path, [prom](Ftp::Result result, std::vector<std::string> list) {
        prom->set_value(std::pair<Ftp::Result, std::vector<std::string>>(result, list));
    });

    std::pair<Ftp::Result, std::vector<std::string>> result = future_result.get();
    EXPECT_EQ(result.first, Ftp::Result::Success);
    EXPECT_GT(result.second.size(), 0);
}

Ftp::Result test_remove_directory(std::shared_ptr<Ftp> ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->remove_directory_async(path, [prom](Ftp::Result result) { prom->set_value(result); });

    return future_result.get();
}

Ftp::Result test_remove_file(std::shared_ptr<Ftp> ftp, const std::string& path)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->remove_file_async(path, [prom](Ftp::Result result) { prom->set_value(result); });

    return future_result.get();
}

void test_download(
    std::shared_ptr<Ftp> ftp, const std::string& remote_file, const std::string& local_path)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();

    ftp->download_async(
        remote_file, local_path, [prom](Ftp::Result result, Ftp::ProgressData progress) {
            if (result == Ftp::Result::Next) {
                int percentage = progress.bytes_transferred * 100 / progress.total_bytes;
                std::cout << "\rDownloading [" << std::setw(3) << percentage << "%] "
                          << progress.bytes_transferred << " of " << progress.total_bytes;
                if (progress.bytes_transferred >= progress.total_bytes) {
                    std::cout << '\n';
                }
                _bytes_transferred = progress.bytes_transferred;
                _file_size = progress.total_bytes;
            } else {
                prom->set_value(result);
            }
        });

    Ftp::Result result = future_result.get();
    EXPECT_EQ(result, Ftp::Result::Success);
    EXPECT_GT(_file_size, 0);
    EXPECT_EQ(_bytes_transferred, _file_size);
}

void test_upload(
    std::shared_ptr<Ftp> ftp, const std::string& local_file, const std::string& remote_folder)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    _bytes_transferred = _file_size = 0;
    ftp->upload_async(
        local_file, remote_folder, [prom](Ftp::Result result, Ftp::ProgressData progress) {
            if (result == Ftp::Result::Next) {
                int percentage = progress.bytes_transferred * 100 / progress.total_bytes;
                std::cout << "\rUploading "
                          << "[" << std::setw(3) << percentage << "%] "
                          << progress.bytes_transferred << " of " << progress.total_bytes;
                if (progress.bytes_transferred == progress.total_bytes) {
                    std::cout << '\n';
                }
                _bytes_transferred = progress.bytes_transferred;
                _file_size = progress.total_bytes;
            } else {
                prom->set_value(result);
            }
        });

    Ftp::Result result = future_result.get();
    EXPECT_GT(_file_size, 0);
    EXPECT_EQ(_bytes_transferred, _file_size);
    EXPECT_EQ(result, Ftp::Result::Success);
}

void test_rename(std::shared_ptr<Ftp> ftp, const std::string& from, const std::string& to)
{
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto future_result = prom->get_future();
    ftp->rename_async(from, to, [prom](Ftp::Result result) { prom->set_value(result); });

    Ftp::Result result = future_result.get();
    EXPECT_EQ(result, Ftp::Result::Success);
}

void compare(
    std::shared_ptr<Ftp> ftp, const std::string& local_file, const std::string& remote_file)
{
    auto prom = std::make_shared<std::promise<std::pair<Ftp::Result, bool>>>();
    auto future_result = prom->get_future();

    ftp->are_files_identical_async(
        local_file, remote_file, [&prom](Ftp::Result result, bool identical) {
            prom->set_value(std::make_pair<>(result, identical));
        });

    auto result = future_result.get();
    EXPECT_EQ(result.first, Ftp::Result::Success);
    EXPECT_EQ(result.second, true);
}

TEST(FtpTest, ListDirectory)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    auto ftp = std::make_shared<Ftp>(system);

    // Reset server in case there are stale open sessions
    reset_server(ftp);

    test_list_directory(ftp, "/");
}

TEST(FtpTest, DownloadFile)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    auto ftp = std::make_shared<Ftp>(system);

    // Reset server in case there are stale open sessions
    reset_server(ftp);

    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);
    mavsdk.intercept_incoming_messages_async([](const mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.01);
    });
    mavsdk.intercept_outgoing_messages_async([](const mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.01);
    });

    test_download(ftp, "/dataman", ".");
    remove("dataman");
}

TEST(FtpTest, UploadFiles)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    auto ftp = std::make_shared<Ftp>(system);

    // Reset server in case there are stale open sessions
    reset_server(ftp);

    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);
    mavsdk.intercept_incoming_messages_async([](const mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.01);
    });
    mavsdk.intercept_outgoing_messages_async([](const mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.1);
    });

    test_create_directory(ftp, "/test");

    const int number_of_files = 100;
    for (int i = 0; i < number_of_files; i++) {
        std::string file_name = "test_file_" + std::to_string(i);
        create_test_file(file_name, 1024);
        test_upload(ftp, file_name, "/test");
        compare(ftp, file_name, "/test/" + file_name);
        remove(file_name.c_str());
    }

    {
        auto prom =
            std::make_shared<std::promise<std::pair<Ftp::Result, std::vector<std::string>>>>();
        auto future_result = prom->get_future();
        ftp->list_directory_async(
            "/test", [prom](Ftp::Result result, std::vector<std::string> list) {
                prom->set_value(std::pair<Ftp::Result, std::vector<std::string>>(result, list));
            });

        std::pair<Ftp::Result, std::vector<std::string>> result = future_result.get();
        int count = 0;
        for (auto entry : result.second) {
            if (entry.rfind("Ftest_file_", 0) == 0) {
                count++;
            }
        }
        EXPECT_EQ(result.first, Ftp::Result::Success);
        EXPECT_GT(result.second.size(), 0);
        EXPECT_EQ(count, number_of_files);
    }
}

TEST(FtpTest, TestServer)
{
    Mavsdk mavsdk_gcs;
    Mavsdk::Configuration config_gcs(Mavsdk::Configuration::UsageType::GroundStation);
    mavsdk_gcs.set_configuration(config_gcs);
    ASSERT_EQ(mavsdk_gcs.add_any_connection("udp://:24550"), ConnectionResult::Success);

    Mavsdk mavsdk_cc;
    Mavsdk::Configuration config_cc(Mavsdk::Configuration::UsageType::GroundStation);
    mavsdk_cc.set_configuration(config_cc);
    ASSERT_EQ(mavsdk_cc.add_any_connection("udp://127.0.0.1:24550"), ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto system_gcs = mavsdk_gcs.systems().at(0);
    auto system_cc = mavsdk_cc.systems().at(0);

    auto mavlink_passthrough_cc = std::make_shared<MavlinkPassthrough>(system_cc);
    mavsdk_cc.intercept_incoming_messages_async([](const mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.1);
    });
    mavsdk_cc.intercept_outgoing_messages_async([](const mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
            return true;
        }
        return (distribution(random_engine) > 0.1);
    });

    auto ftp_server = std::make_shared<Ftp>(system_cc);
    ftp_server->set_root_directory(".");
    uint8_t server_comp_id = ftp_server->get_our_compid();

    auto ftp_client = std::make_shared<Ftp>(system_gcs);
    ftp_client->set_target_compid(server_comp_id);

    test_list_directory(ftp_client, "/");

    ftp_server->set_root_directory(".");
    test_create_directory(ftp_client, "test");

    std::string file_name1 = "ftp_data_file1";
    std::string file_name2 = "ftp_data_file2";
    create_test_file(file_name1, 100000);

    test_upload(ftp_client, file_name1, "test");

    compare(ftp_client, file_name1, "test/" + file_name1);

    test_rename(ftp_client, "test/" + file_name1, "test/" + file_name2);

    compare(ftp_client, file_name1, "test/" + file_name2);

    test_download(ftp_client, "test/" + file_name2, ".");

    Ftp::Result result = test_remove_file(ftp_client, "test/" + file_name1);
    EXPECT_EQ(result, Ftp::Result::FileDoesNotExist);

    result = test_remove_file(ftp_client, "test/" + file_name2);
    EXPECT_EQ(result, Ftp::Result::Success);

    result = test_remove_directory(ftp_client, "test");
    EXPECT_EQ(result, Ftp::Result::Success);
}
