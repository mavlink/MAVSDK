#include "log.hpp"
#include "mavsdk.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
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
#include "plugins/mavlink_direct/mavlink_direct.hpp"
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

    // The callbacks are called for as long as the Mavsdk instances exist, which is longer
    // than the locals of this test if an ASSERT returns early, so they share the state.
    struct State {
        std::mutex sequences_mutex;
        std::vector<uint16_t> open_sequences;
        std::atomic<bool> dropped_first_open_ack{false};
    };
    auto state = std::make_shared<State>();

    auto outgoing = groundstation.subscribe_outgoing_messages_json(
        [state, payload](const Mavsdk::MavlinkMessage& message) {
            const auto bytes = payload(message);
            if (bytes && (*bytes)[3] == 4) { // CMD_OPEN_FILE_RO
                std::lock_guard<std::mutex> lock(state->sequences_mutex);
                state->open_sequences.push_back(
                    static_cast<uint16_t>((*bytes)[0] | ((*bytes)[1] << 8)));
            }
            return true;
        });

    auto incoming = groundstation.subscribe_incoming_messages_json(
        [state, payload](const Mavsdk::MavlinkMessage& message) {
            const auto bytes = payload(message);
            if (bytes && (*bytes)[3] == 128 && (*bytes)[5] == 4 &&
                !state->dropped_first_open_ack.exchange(true)) { // RSP_ACK to CMD_OPEN_FILE_RO
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
    EXPECT_TRUE(state->dropped_first_open_ack.load());
    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    groundstation.unsubscribe_incoming_messages_json(incoming);
    groundstation.unsubscribe_outgoing_messages_json(outgoing);
    std::lock_guard<std::mutex> lock(state->sequences_mutex);
    ASSERT_GE(state->open_sequences.size(), 2);
    EXPECT_EQ(state->open_sequences[0], state->open_sequences[1]);
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

// When a download fails, whatever arrived is already in the local file by the time the
// caller hears about it, so that the caller can look at what arrived.
TEST(Ftp, DownloadFailureLeavesReceivedBytesOnDisk)
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

    struct Outcome {
        Ftp::Result result;
        uint32_t bytes_transferred;
        uintmax_t bytes_on_disk;
    };
    auto prom = std::make_shared<std::promise<Outcome>>();
    auto fut = prom->get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        false,
        [prom, got_some](Ftp::Result result, Ftp::ProgressData progress_data) {
            if (progress_data.bytes_transferred > 500) {
                *got_some = true;
            }
            if (result != Ftp::Result::Next) {
                // Check the file from inside the result callback, as a caller would.
                std::error_code ec;
                const auto on_disk = fs::file_size(temp_dir_downloaded / temp_file, ec);
                prom->set_value(Outcome{result, progress_data.bytes_transferred, ec ? 0 : on_disk});
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    const auto outcome = fut.get();
    LogInfo(
        "Result {}, {} bytes reported, {} bytes on disk",
        to_string(outcome.result),
        outcome.bytes_transferred,
        outcome.bytes_on_disk);
    EXPECT_EQ(outcome.result, Ftp::Result::Timeout);
    EXPECT_GT(outcome.bytes_transferred, 500u);
    EXPECT_EQ(outcome.bytes_on_disk, outcome.bytes_transferred);

    mavsdk_groundstation.unsubscribe_incoming_messages_json(drop_at_in_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(drop_at_out_handle);

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

TEST(Ftp, ReadFileAcrossEof)
{
    ASSERT_TRUE(reset_directories(temp_dir_provided));

    constexpr uint8_t cmd_terminate_session = 1;
    constexpr uint8_t cmd_open_file_ro = 4;
    constexpr uint8_t cmd_read_file = 5;
    constexpr uint8_t rsp_ack = 128;
    constexpr uint8_t rsp_nak = 129;
    constexpr uint8_t err_eof = 6;
    constexpr uint8_t max_data_length = 239;
    constexpr size_t payload_length = 12 + max_data_length;

    const fs::path remote_file = "partial_tail.bin";
    std::vector<uint8_t> content(300);
    for (size_t i = 0; i < content.size(); ++i) {
        content[i] = static_cast<uint8_t>(i * 11 + 5);
    }
    {
        std::ofstream out(temp_dir_provided / remote_file, std::ios::binary | std::ios::trunc);
        out.write(
            reinterpret_cast<const char*>(content.data()),
            static_cast<std::streamsize>(content.size()));
        ASSERT_TRUE(out.good());
    }

    std::mutex mutex;
    uint16_t waiting_seq = 0;
    std::shared_ptr<std::promise<std::array<uint8_t, payload_length>>> waiting_promise;

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
    auto mavlink_direct = MavlinkDirect{maybe_system.value()};

    auto sub_handle = mavlink_direct.subscribe_message(
        "FILE_TRANSFER_PROTOCOL", [&](MavlinkDirect::MavlinkMessage message) {
            const auto json = nlohmann::json::parse(message.fields_json, nullptr, false);
            if (json.is_discarded() || !json.contains("payload") ||
                json["payload"].size() < payload_length) {
                return;
            }
            std::array<uint8_t, payload_length> raw{};
            for (size_t i = 0; i < payload_length; ++i) {
                raw[i] = json["payload"][i].get<uint8_t>();
            }
            const auto seq = static_cast<uint16_t>(raw[0] | (raw[1] << 8));
            std::lock_guard<std::mutex> lock(mutex);
            if (waiting_promise && seq == waiting_seq) {
                auto prom = std::move(waiting_promise);
                prom->set_value(raw);
            }
        });

    uint16_t next_seq = 10;
    auto request = [&](uint8_t session,
                       uint8_t opcode,
                       uint8_t size,
                       uint32_t offset,
                       const std::string& path = {}) {
        const uint16_t seq = next_seq;
        next_seq = static_cast<uint16_t>(next_seq + 2);

        std::array<uint8_t, payload_length> raw{};
        std::memcpy(raw.data(), &seq, sizeof(seq));
        raw[2] = session;
        raw[3] = opcode;
        raw[4] = size;
        std::memcpy(raw.data() + 8, &offset, sizeof(offset));
        std::memcpy(raw.data() + 12, path.data(), path.size());

        auto prom = std::make_shared<std::promise<std::array<uint8_t, payload_length>>>();
        auto fut = prom->get_future();
        {
            std::lock_guard<std::mutex> lock(mutex);
            waiting_seq = static_cast<uint16_t>(seq + 1);
            waiting_promise = prom;
        }

        MavlinkDirect::MavlinkMessage msg{};
        msg.message_name = "FILE_TRANSFER_PROTOCOL";
        msg.target_system_id = 1;
        msg.target_component_id = 1;
        msg.fields_json =
            nlohmann::json{
                {"target_network", 0},
                {"target_system", 1},
                {"target_component", 1},
                {"payload", raw},
            }
                .dump();
        EXPECT_EQ(mavlink_direct.send_message(msg), MavlinkDirect::Result::Success);
        if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
            ADD_FAILURE() << "Timed out waiting for FTP reply";
            return std::array<uint8_t, payload_length>{};
        }
        return fut.get();
    };

    const std::string path = remote_file.string();
    const auto open_rsp =
        request(0, cmd_open_file_ro, static_cast<uint8_t>(path.size() + 1), 0, path);
    ASSERT_EQ(open_rsp[3], rsp_ack);
    const uint8_t session = open_rsp[2];

    const auto first = request(session, cmd_read_file, max_data_length, 0);
    EXPECT_EQ(first[3], rsp_ack);
    EXPECT_EQ(first[4], max_data_length);
    EXPECT_EQ(std::memcmp(first.data() + 12, content.data(), max_data_length), 0);

    // Request a full chunk across EOF; server should return the remaining 61 bytes.
    const size_t expected_tail = content.size() - max_data_length;
    const auto tail = request(session, cmd_read_file, max_data_length, max_data_length);
    EXPECT_EQ(tail[3], rsp_ack);
    EXPECT_EQ(tail[4], expected_tail);
    EXPECT_EQ(std::memcmp(tail.data() + 12, content.data() + max_data_length, expected_tail), 0);

    const auto eof_rsp =
        request(session, cmd_read_file, max_data_length, static_cast<uint32_t>(content.size()));
    EXPECT_EQ(eof_rsp[3], rsp_nak);
    EXPECT_EQ(eof_rsp[12], err_eof);

    const auto close_rsp = request(session, cmd_terminate_session, 0, 0);
    EXPECT_EQ(close_rsp[3], rsp_ack);

    mavlink_direct.unsubscribe_message(sub_handle);
}
