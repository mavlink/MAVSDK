#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <string>

namespace mavsdk {

class CallbackTracker {
public:
    CallbackTracker();
    ~CallbackTracker() = default;

    void record_queued(const std::string& filename, int linenumber);
    void record_executed(const std::string& filename, int linenumber, int64_t duration_us);
    void maybe_print_stats(size_t queue_size);

private:
    struct LocationStats {
        int count{0};
        int64_t total_duration_us{0};
    };

    std::atomic<int> _total_callback_count{0};
    std::chrono::steady_clock::time_point _last_stats_time;
    std::map<std::string, LocationStats> _location_stats;
    std::mutex _stats_mutex;

    static constexpr int STATS_INTERVAL_SECONDS = 5;
    static constexpr int TOP_N_SOURCES = 10;
};

} // namespace mavsdk
