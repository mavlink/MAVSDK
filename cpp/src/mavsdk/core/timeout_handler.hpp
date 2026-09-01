#pragma once

#include "mavsdk_time.hpp"
#include "mavsdk_export.h"

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <functional>
#include <list>
#include <thread>

namespace mavsdk {

// One-shot timeouts, driven by run_once() from the io_context thread.
//
// Threading: add/refresh/remove are callable from any thread. Callbacks are invoked by
// run_once() outside the lock, so a callback may call back into the handler.
//
// Cancellation: remove() cancels a timeout even if run_once() has already picked it up as
// due but has not invoked it yet. It does not wait for a callback that is running right
// now -- use remove_blocking() for that, which is what an owner about to be destroyed
// needs. See the comments on those two functions.
class MAVSDK_TEST_EXPORT TimeoutHandler {
public:
    explicit TimeoutHandler(Time& time);
    ~TimeoutHandler() = default;

    // delete copy and move constructors and assign operators
    TimeoutHandler(TimeoutHandler const&) = delete; // Copy construct
    TimeoutHandler(TimeoutHandler&&) = delete; // Move construct
    TimeoutHandler& operator=(TimeoutHandler const&) = delete; // Copy assign
    TimeoutHandler& operator=(TimeoutHandler&&) = delete; // Move assign

    using Cookie = uint64_t;

    [[nodiscard]] Cookie add(std::function<void()> callback, double duration_s);
    void refresh(Cookie cookie);

    // Cancel a timeout. After this returns the callback will not be started anymore, but if
    // it is running right now it keeps running. Safe to call from within a callback and
    // safe to call while holding a lock that the callback also takes.
    void remove(Cookie cookie);

    // Cancel a timeout and wait for it if it happens to be running right now. Once this
    // returns, the callback is neither running nor going to run, so an owner that captured
    // 'this' can be destroyed safely.
    //
    // Must not be called while holding a lock that the callback itself takes -- that would
    // deadlock. Calling it from within the very callback being removed is fine (it does not
    // wait on itself), as is calling it for a different cookie from within a callback.
    void remove_blocking(Cookie cookie);

    void run_once();

private:
    void remove_impl(Cookie cookie, bool blocking);

    struct Timeout {
        std::function<void()> callback{};
        SteadyTimePoint time{};
        double duration_s{0.0};
        Cookie cookie{0};
    };

    std::list<Timeout> _timeouts{};
    // Timeouts that run_once() has found due and taken out of _timeouts, but has not
    // invoked yet. They stay reachable here so that remove() can still cancel them.
    std::list<Timeout> _due{};

    std::mutex _timeouts_mutex{};
    // Signalled whenever a callback returns, so remove_blocking() can wait for one.
    std::condition_variable _callback_done{};

    // The timeout whose callback run_once() is invoking right now (0 if none), and the
    // thread invoking it so that removing a timeout from its own callback does not wait
    // on itself. Guarded by _timeouts_mutex.
    Cookie _executing_cookie{0};
    std::thread::id _executing_thread{};

    Time& _time;

    Cookie _next_cookie{1};
};

} // namespace mavsdk
