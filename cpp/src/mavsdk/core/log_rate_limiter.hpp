#pragma once

#include <chrono>
#include <mutex>

namespace mavsdk {

// For warnings that could otherwise be logged for every message: lets one through, then
// at most one per interval, and counts the ones held back in between.
class LogRateLimiter {
public:
    explicit LogRateLimiter(
        std::chrono::steady_clock::duration interval = std::chrono::seconds(5)) :
        _interval(interval)
    {}

    // Whether to log now. If so, suppressed is set to how many weren't logged since the last.
    bool should_log(unsigned& suppressed)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const auto now = std::chrono::steady_clock::now();
        if (_logged && now - _last < _interval) {
            ++_suppressed;
            return false;
        }
        suppressed = _suppressed;
        _suppressed = 0;
        _logged = true;
        _last = now;
        return true;
    }

    bool should_log()
    {
        unsigned suppressed;
        return should_log(suppressed);
    }

private:
    const std::chrono::steady_clock::duration _interval;
    std::mutex _mutex{};
    bool _logged{false};
    std::chrono::steady_clock::time_point _last{};
    unsigned _suppressed{0};
};

} // namespace mavsdk
