#include "mavsdk.hpp"
#include "plugins/action/action.hpp"
#include "mavlink_include.hpp"
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <memory>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

namespace {

// Answers TIMESYNC requests with a clock that starts at boot, like PX4 does, and acks every
// command.
class FakeAutopilot {
public:
    explicit FakeAutopilot(unsigned short groundstation_port) :
        _socket(_io_context, asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 0)),
        _groundstation(asio::ip::make_address("127.0.0.1"), groundstation_port)
    {
        receive();
        send_heartbeat();
        _thread = std::thread([this]() { _io_context.run(); });
    }

    ~FakeAutopilot()
    {
        _io_context.stop();
        _thread.join();
    }

    // Local time minus the autopilot's time, which is what MAVSDK should find.
    std::chrono::nanoseconds boot_time() const { return _boot.time_since_epoch(); }

    // 0 means: answer the component that asked.
    std::atomic<uint8_t> reply_target_component{0};

    // Holds back every answer by this long, like a slow link would.
    std::atomic<int> reply_delay_ms{0};

    std::atomic<int> timesync_replies{0};

private:
    int64_t now_ns() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now() - _boot)
            .count();
    }

    void send(const mavlink_message_t& message)
    {
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        const auto len = mavlink_msg_to_send_buffer(buffer, &message);
        _socket.send_to(asio::buffer(buffer, len), _groundstation);
    }

    void send_heartbeat()
    {
        mavlink_message_t message{};
        mavlink_msg_heartbeat_pack_chan(
            1, 1, channel, &message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4, 0, 0, MAV_STATE_ACTIVE);
        send(message);

        _heartbeat_timer.expires_after(std::chrono::milliseconds(200));
        _heartbeat_timer.async_wait([this](const asio::error_code& ec) {
            if (!ec) {
                send_heartbeat();
            }
        });
    }

    void receive()
    {
        _socket.async_receive_from(
            asio::buffer(_receive_buffer),
            _sender,
            [this](const asio::error_code& ec, std::size_t n) {
                if (ec) {
                    return;
                }
                for (std::size_t i = 0; i < n; ++i) {
                    mavlink_message_t message{};
                    if (mavlink_parse_char(channel - 1, _receive_buffer[i], &message, &_status)) {
                        handle(message);
                    }
                }
                receive();
            });
    }

    void send_delayed(const mavlink_message_t& message, std::function<void()> on_sent = {})
    {
        auto timer = std::make_shared<asio::steady_timer>(
            _io_context, std::chrono::milliseconds(reply_delay_ms.load()));
        timer->async_wait([this, timer, message, on_sent](const asio::error_code& ec) {
            if (!ec) {
                send(message);
                if (on_sent) {
                    on_sent();
                }
            }
        });
    }

    void handle(const mavlink_message_t& message)
    {
        if (message.msgid == MAVLINK_MSG_ID_COMMAND_LONG) {
            mavlink_command_long_t command{};
            mavlink_msg_command_long_decode(&message, &command);
            mavlink_message_t ack{};
            mavlink_msg_command_ack_pack_chan(
                1,
                1,
                channel,
                &ack,
                command.command,
                MAV_RESULT_ACCEPTED,
                0,
                0,
                message.sysid,
                message.compid);
            send_delayed(ack);
            return;
        }

        if (message.msgid != MAVLINK_MSG_ID_TIMESYNC) {
            return;
        }
        mavlink_timesync_t request{};
        mavlink_msg_timesync_decode(&message, &request);
        if (request.tc1 != 0) {
            return;
        }
        const uint8_t target = reply_target_component;
        mavlink_message_t reply{};
        mavlink_msg_timesync_pack_chan(
            1,
            1,
            channel,
            &reply,
            now_ns(),
            request.ts1,
            message.sysid,
            target != 0 ? target : message.compid);
        send_delayed(reply, [this]() { ++timesync_replies; });
    }

    static constexpr uint8_t channel = MAVLINK_COMM_NUM_BUFFERS - 1;

    const std::chrono::system_clock::time_point _boot{
        std::chrono::system_clock::now() - std::chrono::seconds(100)};
    asio::io_context _io_context;
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _groundstation;
    asio::steady_timer _heartbeat_timer{_io_context};
    std::array<uint8_t, 2048> _receive_buffer{};
    asio::ip::udp::endpoint _sender;
    mavlink_status_t _status{};
    std::thread _thread;
};

} // namespace

TEST(Timesync, StatusReportsOffset)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("udpin://127.0.0.1:17130"), ConnectionResult::Success);
    FakeAutopilot autopilot{17130};

    auto system = mavsdk.first_autopilot(5.0);
    ASSERT_TRUE(system);
    EXPECT_FALSE(system.value()->timesync_status().synced);

    auto prom = std::promise<System::TimesyncStatus>{};
    auto fut = prom.get_future();
    std::atomic<bool> done{false};
    auto handle = system.value()->subscribe_timesync_status([&](System::TimesyncStatus status) {
        if (status.synced && !done.exchange(true)) {
            prom.set_value(status);
        }
    });

    system.value()->enable_timesync();

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    const auto reported = fut.get();
    system.value()->unsubscribe_timesync_status(handle);

    const auto expected_offset_ns = -autopilot.boot_time().count();
    EXPECT_GT(reported.rtt_ns, 0);
    EXPECT_LT(reported.rtt_ns, 10'000'000);
    // Within half the round trip, plus some slack for the scheduler.
    EXPECT_LT(std::llabs(reported.offset_ns - expected_offset_ns), 10'000'000);

    const auto status = system.value()->timesync_status();
    EXPECT_TRUE(status.synced);
    EXPECT_LT(std::llabs(status.offset_ns - expected_offset_ns), 10'000'000);
}

TEST(Timesync, IgnoresRepliesForOtherComponents)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("udpin://127.0.0.1:17131"), ConnectionResult::Success);
    FakeAutopilot autopilot{17131};
    // Another component on our system ID asked.
    autopilot.reply_target_component = 99;

    auto system = mavsdk.first_autopilot(5.0);
    ASSERT_TRUE(system);

    std::atomic<int> updates{0};
    auto handle =
        system.value()->subscribe_timesync_status([&](System::TimesyncStatus) { ++updates; });
    system.value()->enable_timesync();

    // The first request goes out right away.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(updates, 0);
    EXPECT_FALSE(system.value()->timesync_status().synced);

    // Once it answers us, the next request (5 s later) syncs.
    autopilot.reply_target_component = 0;
    for (int i = 0; i < 60 && !system.value()->timesync_status().synced; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_TRUE(system.value()->timesync_status().synced);
    EXPECT_GT(updates, 0);

    system.value()->unsubscribe_timesync_status(handle);
}

TEST(Timesync, SlowLinkStretchesTimeout)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("udpin://127.0.0.1:17132"), ConnectionResult::Success);
    FakeAutopilot autopilot{17132};
    // Longer than a command takes to time out with the default 0.5 s: four attempts.
    autopilot.reply_delay_ms = 3000;

    auto system = mavsdk.first_autopilot(5.0);
    ASSERT_TRUE(system);

    // The RTT is measured without enable_timesync(). Requests go out every 5 s.
    for (int i = 0; i < 150 && autopilot.timesync_replies == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_GT(autopilot.timesync_replies, 0);
    // Let the answer arrive.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_FALSE(system.value()->timesync_status().synced);

    Action action{system.value()};
    EXPECT_EQ(action.arm(), Action::Result::Success);
}
