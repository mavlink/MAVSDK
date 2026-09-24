#include "log.hpp"
#include "mavsdk.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include <fstream>
#include <thread>
#include "plugins/ftp/ftp.hpp"
#include "plugins/ftp_server/ftp_server.hpp"
#include "fs_helpers.hpp"
#include "unused.hpp"
#include <nlohmann/json.hpp>

using namespace mavsdk;

static constexpr double reduced_timeout_s = 0.1;

static const fs::path temp_dir_provided = test_data_dir() / "provided";
static const fs::path temp_dir_downloaded = test_data_dir() / "downloaded";

static const fs::path temp_file = "data.bin";

TEST(Ftp, RetryOpenKeepsRequestSequence)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    groundstation.set_timeout_s(reduced_timeout_s);
    Mavsdk autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    autopilot.set_timeout_s(reduced_timeout_s);

    // FTP payload bytes: sequence at 0..1, opcode at 3, request opcode at 5.
    auto payload =
        [](const Mavsdk::MavlinkMessage& message) -> std::optional<std::array<uint8_t, 6>> {
        if (message.message_name != "FILE_TRANSFER_PROTOCOL") {
            return std::nullopt;
        }
        const auto fields = nlohmann::json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded() || !fields.contains("payload") || !fields["payload"].is_array() ||
            fields["payload"].size() < 6) {
            return std::nullopt;
        }
        std::array<uint8_t, 6> result{};
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = fields["payload"][i].get<uint8_t>();
        }
        return result;
    };

    std::mutex sequences_mutex;
    std::vector<uint16_t> open_sequences;
    auto outgoing =
        groundstation.subscribe_outgoing_messages_json([&](const Mavsdk::MavlinkMessage& message) {
            const auto bytes = payload(message);
            if (bytes && (*bytes)[3] == 4) { // CMD_OPEN_FILE_RO
                std::lock_guard<std::mutex> lock(sequences_mutex);
                open_sequences.push_back(static_cast<uint16_t>((*bytes)[0] | ((*bytes)[1] << 8)));
            }
            return true;
        });

    std::atomic<bool> dropped_first_open_ack{false};
    auto incoming =
        groundstation.subscribe_incoming_messages_json([&](const Mavsdk::MavlinkMessage& message) {
            const auto bytes = payload(message);
            if (bytes && (*bytes)[3] == 128 && (*bytes)[5] == 4 &&
                !dropped_first_open_ack.exchange(true)) { // RSP_ACK to CMD_OPEN_FILE_RO
                return false;
            }
            return true;
        });

    ASSERT_EQ(groundstation.add_any_connection("udpin://0.0.0.0:17000"), ConnectionResult::Success);
    ASSERT_EQ(autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto server = FtpServer{autopilot.server_component()};
    server.set_root_dir(temp_dir_provided.string());
    auto system = groundstation.first_autopilot(10.0);
    ASSERT_TRUE(system);
    auto ftp = Ftp{system.value()};

    auto promise = std::make_shared<std::promise<Ftp::Result>>();
    auto result = promise->get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        false,
        [promise](Ftp::Result value, Ftp::ProgressData) {
            if (value != Ftp::Result::Next) {
                promise->set_value(value);
            }
        });
    ASSERT_EQ(result.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(result.get(), Ftp::Result::Success);
    EXPECT_TRUE(dropped_first_open_ack.load());
    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    groundstation.unsubscribe_incoming_messages_json(incoming);
    groundstation.unsubscribe_outgoing_messages_json(outgoing);
    std::lock_guard<std::mutex> lock(sequences_mutex);
    ASSERT_GE(open_sequences.size(), 2);
    EXPECT_EQ(open_sequences[0], open_sequences[1]);
}

