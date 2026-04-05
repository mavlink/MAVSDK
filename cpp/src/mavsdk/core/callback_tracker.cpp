#include "callback_tracker.hpp"
#include "log.hpp"
#include <algorithm>
#include <vector>

namespace mavsdk {

CallbackTracker::CallbackTracker() : _last_stats_time(std::chrono::steady_clock::now()) {}

void CallbackTracker::record_queued(const std::string& filename, int linenumber)
{
    if (filename.empty()) {
        return;
    }

    _total_callback_count++;

    std::lock_guard<std::mutex> lock(_stats_mutex);
    std::string location = filename + ":" + std::to_string(linenumber);
    _location_stats[location].count++;
}

void CallbackTracker::record_executed(
    const std::string& filename, int linenumber, int64_t duration_us)
{
    if (filename.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(_stats_mutex);
    std::string location = filename + ":" + std::to_string(linenumber);
    _location_stats[location].total_duration_us += duration_us;
}

void CallbackTracker::maybe_print_stats(size_t queue_size)
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _last_stats_time).count();

    if (elapsed < STATS_INTERVAL_SECONDS) {
        return;
    }

    // Calculate callbacks per second
    int callbacks_per_sec = _total_callback_count.load() / elapsed;

    LogInfo(
        "Callback stats: {} callbacks/sec (avg), queue size: {}",
        static_cast<int>(callbacks_per_sec),
        static_cast<int>(queue_size));

    // Get a snapshot of the stats and clear them
    std::map<std::string, LocationStats> stats_snapshot;
    int64_t total_duration_us = 0;
    {
        std::lock_guard<std::mutex> lock(_stats_mutex);
        stats_snapshot = _location_stats;
        _location_stats.clear();

        // Calculate total duration across all callbacks
        for (const auto& [location, stats] : stats_snapshot) {
            total_duration_us += stats.total_duration_us;
        }
    }

    if (!stats_snapshot.empty()) {
        LogInfo("Top callback sources (by time):");

        // Convert map to vector and sort by total duration
        std::vector<std::pair<std::string, LocationStats>> sorted_stats(
            stats_snapshot.begin(), stats_snapshot.end());
        std::sort(sorted_stats.begin(), sorted_stats.end(), [](const auto& a, const auto& b) {
            return a.second.total_duration_us > b.second.total_duration_us;
        });

        // Print top N sources with count, duration, and percentage
        int count = 0;
        for (const auto& [location, stats] : sorted_stats) {
            if (count++ >= TOP_N_SOURCES) {
                break;
            }

            int avg_duration_us = static_cast<int>(stats.total_duration_us / stats.count);
            int percentage_int = 0;
            if (total_duration_us > 0) {
                percentage_int =
                    static_cast<int>((stats.total_duration_us * 100) / total_duration_us);
            }

            LogInfo(
                "  {}: {} calls, {} us total, {} us avg, {}% of time",
                location,
                static_cast<int>(stats.count),
                static_cast<int>(stats.total_duration_us),
                static_cast<int>(avg_duration_us),
                static_cast<int>(percentage_int));
        }

        // Print total time spent in callbacks
        int cpu_percentage = static_cast<int>((total_duration_us / 10000) / elapsed);
        LogInfo(
            "Total time in callbacks: {} us ({} ms) over {} seconds ({}% CPU)",
            static_cast<int>(total_duration_us),
            static_cast<int>(total_duration_us / 1000),
            static_cast<int>(elapsed),
            static_cast<int>(cpu_percentage));
    }

    // Reset counters
    _total_callback_count = 0;
    _last_stats_time = now;
}

} // namespace mavsdk
