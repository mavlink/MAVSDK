#pragma once

#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include "mavsdk_time.h"

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
    };

    std::unordered_map<void*, std::shared_ptr<Timeout>> _timeouts{};
    std::mutex _timeouts_mutex{};
    bool _iterator_invalidated{false};

    Time& _time;
};

} // namespace mavsdk
