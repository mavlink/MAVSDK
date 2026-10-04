#include "mavsdk.hpp"
#include "plugins/log_files/log_files.hpp"
#include "fake_log_autopilot.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iterator>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;
namespace fs = std::filesystem;

static constexpr size_t MOCK_LOG_SIZE = 300 * 1024;

static std::vector<uint8_t> read_file(const fs::path& path)
{
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

// Unique per run so parallel test runs don't collide; removed on scope exit.
class TempLogPath {
public:
    explicit TempLogPath(const std::string& name) :
        _path(fs::temp_directory_path() / ("mavsdk_" + name + "_" + unique_suffix() + ".ulg"))
    {
        fs::remove(_path);
    }
    ~TempLogPath()
    {
        std::error_code ec;
        fs::remove(_path, ec);
    }
    TempLogPath(const TempLogPath&) = delete;
    TempLogPath& operator=(const TempLogPath&) = delete;

    const fs::path& path() const { return _path; }
    std::string str() const { return _path.string(); }

private:
    static std::string unique_suffix()
    {
        return std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    }

    fs::path _path;
};

template<typename Predicate>
static bool wait_until(Predicate predicate, std::chrono::seconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (!predicate()) {
        if (std::chrono::steady_clock::now() > deadline) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true;
}

// Ground station + fake autopilot connected over localhost, with the log list fetched.
struct LogFilesFixture {
    explicit LogFilesFixture(int port, std::vector<size_t> log_sizes = {MOCK_LOG_SIZE}) :
        fake(mavsdk_autopilot, log_sizes)
    {
        const auto port_str = std::to_string(port);
        EXPECT_EQ(
            mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:" + port_str),
            ConnectionResult::Success);
        EXPECT_EQ(
            mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:" + port_str),
            ConnectionResult::Success);

        auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
        EXPECT_TRUE(maybe_system);
        if (!maybe_system) {
            return;
        }
        log_files.emplace(maybe_system.value());

        // The plugin sends LOG_REQUEST_END on init; wait for it before counting.
        EXPECT_TRUE(
            wait_until([this]() { return fake.end_requests() >= 1; }, std::chrono::seconds(5)));

        auto [result, all_entries] = log_files->get_entries();
        EXPECT_EQ(result, LogFiles::Result::Success);
        EXPECT_EQ(all_entries.size(), log_sizes.size());
        entries = all_entries;
        if (!entries.empty()) {
            entry = entries[0];
        }

        fake.reset_counters();
    }

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    FakeLogAutopilot fake;
    std::optional<LogFiles> log_files;
    std::vector<LogFiles::Entry> entries;
    LogFiles::Entry entry{}; // entries[0]
};

// Records every download callback in order, and resolves a future on the first final result.
struct DownloadObserver {
    struct State {
        std::mutex mutex;
        std::vector<LogFiles::Result> events;
        std::promise<LogFiles::Result> final_prom;
        bool final_seen{false};
    };
    std::shared_ptr<State> state = std::make_shared<State>();
    std::future<LogFiles::Result> final_fut = state->final_prom.get_future();

    LogFiles::DownloadLogFileCallback callback(std::function<void()> on_first_progress = {})
    {
        auto s = state;
        return [s, on_first_progress](LogFiles::Result result, LogFiles::ProgressData) {
            bool first_progress = false;
            {
                std::lock_guard<std::mutex> lock(s->mutex);
                s->events.push_back(result);
                if (result == LogFiles::Result::Next) {
                    first_progress =
                        std::count(s->events.begin(), s->events.end(), LogFiles::Result::Next) == 1;
                } else if (!s->final_seen) {
                    s->final_seen = true;
                    s->final_prom.set_value(result);
                }
            }
            if (first_progress && on_first_progress) {
                on_first_progress();
            }
        };
    }

    std::vector<LogFiles::Result> events() const
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        return state->events;
    }

    unsigned final_count() const
    {
        const auto all = events();
        return static_cast<unsigned>(
            std::count_if(all.begin(), all.end(), [](LogFiles::Result result) {
                return result != LogFiles::Result::Next;
            }));
    }
};

TEST(LogFiles, DownloadMockLog)
{
    LogFilesFixture fixture{17060};
    ASSERT_TRUE(fixture.log_files);
    EXPECT_EQ(fixture.entry.size_bytes, fixture.fake.log().size());

    const TempLogPath path{"download_mock"};
    DownloadObserver observer;
    fixture.log_files->download_log_file_async(fixture.entry, path.str(), observer.callback());

    ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Success);
    EXPECT_EQ(read_file(path.path()), fixture.fake.log());
}

