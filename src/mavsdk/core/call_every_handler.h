#pragma once

#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>
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

    void add(std::function<void()> callback, double interval_s, void** cookie);
    void change(double interval_s, const void* cookie);
    void reset(const void* cookie);
    void remove(const void* cookie);

    void run_once();

private:
    struct Entry {
        std::function<void()> callback{nullptr};
        SteadyTimePoint last_time{};
        double interval_s{0.0};
    };

    std::unordered_map<void*, std::shared_ptr<Entry>> _entries{};
    std::mutex _entries_mutex{};
    bool _iterator_invalidated{false};

    Time& _time;
};

} // namespace mavsdk
