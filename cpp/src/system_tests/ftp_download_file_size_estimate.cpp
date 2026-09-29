#include "mavsdk.hpp"
#include <array>
#include <cstring>
#include <future>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include "plugins/ftp/ftp.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include "fs_helpers.hpp"
#include <nlohmann/json.hpp>

using namespace mavsdk;

namespace {

constexpr double reduced_timeout_s = 0.1;

const fs::path temp_dir_downloaded = test_data_dir() / "downloaded";

// The fake server answers on a component of its own, where no built-in FTP server of the
// autopilot side is listening.
constexpr uint8_t fake_server_component_id = 100;

constexpr uint8_t cmd_terminate_session = 1;
constexpr uint8_t cmd_reset_sessions = 2;
constexpr uint8_t cmd_open_file_ro = 4;
constexpr uint8_t cmd_read_file = 5;
constexpr uint8_t cmd_burst_read_file = 15;
constexpr uint8_t rsp_ack = 128;
constexpr uint8_t rsp_nak = 129;

constexpr uint8_t err_fail_errno = 2;
constexpr uint8_t err_eof = 6;

constexpr size_t max_data_length = 239;
constexpr size_t payload_length = 12 + max_data_length;

struct Payload {
    uint16_t seq_number{0};
    uint8_t session{0};
    uint8_t opcode{0};
    uint8_t size{0};
    uint8_t req_opcode{0};
    uint8_t burst_complete{0};
    uint32_t offset{0};
    std::array<uint8_t, max_data_length> data{};
};

// An FTP server that behaves like ArduPilot's does for its virtual files, such as
// @PARAM/param.pck: the size it reports when a file is opened is only an estimate, it
// only accepts reads of the size of the first one on a handle, a burst ends with
// NAK(EOF), and a request with a sequence number one less than that of its last reply gets
// that reply again.
class FakeFtpServer {
public:
    FakeFtpServer(
        std::shared_ptr<ServerComponent> server_component,
        std::vector<uint8_t> content,
        uint32_t reported_size) :
        _mavlink_direct{server_component},
        _content(std::move(content)),
        _reported_size(reported_size)
    {
        _handle = _mavlink_direct.subscribe_message(
            "FILE_TRANSFER_PROTOCOL",
            [this](MavlinkDirectServer::MavlinkMessage message) { handle(message); });
    }

    ~FakeFtpServer() { _mavlink_direct.unsubscribe_message(_handle); }

private:
    void handle(const MavlinkDirectServer::MavlinkMessage& message)
    {
        const auto json = nlohmann::json::parse(message.fields_json, nullptr, false);
        if (json.is_discarded() || json.value("target_component", 0) != fake_server_component_id) {
            return;
        }

        const auto& raw_json = json.at("payload");
        std::array<uint8_t, payload_length> raw{};
        for (size_t i = 0; i < raw.size() && i < raw_json.size(); ++i) {
            raw[i] = raw_json[i].get<uint8_t>();
        }
        const Payload request = parse(raw);

        std::lock_guard<std::mutex> lock(_mutex);
        _target_system_id = message.system_id;
        _target_component_id = message.component_id;

        if (_last_reply &&
            static_cast<uint16_t>(request.seq_number + 1) == _last_reply->seq_number) {
            send(*_last_reply);
            return;
        }

        Payload reply{};
        reply.seq_number = request.seq_number + 1;
        reply.req_opcode = request.opcode;
        reply.opcode = rsp_ack;

        switch (request.opcode) {
            case cmd_open_file_ro:
                _read_size = 0;
                reply.size = sizeof(_reported_size);
                std::memcpy(reply.data.data(), &_reported_size, sizeof(_reported_size));
                break;
            case cmd_read_file:
                read(request.offset, request.size, reply);
                break;
            case cmd_burst_read_file:
                reply.offset = request.offset;
                while (read(reply.offset, request.size, reply)) {
                    send(reply);
                    reply.offset += reply.size;
                    ++reply.seq_number;
                }
                reply.burst_complete = 1;
                break;
            case cmd_terminate_session:
            case cmd_reset_sessions:
                break;
            default:
                return;
        }

        send(reply);
    }

    // Fills in the reply for a read, or a NAK if there is nothing to read.
    bool read(uint32_t offset, uint8_t size, Payload& reply)
    {
        if (_read_size == 0) {
            _read_size = size;
        }
        if (size != _read_size) {
            reply.opcode = rsp_nak;
            reply.size = 2;
            reply.data[0] = err_fail_errno;
            reply.data[1] = EINVAL;
            return false;
        }
        if (offset >= _content.size()) {
            reply.opcode = rsp_nak;
            reply.size = 1;
            reply.data[0] = err_eof;
            return false;
        }
        reply.opcode = rsp_ack;
        reply.offset = offset;
        reply.size = static_cast<uint8_t>(std::min(size_t(size), _content.size() - offset));
        std::memcpy(reply.data.data(), _content.data() + offset, reply.size);
        return true;
    }