TEST(LogFiles, CancelDuringDownload)
{
    LogFilesFixture fixture{17061};
    ASSERT_TRUE(fixture.log_files);
    // Slow enough that cancel lands mid-transfer. The fake ignores LOG_REQUEST_END and keeps
    // streaming, so late LOG_DATA is guaranteed to arrive after the cancel.
    fixture.fake.packet_delay = std::chrono::microseconds(500);
    fixture.fake.ignore_end = true;

    const TempLogPath path{"cancel_during"};
    DownloadObserver observer;
    auto& log_files = *fixture.log_files;
    fixture.log_files->download_log_file_async(
        fixture.entry, path.str(), observer.callback([&log_files]() {
            EXPECT_EQ(log_files.cancel_download_log_file(), LogFiles::Result::Success);
        }));

    ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Cancelled);

    // Let a good number of late packets go through the plugin.
    ASSERT_TRUE(wait_until(
        [&]() { return fixture.fake.packets_after_end >= 100; }, std::chrono::seconds(10)));

    // Late data caused no further callbacks and did not recreate the file. (A closed file
    // already rejects writes, so this cannot tell whether the !active guard itself ran.)
    const auto events = observer.events();
    ASSERT_FALSE(events.empty());
    EXPECT_EQ(events.back(), LogFiles::Result::Cancelled);
    EXPECT_EQ(observer.final_count(), 1u);
    EXPECT_GE(fixture.fake.end_requests(), 1u);
    EXPECT_FALSE(fs::exists(path.path()));
}

TEST(LogFiles, CancelStalledDownload)
{
    LogFilesFixture fixture{17062};
    ASSERT_TRUE(fixture.log_files);
    fixture.fake.stop_after_bytes = 100 * 1024;

    const TempLogPath path{"cancel_stalled"};
    DownloadObserver observer;
    fixture.log_files->download_log_file_async(fixture.entry, path.str(), observer.callback());

    // Wait until the vehicle has gone silent and MAVSDK is retrying.
    ASSERT_TRUE(wait_until(
        [&]() { return fixture.fake.data_requests_after_stop > 0; }, std::chrono::seconds(10)));

    EXPECT_EQ(fixture.log_files->cancel_download_log_file(), LogFiles::Result::Success);

    ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Cancelled);

    // Retries must stop.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    const auto retries_after_cancel = fixture.fake.data_requests_after_stop.load();
    std::this_thread::sleep_for(std::chrono::seconds(3));

    EXPECT_EQ(fixture.fake.data_requests_after_stop.load(), retries_after_cancel);
    EXPECT_EQ(observer.events().back(), LogFiles::Result::Cancelled);
    EXPECT_EQ(observer.final_count(), 1u);
    EXPECT_GE(fixture.fake.end_requests(), 1u);
    EXPECT_FALSE(fs::exists(path.path()));
}

TEST(LogFiles, CancelWhenIdle)
{
    LogFilesFixture fixture{17063};
    ASSERT_TRUE(fixture.log_files);

    EXPECT_EQ(fixture.log_files->cancel_download_log_file(), LogFiles::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(fixture.fake.end_requests(), 0u);
}

TEST(LogFiles, DownloadAfterCancel)
{
    LogFilesFixture fixture{17064};
    ASSERT_TRUE(fixture.log_files);
    fixture.fake.packet_delay = std::chrono::microseconds(500);

    const TempLogPath path{"after_cancel"};
    {
        DownloadObserver observer;
        auto& log_files = *fixture.log_files;
        fixture.log_files->download_log_file_async(
            fixture.entry, path.str(), observer.callback([&log_files]() {
                log_files.cancel_download_log_file();
            }));
        ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
        EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Cancelled);
    }

    fixture.fake.packet_delay = std::chrono::microseconds(0);

    DownloadObserver observer;
    fixture.log_files->download_log_file_async(fixture.entry, path.str(), observer.callback());
    ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Success);
    EXPECT_EQ(read_file(path.path()), fixture.fake.log());
}