TEST(Ftp, DownloadFile)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    // First we try to access the file without the root directory set.
    // We expect that an error as we don't have any permission.
    {
        auto prom = std::make_shared<std::promise<Ftp::Result>>();
        auto fut = prom->get_future();
        ftp.download_async(
            temp_file.string(),
            temp_dir_downloaded.string(),
            false,
            [prom](Ftp::Result result, Ftp::ProgressData) {
                if (result != Ftp::Result::Next) {
                    prom->set_value(result);
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(1));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::ProtocolError);
    }

    // Now we set the root dir and expect it to work.
    ftp_server.set_root_dir(temp_dir_provided.string());

    {
        auto prom = std::make_shared<std::promise<Ftp::Result>>();
        auto fut = prom->get_future();
        ftp.download_async(
            temp_file.string(),
            temp_dir_downloaded.string(),
            false,
            [prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                if (result != Ftp::Result::Next) {
                    prom->set_value(result);
                } else {
                    LogDebug(
                        "Download progress: {}/{} bytes",
                        progress_data.bytes_transferred,
                        progress_data.total_bytes);
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

TEST(Ftp, DownloadBigFile)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50000));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    ftp_server.set_root_dir(temp_dir_provided.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto fut = prom->get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        false,
        [prom](Ftp::Result result, Ftp::ProgressData progress_data) {
            if (result != Ftp::Result::Next) {
                prom->set_value(result);
            } else {
                LogDebug(
                    "Download progress: {}/{} bytes",
                    progress_data.bytes_transferred,
                    progress_data.total_bytes);
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(20));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Ftp, DownloadBigFileLossy)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 10000));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    auto counter = std::make_shared<std::atomic<unsigned>>(0);
    auto drop_some = [counter](Mavsdk::MavlinkMessage) -> bool { return (*counter)++ % 5 != 0; };

    auto drop_some_in_handle = mavsdk_groundstation.subscribe_incoming_messages_json(drop_some);
    auto drop_some_out_handle = mavsdk_groundstation.subscribe_outgoing_messages_json(drop_some);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    ftp_server.set_root_dir(temp_dir_provided.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    auto slow_down_counter = std::make_shared<unsigned>(0);
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto fut = prom->get_future();
    ftp.download_async(
        ("" / temp_file).string(),
        temp_dir_downloaded.string(),
        false,
        [prom, slow_down_counter](Ftp::Result result, Ftp::ProgressData progress_data) {
            if (result != Ftp::Result::Next) {
                prom->set_value(result);
            } else {
                if ((*slow_down_counter)++ % 10 == 0) {
                    LogDebug(
                        "Download progress: {}/{} bytes",
                        progress_data.bytes_transferred,
                        progress_data.total_bytes);
                }
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(20));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    mavsdk_groundstation.unsubscribe_incoming_messages_json(drop_some_in_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(drop_some_out_handle);
}

TEST(Ftp, DownloadStopAndTryAgain)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 5000));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    // Once we received some, we want to stop all traffic.
    auto got_some = std::make_shared<std::atomic<bool>>(false);
    auto drop_at_some_point = [got_some](Mavsdk::MavlinkMessage) -> bool { return !*got_some; };

    auto drop_at_in_handle =
        mavsdk_groundstation.subscribe_incoming_messages_json(drop_at_some_point);
    auto drop_at_out_handle =
        mavsdk_groundstation.subscribe_outgoing_messages_json(drop_at_some_point);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    ftp_server.set_root_dir(temp_dir_provided.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    {
        auto prom = std::make_shared<std::promise<Ftp::Result>>();
        auto fut = prom->get_future();
        ftp.download_async(
            ("" / temp_file).string(),
            temp_dir_downloaded.string(),
            false,
            [prom, got_some](Ftp::Result result, Ftp::ProgressData progress_data) {
                if (progress_data.bytes_transferred > 500) {
                    *got_some = true;
                }
                if (result != Ftp::Result::Next) {
                    LogDebug("Got result: {}", to_string(result));
                    prom->set_value(result);
                } else {
                    LogDebug(
                        "Download progress: {}/{} bytes",
                        progress_data.bytes_transferred,
                        progress_data.total_bytes);
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(10));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::Timeout);
    }

    // Let the traffic through again for the next attempt.
    mavsdk_groundstation.unsubscribe_incoming_messages_json(drop_at_in_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(drop_at_out_handle);

    {
        // Now try again
        auto prom = std::make_shared<std::promise<Ftp::Result>>();
        auto fut = prom->get_future();
        ftp.download_async(
            ("" / temp_file).string(),
            temp_dir_downloaded.string(),
            false,
            [prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                if (result != Ftp::Result::Next) {
                    prom->set_value(result);
                } else {
                    LogDebug(
                        "Download progress: {}/{} bytes",
                        progress_data.bytes_transferred,
                        progress_data.total_bytes);
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(10));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::Success);
        EXPECT_TRUE(
            are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Ftp, DownloadFileOutsideOfRoot)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    // Now we set the root dir and expect it to work.
    ftp_server.set_root_dir(temp_dir_provided.string());

    {
        auto prom = std::make_shared<std::promise<Ftp::Result>>();
        auto fut = prom->get_future();
        ftp.download_async(
            (fs::path("") / ".." / temp_file).string(),
            temp_dir_downloaded.string(),
            false,
            [prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                UNUSED(progress_data);
                if (result != Ftp::Result::Next) {
                    prom->set_value(result);
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(1));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::ProtocolError);
    }
}

TEST(Ftp, DownloadFileOutsideOfRootSharedPrefix)
{
    // Regression test for a path traversal where a sibling directory whose name
    // shares a prefix with the root dir (e.g. root "provided", sibling
    // "provided_secret") slipped past a plain string-prefix containment check.
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 50));

    // Create a sibling directory next to the root whose name starts with the
    // root dir's name, and put a secret file in it.
    const fs::path sibling_dir =
        temp_dir_provided.parent_path() / (temp_dir_provided.filename().string() + "_secret");
    const fs::path secret_file = "secret.bin";
    ASSERT_TRUE(reset_directories(sibling_dir));
    ASSERT_TRUE(create_temp_file(sibling_dir / secret_file, 50));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto ftp = Ftp{system};

    ftp_server.set_root_dir(temp_dir_provided.string());

    {
        // Try to reach the sibling directory via "../provided_secret/secret.bin".
        const auto escape_path = (fs::path("..") / sibling_dir.filename() / secret_file).string();

        auto prom = std::make_shared<std::promise<Ftp::Result>>();
        auto fut = prom->get_future();
        ftp.download_async(
            escape_path,
            temp_dir_downloaded.string(),
            false,
            [prom](Ftp::Result result, Ftp::ProgressData progress_data) {
                UNUSED(progress_data);
                if (result != Ftp::Result::Next) {
                    prom->set_value(result);
                }
            });

        auto future_status = fut.wait_for(std::chrono::seconds(1));
        ASSERT_EQ(future_status, std::future_status::ready);
        EXPECT_EQ(fut.get(), Ftp::Result::ProtocolError);
    }
}
