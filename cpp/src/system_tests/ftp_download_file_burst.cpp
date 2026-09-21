#include "log.hpp"
#include "mavsdk.hpp"
#include <array>
#include <atomic>
#include <cstring>
#include <filesystem>
#include <gtest/gtest.h>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <fstream>
#include <thread>
#include <utility>
#include <vector>
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

TEST(Ftp, DownloadBurstFile)
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
            true,
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
            true,
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

// An empty file leaves the server nothing to stream, so it answers the burst request with
// NAK(EOF). That is how the PX4 server ends a burst's data as well, so it has to be taken
// as "this part has no more data" rather than as a failure.
TEST(Ftp, DownloadBurstEmptyFile)
{
    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, 0));
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
        true,
        [prom](Ftp::Result result, Ftp::ProgressData) {
            if (result != Ftp::Result::Next) {
                prom->set_value(result);
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(5));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Ftp, DownloadBurstBigFile)
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

    auto slow_down_counter = std::make_shared<unsigned>(0);
    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto fut = prom->get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        true,
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Ftp, DownloadBurstBigFileLossy)
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
        true,
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

    auto future_status = fut.wait_for(std::chrono::seconds(30));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    mavsdk_groundstation.unsubscribe_incoming_messages_json(drop_some_in_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(drop_some_out_handle);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Ftp, DownloadBurstStopAndTryAgain)
{
    constexpr int file_size = 1000;
    constexpr int msg_count = file_size / 255 + 6; // 6 messages for transfer initialization

    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, file_size));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    // Once we received half, we want to stop all traffic.
    auto received = std::make_shared<int>(0);
    auto drop_at_some_point_in = [received, msg_count](Mavsdk::MavlinkMessage message) -> bool {
        if (message.message_name == "FILE_TRANSFER_PROTOCOL") {
            (*received)++;
        }
        if (*received >= msg_count / 2) {
            return false;
        }
        return true;
    };

    auto drop_at_some_point_out = [received, msg_count](Mavsdk::MavlinkMessage) -> bool {
        if (*received >= msg_count / 2) {
            return false;
        }
        return true;
    };

    auto drop_at_in_handle =
        mavsdk_groundstation.subscribe_incoming_messages_json(drop_at_some_point_in);
    auto drop_at_out_handle =
        mavsdk_groundstation.subscribe_outgoing_messages_json(drop_at_some_point_out);

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
            true,
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
            true,
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

TEST(Ftp, DownloadBurstFileOutsideOfRoot)
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
            true,
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

