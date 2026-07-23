#include "mavlink_message_handler.hpp"
#include "mavlink_include.hpp"
#include <gtest/gtest.h>
#include <asio.hpp>
#include <atomic>
#include <chrono>
#include <thread>

using namespace mavsdk;

namespace {

void drain(asio::io_context& io)
{
    // Drain posted register/unregister work after a short yield for the posts.
    for (int i = 0; i < 50; ++i) {
        const auto n = io.poll();
        if (n == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            io.poll();
            return;
        }
    }
}

mavlink_message_t make_msg(uint32_t msgid, uint8_t compid)
{
    mavlink_message_t msg{};
    msg.msgid = msgid;
    msg.compid = compid;
    msg.sysid = 1;
    return msg;
}

} // namespace

TEST(MavlinkMessageHandler, RegisterProcessUnregister)
{
    asio::io_context io;
    MavlinkMessageHandler handler(io);

    std::atomic<int> hits{0};
    const int cookie_val = 7;
    const void* cookie = &cookie_val;

    handler.register_one(
        1, [&hits](const mavlink_message_t&) { hits.fetch_add(1); }, cookie);
    drain(io);

    handler.process_message(make_msg(1, 1));
    EXPECT_EQ(hits.load(), 1);

    // Different msg id must not fire
    handler.process_message(make_msg(2, 1));
    EXPECT_EQ(hits.load(), 1);

    handler.unregister_all(cookie);
    drain(io);

    handler.process_message(make_msg(1, 1));
    EXPECT_EQ(hits.load(), 1);
}

TEST(MavlinkMessageHandler, ComponentIdFilter)
{
    asio::io_context io;
    MavlinkMessageHandler handler(io);

    std::atomic<int> hits{0};
    const int cookie_val = 11;
    const void* cookie = &cookie_val;

    handler.register_one_with_component_id(
        42, 50, [&hits](const mavlink_message_t&) { hits.fetch_add(1); }, cookie);
    drain(io);

    handler.process_message(make_msg(42, 1)); // wrong component
    EXPECT_EQ(hits.load(), 0);

    handler.process_message(make_msg(42, 50)); // matching component
    EXPECT_EQ(hits.load(), 1);

    handler.unregister_one(42, cookie);
    drain(io);
    handler.process_message(make_msg(42, 50));
    EXPECT_EQ(hits.load(), 1);
}

TEST(MavlinkMessageHandler, UnregisterAllBlockingWhenStopped)
{
    asio::io_context io;
    MavlinkMessageHandler handler(io);

    std::atomic<int> hits{0};
    const int cookie_val = 3;
    const void* cookie = &cookie_val;

    handler.register_one(
        9, [&hits](const mavlink_message_t&) { hits.fetch_add(1); }, cookie);
    drain(io);
    handler.process_message(make_msg(9, 1));
    EXPECT_EQ(hits.load(), 1);

    io.stop();
    handler.unregister_all_blocking(cookie);

    // After unregister on stopped context, process should not fire even if we restart.
    io.restart();
    handler.process_message(make_msg(9, 1));
    EXPECT_EQ(hits.load(), 1);
}
