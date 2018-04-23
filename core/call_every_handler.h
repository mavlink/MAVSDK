#pragma once

#include <mutex>
#include <memory>
#include <functional>
#include <map>
#include "global_include.h"

namespace dronecore {

class CallEveryHandler
{
public:
    CallEveryHandler(Time &time);
    ~CallEveryHandler();

    // delete copy and move constructors and assign operators
    CallEveryHandler(CallEveryHandler const &) = delete;            // Copy construct
    CallEveryHandler(CallEveryHandler &&) = delete;                 // Move construct
    CallEveryHandler &operator=(CallEveryHandler const &) = delete; // Copy assign
    CallEveryHandler &operator=(CallEveryHandler &&) = delete;      // Move assign

    void add(std::function<void()> callback, float interval_s, void **cookie);
    void change(float interval_s, const void *cookie);
    void reset(const void *cookie);
    void remove(const void *cookie);

    void run_once();

private:
    struct Entry {
        std::function<void()> callback;
        dl_time_t last_time;
        float interval_s;
    };

    std::map<void *, std::shared_ptr<Entry>> _entries {};
    std::mutex _entries_mutex {};
    bool _iterator_invalidated {false};

    Time &_time;
};

} // namespace dronecore