namespace {

// Opcodes as defined by the MAVLink FTP protocol.
constexpr uint8_t ftp_cmd_terminate_session = 1;
constexpr uint8_t ftp_cmd_open_file_ro = 4;
constexpr uint8_t ftp_cmd_read_file = 5;
constexpr uint8_t ftp_cmd_burst_read_file = 15;
constexpr uint8_t ftp_rsp_ack = 128;
constexpr uint8_t ftp_rsp_nak = 129;

// The FTP server under test runs on the autopilot component of the autopilot side.
constexpr uint8_t ftp_server_component_id = 1;

constexpr size_t ftp_max_data_length = 239;
// The FILE_TRANSFER_PROTOCOL payload field, which is the FTP header plus the data.
constexpr size_t ftp_payload_length = 12 + ftp_max_data_length;

// The payload of FILE_TRANSFER_PROTOCOL, assembled by hand so that this test can talk
// to the server without going through the FTP client.
struct RawFtpPayload {
    uint16_t seq_number{0};
    uint8_t session{0};
    uint8_t opcode{0};
    uint8_t size{0};
    uint8_t req_opcode{0};
    uint8_t burst_complete{0};
    uint32_t offset{0};
    std::array<uint8_t, ftp_max_data_length> data{};
};

RawFtpPayload parse_ftp_payload(const uint8_t* raw)
{
    RawFtpPayload payload{};
    std::memcpy(&payload.seq_number, raw + 0, sizeof(payload.seq_number));
    payload.session = raw[2];
    payload.opcode = raw[3];
    payload.size = raw[4];
    payload.req_opcode = raw[5];
    payload.burst_complete = raw[6];
    std::memcpy(&payload.offset, raw + 8, sizeof(payload.offset));
    std::memcpy(payload.data.data(), raw + 12, payload.data.size());
    return payload;
}

// Pick the FTP payload out of the JSON fields of a FILE_TRANSFER_PROTOCOL message.
std::optional<RawFtpPayload> parse_ftp_payload_json(const std::string& fields_json)
{
    const auto json = nlohmann::json::parse(fields_json, nullptr, false);
    if (json.is_discarded() || !json.is_object() || !json.contains("payload")) {
        return std::nullopt;
    }

    const auto& raw_json = json.at("payload");
    if (!raw_json.is_array() || raw_json.size() < ftp_payload_length) {
        return std::nullopt;
    }

    std::array<uint8_t, ftp_payload_length> raw{};
    for (size_t i = 0; i < raw.size(); ++i) {
        raw[i] = raw_json[i].get<uint8_t>();
    }

    return parse_ftp_payload(raw.data());
}

// Same, for the messages that the intercept hooks hand out: everything goes through
// there, so the ones that are not FTP are skipped.
std::optional<RawFtpPayload> parse_ftp_payload_json(const Mavsdk::MavlinkMessage& message)
{
    if (message.message_name != "FILE_TRANSFER_PROTOCOL") {
        return std::nullopt;
    }

    return parse_ftp_payload_json(message.fields_json);
}

void serialize_ftp_payload(const RawFtpPayload& payload, uint8_t* raw)
{
    std::memset(raw, 0, ftp_payload_length);
    std::memcpy(raw + 0, &payload.seq_number, sizeof(payload.seq_number));
    raw[2] = payload.session;
    raw[3] = payload.opcode;
    raw[4] = payload.size;
    raw[5] = payload.req_opcode;
    raw[6] = payload.burst_complete;
    std::memcpy(raw + 8, &payload.offset, sizeof(payload.offset));
    std::memcpy(raw + 12, payload.data.data(), payload.data.size());
}

void send_raw_ftp(
    MavlinkDirect& mavlink_direct, uint8_t target_system_id, const RawFtpPayload& payload)
{
    std::array<uint8_t, ftp_payload_length> raw{};
    serialize_ftp_payload(payload, raw.data());

    nlohmann::json fields;
    fields["target_network"] = 0;
    fields["payload"] = raw;

    MavlinkDirect::MavlinkMessage message{};
    message.message_name = "FILE_TRANSFER_PROTOCOL";
    message.target_system_id = target_system_id;
    // The FTP server of the autopilot side runs on its autopilot component.
    message.target_component_id = ftp_server_component_id;
    message.fields_json = fields.dump();

    EXPECT_EQ(mavlink_direct.send_message(message), MavlinkDirect::Result::Success);
}

} // namespace

