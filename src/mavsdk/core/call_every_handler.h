#pragma once

#include <cstdint>
#include <mutex>
#include <memory>
#include <functional>
#include <vector>
#include "mavsdk_time.h"

namespace mavsdk {

class CallEveryHandler {
public:
    explicit CallEveryHandler(Time& time);
    ~CallEveryHandler() = default;

    // delete copy and move constructors and assign operators
    CallEveryHandler(CallEveryHandler const&) = delete; // Copy construct
    CallEveryHandler(CallEveryHandler&&) = delete; // Move construct
    CallEveryHandler& operator=(CallEveryHandler const&) = delete; // Copy assign
    CallEveryHandler& operator=(CallEveryHandler&&) = delete; // Move assign

    using Cookie = uint64_t;

    Cookie add(std::function<void()> callback, double interval_s);
    void change(double interval_s, Cookie cookie);
    void reset(Cookie cookie);
    void remove(Cookie cookie);

    void run_once();

private:
    struct Entry {
        std::function<void()> callback{nullptr};
        SteadyTimePoint last_time{};
        double interval_s{0.0};
        Cookie cookie{};
    };

    std::vector<Entry> _entries{};
    std::mutex _entries_mutex{};
    bool _iterator_invalidated{false};

    Time& _time;

    Cookie _next_cookie{1};
};

} // namespace mavsdk