TEST(LogFiles, DownloadAfterCancelWhileStreaming)
{
    // Log 0 is cancelled and keeps streaming; log 1 has a different id, size and content.
    LogFilesFixture fixture{17066, {MOCK_LOG_SIZE, 200 * 1024}};
    ASSERT_TRUE(fixture.log_files);
    ASSERT_EQ(fixture.entries.size(), 2u);
    const auto& first_entry = fixture.entries[0];
    const auto& second_entry = fixture.entries[1];
    ASSERT_NE(first_entry.id, second_entry.id);
    ASSERT_NE(fixture.fake.log(first_entry.id), fixture.fake.log(second_entry.id));

    fixture.fake.packet_delay = std::chrono::microseconds(500);
    fixture.fake.ignore_end = true;

    const TempLogPath cancelled_path{"streaming_cancelled"};
    {
        DownloadObserver observer;
        auto& log_files = *fixture.log_files;
        fixture.log_files->download_log_file_async(
            first_entry, cancelled_path.str(), observer.callback([&log_files]() {
                log_files.cancel_download_log_file();
            }));
        ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
        EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Cancelled);
    }

    // Log 0 is now streaming late LOG_DATA, wrapping through the file.
    ASSERT_TRUE(wait_until(
        [&]() { return fixture.fake.packets_after_end >= 100; }, std::chrono::seconds(10)));
    fixture.fake.packet_delay = std::chrono::microseconds(0);

    // Same entry again, while its late packets keep arriving.
    const TempLogPath same_path{"streaming_same"};
    {
        DownloadObserver observer;
        fixture.log_files->download_log_file_async(
            first_entry, same_path.str(), observer.callback());
        ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
        EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Success);
        EXPECT_EQ(read_file(same_path.path()), fixture.fake.log(first_entry.id));
    }

    // A different entry while log 0 is still streaming: none of its packets may end up in it.
    const auto streamed_before = fixture.fake.packets_after_end.load();
    const TempLogPath other_path{"streaming_other"};
    {
        DownloadObserver observer;
        fixture.log_files->download_log_file_async(
            second_entry, other_path.str(), observer.callback());
        ASSERT_EQ(observer.final_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
        EXPECT_EQ(observer.final_fut.get(), LogFiles::Result::Success);
        EXPECT_EQ(read_file(other_path.path()), fixture.fake.log(second_entry.id));
    }
    // Late packets for log 0 really were arriving during the download of log 1.
    EXPECT_GT(fixture.fake.packets_after_end.load(), streamed_before);
}

TEST(LogFiles, SecondDownloadWhileActiveIsRejected)
{
    LogFilesFixture fixture{17065};
    ASSERT_TRUE(fixture.log_files);
    fixture.fake.packet_delay = std::chrono::microseconds(500);

    const TempLogPath path{"first"};
    const TempLogPath second_path{"second"};

    DownloadObserver first;
    fixture.log_files->download_log_file_async(fixture.entry, path.str(), first.callback());

    DownloadObserver second;
    fixture.log_files->download_log_file_async(fixture.entry, second_path.str(), second.callback());
    ASSERT_EQ(second.final_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(second.final_fut.get(), LogFiles::Result::InvalidArgument);
    EXPECT_FALSE(fs::exists(second_path.path()));

    // The first download is unaffected.
    fixture.fake.packet_delay = std::chrono::microseconds(0);
    ASSERT_EQ(first.final_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(first.final_fut.get(), LogFiles::Result::Success);
    EXPECT_EQ(read_file(path.path()), fixture.fake.log());
}