// A plain read arriving while a burst is running must not move the burst along: the two
// used to share the ifstream position, so the next burst packet carried the bytes from
// wherever the read had left the stream, labelled with the offset the burst was at.
TEST(Ftp, BurstInterleavedRead)
{
    constexpr size_t file_size = 20000;
    // The read is aimed far away from where the burst will be when it goes out, so that
    // data from the wrong place is unmistakable.
    constexpr uint32_t interleaved_read_offset = 15000;
    constexpr unsigned burst_packets_before_read = 3;

    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, file_size));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

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

    auto mavlink_direct = MavlinkDirect{system};
    const auto target_system_id = system->get_system_id();

    std::mutex mutex;
    std::vector<std::pair<uint32_t, std::vector<uint8_t>>> burst_chunks;
    std::optional<std::pair<uint32_t, std::vector<uint8_t>>> read_chunk;
    // How far the burst had got when the reply to the interleaved read came back, so that
    // the test can tell whether the read really did land in a running burst.
    size_t burst_chunks_at_read_reply = 0;
    bool burst_nak = false;
    bool read_sent = false;

    auto open_prom = std::make_shared<std::promise<uint32_t>>();
    auto open_fut = open_prom->get_future();
    auto open_flag = std::make_shared<std::once_flag>();

    auto done_prom = std::make_shared<std::promise<void>>();
    auto done_fut = done_prom->get_future();
    auto done_flag = std::make_shared<std::once_flag>();

    auto handle = mavlink_direct.subscribe_message(
        "FILE_TRANSFER_PROTOCOL", [&](MavlinkDirect::MavlinkMessage message) {
            const auto maybe_payload = parse_ftp_payload_json(message.fields_json);
            if (!maybe_payload) {
                return;
            }
            const auto payload = maybe_payload.value();

            if (payload.req_opcode == ftp_cmd_open_file_ro && payload.opcode == ftp_rsp_ack) {
                uint32_t size{0};
                std::memcpy(&size, payload.data.data(), sizeof(size));
                std::call_once(*open_flag, [&]() { open_prom->set_value(size); });
                return;
            }

            if (payload.req_opcode == ftp_cmd_burst_read_file) {
                std::lock_guard<std::mutex> lock(mutex);
                if (payload.opcode == ftp_rsp_nak) {
                    burst_nak = true;
                    std::call_once(*done_flag, [&]() { done_prom->set_value(); });
                    return;
                }

                burst_chunks.emplace_back(
                    payload.offset,
                    std::vector<uint8_t>(
                        payload.data.begin(), payload.data.begin() + payload.size));

                if (!read_sent && burst_chunks.size() == burst_packets_before_read) {
                    read_sent = true;
                    RawFtpPayload read{};
                    read.seq_number = 1000;
                    read.opcode = ftp_cmd_read_file;
                    read.offset = interleaved_read_offset;
                    read.size = ftp_max_data_length;
                    send_raw_ftp(mavlink_direct, target_system_id, read);
                }

                if (payload.burst_complete) {
                    std::call_once(*done_flag, [&]() { done_prom->set_value(); });
                }
                return;
            }

            if (payload.req_opcode == ftp_cmd_read_file && payload.opcode == ftp_rsp_ack) {
                std::lock_guard<std::mutex> lock(mutex);
                read_chunk = std::make_pair(
                    payload.offset,
                    std::vector<uint8_t>(
                        payload.data.begin(), payload.data.begin() + payload.size));
                burst_chunks_at_read_reply = burst_chunks.size();
            }
        });

    {
        RawFtpPayload open{};
        open.seq_number = 1;
        open.opcode = ftp_cmd_open_file_ro;
        const auto remote_path = temp_file.string();
        std::memcpy(open.data.data(), remote_path.c_str(), remote_path.size());
        open.size = static_cast<uint8_t>(remote_path.size() + 1);
        send_raw_ftp(mavlink_direct, target_system_id, open);
    }

    ASSERT_EQ(open_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    ASSERT_EQ(open_fut.get(), file_size);

    {
        RawFtpPayload burst{};
        burst.seq_number = 2;
        burst.opcode = ftp_cmd_burst_read_file;
        burst.offset = 0;
        burst.size = ftp_max_data_length;
        send_raw_ftp(mavlink_direct, target_system_id, burst);
    }

    EXPECT_EQ(done_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);

    {
        RawFtpPayload terminate{};
        terminate.seq_number = 2000;
        terminate.opcode = ftp_cmd_terminate_session;
        send_raw_ftp(mavlink_direct, target_system_id, terminate);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mavlink_direct.unsubscribe_message(handle);

    std::lock_guard<std::mutex> lock(mutex);

    EXPECT_FALSE(burst_nak);
    EXPECT_TRUE(read_sent);
    EXPECT_GT(burst_chunks.size(), burst_packets_before_read);

    // Every byte of every burst packet has to be the byte that belongs at that offset.
    unsigned corrupt_chunks = 0;
    for (const auto& chunk : burst_chunks) {
        for (size_t i = 0; i < chunk.second.size(); ++i) {
            if (chunk.second[i] != static_cast<uint8_t>((chunk.first + i) % 256)) {
                ++corrupt_chunks;
                break;
            }
        }
    }
    EXPECT_EQ(corrupt_chunks, 0u);

    ASSERT_TRUE(read_chunk.has_value());
    // The read has to have been served while the burst was still running, otherwise this
    // test is not testing anything.
    EXPECT_GT(burst_chunks_at_read_reply, 0u);
    EXPECT_LT(burst_chunks_at_read_reply, burst_chunks.size());
    EXPECT_EQ(read_chunk->first, interleaved_read_offset);
    unsigned corrupt_read_bytes = 0;
    for (size_t i = 0; i < read_chunk->second.size(); ++i) {
        if (read_chunk->second[i] != static_cast<uint8_t>((read_chunk->first + i) % 256)) {
            ++corrupt_read_bytes;
        }
    }
    EXPECT_EQ(corrupt_read_bytes, 0u);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// The server streams a burst in parts, so a file of several times that size has to come
// down in several bursts, with the client asking for the next part each time.
TEST(Ftp, DownloadBurstFileInParts)
{
    // The server sends parts of 35 KB, so this is a handful of them.
    constexpr size_t file_size = 100000;

    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, file_size));
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

    auto parts = std::make_shared<std::atomic<unsigned>>(0);
    auto handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [parts](Mavsdk::MavlinkMessage message) -> bool {
            const auto payload = parse_ftp_payload_json(message);
            if (payload && payload->req_opcode == ftp_cmd_burst_read_file &&
                payload->opcode == ftp_rsp_ack && payload->burst_complete != 0) {
                ++(*parts);
            }
            return true;
        });

    auto ftp = Ftp{system};

    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto fut = prom->get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        true,
        [prom](Ftp::Result result, Ftp::ProgressData) {
            if (result != Ftp::Result::Next) {
                prom->set_value(result);
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(20));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    // More than one burst was needed for this file.
    EXPECT_GE(parts->load(), 2u);

    mavsdk_groundstation.unsubscribe_incoming_messages_json(handle);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// A burst that stalls has to be picked up as a burst again. Asking for the rest of the
// file in 239 byte reads instead means a round trip per packet, which is what this checks
// by counting how many reads it takes.
TEST(Ftp, DownloadBurstStallResumesAsBurst)
{
    constexpr size_t file_size = 100000;
    // Stop all FTP traffic for a while once this many messages have arrived, which is
    // early enough that most of the file is still to come.
    constexpr unsigned blackout_after_messages = 20;
    constexpr auto blackout_duration = std::chrono::milliseconds(400);
    // Only holes should ever be read back, and there are very few of those here. The old
    // behaviour turned the whole rest of the file into reads, which would be hundreds.
    constexpr unsigned max_reads = 30;

    ASSERT_TRUE(create_temp_file(temp_dir_provided / temp_file, file_size));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    auto received = std::make_shared<std::atomic<unsigned>>(0);
    auto blackout_end = std::make_shared<std::mutex>();
    auto blackout_until = std::make_shared<std::optional<std::chrono::steady_clock::time_point>>();

    auto in_blackout = [blackout_end, blackout_until]() {
        std::lock_guard<std::mutex> lock(*blackout_end);
        return blackout_until->has_value() &&
               std::chrono::steady_clock::now() < blackout_until->value();
    };

    auto drop_incoming = [received, blackout_end, blackout_until, in_blackout, blackout_duration](
                             Mavsdk::MavlinkMessage message) -> bool {
        if (message.message_name != "FILE_TRANSFER_PROTOCOL") {
            return true;
        }
        if (++(*received) == blackout_after_messages) {
            std::lock_guard<std::mutex> lock(*blackout_end);
            *blackout_until = std::chrono::steady_clock::now() + blackout_duration;
        }
        return !in_blackout();
    };

    auto drop_outgoing = [in_blackout](Mavsdk::MavlinkMessage message) -> bool {
        if (message.message_name != "FILE_TRANSFER_PROTOCOL") {
            return true;
        }
        return !in_blackout();
    };

    auto drop_in_handle = mavsdk_groundstation.subscribe_incoming_messages_json(drop_incoming);
    auto drop_out_handle = mavsdk_groundstation.subscribe_outgoing_messages_json(drop_outgoing);

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

    auto reads = std::make_shared<std::atomic<unsigned>>(0);
    auto handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [reads](Mavsdk::MavlinkMessage message) -> bool {
            const auto payload = parse_ftp_payload_json(message);
            if (payload && payload->req_opcode == ftp_cmd_read_file &&
                payload->opcode == ftp_rsp_ack) {
                ++(*reads);
            }
            return true;
        });

    auto ftp = Ftp{system};

    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto fut = prom->get_future();
    ftp.download_async(
        temp_file.string(),
        temp_dir_downloaded.string(),
        true,
        [prom](Ftp::Result result, Ftp::ProgressData) {
            if (result != Ftp::Result::Next) {
                prom->set_value(result);
            }
        });

    auto future_status = fut.wait_for(std::chrono::seconds(30));
    ASSERT_EQ(future_status, std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    EXPECT_TRUE(
        are_files_identical(temp_dir_provided / temp_file, temp_dir_downloaded / temp_file));

    EXPECT_GE(received->load(), blackout_after_messages);
    EXPECT_LE(reads->load(), max_reads);

    mavsdk_groundstation.unsubscribe_incoming_messages_json(handle);
    mavsdk_groundstation.unsubscribe_incoming_messages_json(drop_in_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(drop_out_handle);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
