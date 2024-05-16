#pragma once

#include "mavsdk_time.h"

#include <cstdint>
#include <mutex>
#include <memory>
#include <functional>
#include <vector>

namespace mavsdk {

class TimeoutHandler {
public:
    explicit TimeoutHandler(Time& time);
    ~TimeoutHandler() = default;

    // delete copy and move constructors and assign operators
    TimeoutHandler(TimeoutHandler const&) = delete; // Copy construct
    TimeoutHandler(TimeoutHandler&&) = delete; // Move construct
    TimeoutHandler& operator=(TimeoutHandler const&) = delete; // Copy assign
    TimeoutHandler& operator=(TimeoutHandler&&) = delete; // Move assign

    void add(std::function<void()> callback, double duration_s, void** cookie);
    void refresh(const void* cookie);
    void remove(const void* cookie);

    void run_once();

private:
    struct Timeout {
        std::function<void()> callback{};
        SteadyTimePoint time{};
        double duration_s{0.0};
        uint64_t cookie{0};
    };

    std::vector<Timeout> _timeouts{};
    std::mutex _timeouts_mutex{};
    bool _iterator_invalidated{false};

    Time& _time;

    uint64_t _next_cookie{1};
};

} // namespace mavsdk
