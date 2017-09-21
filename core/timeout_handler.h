#pragma once

#include <mutex>
#include <memory>
#include <functional>
#include <map>
#include "global_include.h"

namespace dronecore {

class TimeoutHandler
{
public:
    TimeoutHandler();
    ~TimeoutHandler();

    // delete copy and move constructors and assign operators
    TimeoutHandler(TimeoutHandler const &) = delete;            // Copy construct
    TimeoutHandler(TimeoutHandler &&) = delete;                 // Move construct
    TimeoutHandler &operator=(TimeoutHandler const &) = delete; // Copy assign
    TimeoutHandler &operator=(TimeoutHandler &&) = delete;      // Move assign

    void add(std::function<void()> callback, double duration_s, void **cookie);
    void refresh(const void *cookie);
    void update(double new_duration_s, const void *cookie);
    void remove(const void *cookie);

    void run_once();

private:
    struct Timeout {
        std::function<void()> callback;
        dl_time_t start_time;
        double duration_s;
    };

    std::map<void *, std::shared_ptr<Timeout>> _timeouts {};
    std::mutex _timeouts_mutex {};
};

} // namespace dronecore
