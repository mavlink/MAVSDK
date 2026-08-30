#pragma once

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <functional>
#include <list>
#include <thread>
#include "mavsdk_time.hpp"
#include "mavsdk_export.h"

namespace mavsdk {

// Recurring callbacks, driven by run_once() from the io_context thread.
//
// Threading: add/change/reset/remove are callable from any thread. Callbacks are invoked by
// run_once() outside the lock, so a callback may call back into the handler.
//
// Cancellation: remove() cancels an entry even if run_once() has already found it due but
// has not invoked it yet. It does not wait for a callback that is running right now -- use
// remove_blocking() for that, which is what an owner about to be destroyed needs. See the
// comments on those two functions.
class MAVSDK_TEST_EXPORT CallEveryHandler {
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

    // Stop a recurring callback. After this returns it will not be started anymore, but if
    // it is running right now it keeps running. Safe to call from within a callback and
    // safe to call while holding a lock that the callback also takes.
    void remove(Cookie cookie);

    // Stop a recurring callback and wait for it if it happens to be running right now. Once
    // this returns, the callback is neither running nor going to run, so an owner that
    // captured 'this' can be destroyed safely.
    //
    // Must not be called while holding a lock that the callback itself takes -- that would
    // deadlock. Calling it from within the very callback being removed is fine (it does not
    // wait on itself), as is calling it for a different cookie from within a callback.
    void remove_blocking(Cookie cookie);

    void run_once();

private:
    void remove_impl(Cookie cookie, bool blocking);

    struct Entry {
        std::function<void()> callback{nullptr};
        SteadyTimePoint last_time{};
        double interval_s{0.0};
        Cookie cookie{};
    };

    std::mutex _mutex{};
    std::list<Entry> _entries{};
    // Cookies that run_once() has found due but has not invoked yet, in order. Kept so that
    // remove() can still cancel them before they fire.
    std::list<Cookie> _due{};

    // Signalled whenever a callback returns, so remove_blocking() can wait for one.
    std::condition_variable _callback_done{};

    // The entry whose callback run_once() is invoking right now (0 if none), and the thread
    // invoking it so that removing an entry from its own callback does not wait on itself.
    // Guarded by _mutex.
    Cookie _executing_cookie{0};
    std::thread::id _executing_thread{};

    Time& _time;

    Cookie _next_cookie{1};
};

} // namespace mavsdk