    void send(const Payload& payload)
    {
        _last_reply = payload;

        nlohmann::json fields;
        fields["target_network"] = 0;
        fields["target_system"] = _target_system_id;
        fields["target_component"] = _target_component_id;
        fields["payload"] = serialize(payload);

        MavlinkDirectServer::MavlinkMessage message{};
        message.message_name = "FILE_TRANSFER_PROTOCOL";
        message.target_system_id = _target_system_id;
        message.target_component_id = _target_component_id;
        message.fields_json = fields.dump();
        EXPECT_EQ(_mavlink_direct.send_message(message), MavlinkDirectServer::Result::Success);
    }

    static Payload parse(const std::array<uint8_t, payload_length>& raw)
    {
        Payload payload{};
        std::memcpy(&payload.seq_number, raw.data(), sizeof(payload.seq_number));
        payload.session = raw[2];
        payload.opcode = raw[3];
        payload.size = raw[4];
        payload.req_opcode = raw[5];
        payload.burst_complete = raw[6];
        std::memcpy(&payload.offset, raw.data() + 8, sizeof(payload.offset));
        std::memcpy(payload.data.data(), raw.data() + 12, payload.data.size());
        return payload;
    }

    static std::array<uint8_t, payload_length> serialize(const Payload& payload)
    {
        std::array<uint8_t, payload_length> raw{};
        std::memcpy(raw.data(), &payload.seq_number, sizeof(payload.seq_number));
        raw[2] = payload.session;
        raw[3] = payload.opcode;
        raw[4] = payload.size;
        raw[5] = payload.req_opcode;
        raw[6] = payload.burst_complete;
        std::memcpy(raw.data() + 8, &payload.offset, sizeof(payload.offset));
        std::memcpy(raw.data() + 12, payload.data.data(), payload.data.size());
        return raw;
    }

    MavlinkDirectServer _mavlink_direct;
    MavlinkDirectServer::MessageHandle _handle{};
    std::mutex _mutex;
    const std::vector<uint8_t> _content;
    const uint32_t _reported_size;
    uint8_t _read_size{0};
    std::optional<Payload> _last_reply{};
    uint32_t _target_system_id{0};
    uint32_t _target_component_id{0};
};

void download_with_size_estimate(size_t actual_size, uint32_t reported_size, bool use_burst)
{
    const fs::path remote_file = "estimated.bin";

    std::vector<uint8_t> content(actual_size);
    for (size_t i = 0; i < content.size(); ++i) {
        content[i] = static_cast<uint8_t>(i * 7 + i / 256);
    }

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

    FakeFtpServer fake_server{mavsdk_autopilot.server_component(), content, reported_size};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    auto ftp = Ftp{system};
    ASSERT_EQ(ftp.set_target_compid(fake_server_component_id), Ftp::Result::Success);

    auto prom = std::make_shared<std::promise<Ftp::Result>>();
    auto fut = prom->get_future();
    ftp.download_async(
        remote_file.string(),
        temp_dir_downloaded.string(),
        use_burst,
        [prom](Ftp::Result result, Ftp::ProgressData) {
            if (result != Ftp::Result::Next) {
                prom->set_value(result);
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(fut.get(), Ftp::Result::Success);

    std::ifstream downloaded(temp_dir_downloaded / remote_file, std::ios::binary);
    const std::vector<uint8_t> downloaded_content{
        std::istreambuf_iterator<char>(downloaded), std::istreambuf_iterator<char>()};
    EXPECT_EQ(downloaded_content.size(), content.size());
    EXPECT_TRUE(downloaded_content == content);
}

} // namespace

// ArduPilot reports 100000 bytes for most @SYS files.
TEST(Ftp, DownloadFileSizeEstimateTooLarge)
{
    download_with_size_estimate(712, 100000, false);
}

TEST(Ftp, DownloadBurstFileSizeEstimateTooLarge)
{
    download_with_size_estimate(712, 100000, true);
}

// The rest of the last read according to the estimate is refused, because it is shorter
// than the reads before it.
TEST(Ftp, DownloadFileSizeEstimateSlightlyTooLarge)
{
    download_with_size_estimate(15110, 15110 + 100, false);
}

TEST(Ftp, DownloadBurstFileSizeEstimateSlightlyTooLarge)
{
    download_with_size_estimate(15110, 15110 + 100, true);
}

// ArduPilot's @PARAM/param.pck?withdefaults=1 with many parameters not at their default.
TEST(Ftp, DownloadFileSizeEstimateTooSmall)
{
    download_with_size_estimate(16595, 16440, false);
}

TEST(Ftp, DownloadBurstFileSizeEstimateTooSmall)
{
    download_with_size_estimate(16595, 16440, true);
}

// A burst of a single packet, after which the next request has the sequence number that
// the server would answer with the NAK(EOF) that ended the burst.
TEST(Ftp, DownloadBurstFileSizeEstimateSinglePacket)
{
    download_with_size_estimate(48, 1048576, true);
}
