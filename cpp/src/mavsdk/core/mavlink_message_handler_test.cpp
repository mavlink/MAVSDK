#include "mavlink_message_handler.hpp"
#include "mavlink_include.hpp"
#include <gtest/gtest.h>
#include <asio.hpp>
#include <atomic>
#include <chrono>
#include <future>
#include <thread>

using namespace mavsdk;

namespace {

// Run io_context on a background thread so register/unregister posts complete
// under the same model as production (io-thread table ownership).
class IoRunner {
public:
    IoRunner() : _work(asio::make_work_guard(_io)), _thread([this]() { _io.run(); }) {}

    ~IoRunner()
    {
        _work.reset();
        _io.stop();
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    asio::io_context& io() { return _io; }

    // Wait until all currently queued handlers have run.
    void sync()
    {
        std::promise<void> done;
        auto fut = done.get_future();
        asio::post(_io, [&done]() { done.set_value(); });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    // Execute fn on the io thread and wait.
    template<typename Fn> void on_io(Fn&& fn)
    {
        std::promise<void> done;
        auto fut = done.get_future();
        asio::post(_io, [&done, fn = std::forward<Fn>(fn)]() mutable {
            fn();
            done.set_value();
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

private:
    asio::io_context _io;
    asio::executor_work_guard<asio::io_context::executor_type> _work;
    std::thread _thread;
};

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
    IoRunner runner;
    MavlinkMessageHandler handler(runner.io());

    std::atomic<int> hits{0};
    const int cookie_val = 7;
    const void* cookie = &cookie_val;
    // Avoid low system ids that other subsystem tests might saturate in logs.
    constexpr uint32_t kMsgId = 1500;

    handler.register_one(
        static_cast<uint16_t>(kMsgId),
        [&hits](const mavlink_message_t&) { hits.fetch_add(1); },
        cookie);
    runner.sync();

    runner.on_io([&]() { handler.process_message(make_msg(kMsgId, 1)); });
    EXPECT_EQ(hits.load(), 1);

    // Different msg id must not fire
    runner.on_io([&]() { handler.process_message(make_msg(kMsgId + 1, 1)); });
    EXPECT_EQ(hits.load(), 1);

    handler.unregister_all(cookie);
    runner.sync();

    runner.on_io([&]() { handler.process_message(make_msg(kMsgId, 1)); });
    EXPECT_EQ(hits.load(), 1);
}

TEST(MavlinkMessageHandler, ComponentIdFilter)
{
    IoRunner runner;
    MavlinkMessageHandler handler(runner.io());

    std::atomic<int> hits{0};
    const int cookie_val = 11;
    const void* cookie = &cookie_val;
    constexpr uint32_t kMsgId = 1501;
    constexpr uint8_t kComp = 50;

    handler.register_one_with_component_id(
        static_cast<uint16_t>(kMsgId),
        kComp,
        [&hits](const mavlink_message_t&) { hits.fetch_add(1); },
        cookie);
    runner.sync();

    runner.on_io([&]() { handler.process_message(make_msg(kMsgId, 1)); }); // wrong component
    EXPECT_EQ(hits.load(), 0);

    runner.on_io([&]() { handler.process_message(make_msg(kMsgId, kComp)); }); // match
    EXPECT_EQ(hits.load(), 1);

    handler.unregister_one(static_cast<uint16_t>(kMsgId), cookie);
    runner.sync();
    runner.on_io([&]() { handler.process_message(make_msg(kMsgId, kComp)); });
    EXPECT_EQ(hits.load(), 1);
}

TEST(MavlinkMessageHandler, UnregisterAllBlockingWhenStopped)
{
    asio::io_context io;
    MavlinkMessageHandler handler(io);

    std::atomic<int> hits{0};
    const int cookie_val = 3;
    const void* cookie = &cookie_val;
    constexpr uint32_t kMsgId = 1502;

    handler.register_one(
        static_cast<uint16_t>(kMsgId),
        [&hits](const mavlink_message_t&) { hits.fetch_add(1); },
        cookie);

    // Drain the register post on this thread (same pattern as non-threaded tests).
    for (int i = 0; i < 50; ++i) {
        if (io.poll() > 0) {
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        io.poll();
        break;
    }

    handler.process_message(make_msg(kMsgId, 1));
    EXPECT_EQ(hits.load(), 1);

    io.stop();
    handler.unregister_all_blocking(cookie);

    // After unregister on stopped context, process should not fire even if we restart.
    io.restart();
    handler.process_message(make_msg(kMsgId, 1));
    EXPECT_EQ(hits.load(), 1);
}
