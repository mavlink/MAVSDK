#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include "mavsdk_export.h"

namespace mavsdk {

using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using SystemTimePoint = std::chrono::time_point<std::chrono::system_clock>;
using AutopilotTimePoint = std::chrono::time_point<std::chrono::system_clock>;

class MAVSDK_TEST_EXPORT Time {
public:
    Time() = default;
    virtual ~Time() = default;

    virtual SteadyTimePoint steady_time();
    virtual SystemTimePoint system_time();
    double elapsed_s();
    double elapsed_since_s(const SteadyTimePoint& since);
    uint64_t elapsed_ms() const;
    uint64_t elapsed_us() const;
    SteadyTimePoint steady_time_in_future(double duration_s);
    static void shift_steady_time_by(SteadyTimePoint& time, double offset_s);

    virtual void sleep_for(std::chrono::hours h);
    virtual void sleep_for(std::chrono::minutes m);
    virtual void sleep_for(std::chrono::seconds s);
    virtual void sleep_for(std::chrono::milliseconds ms);
    virtual void sleep_for(std::chrono::microseconds us);
    virtual void sleep_for(std::chrono::nanoseconds ns);
};

class MAVSDK_TEST_EXPORT FakeTime : public Time {
public:
    FakeTime();
    ~FakeTime() override = default;
    SteadyTimePoint steady_time() override;
    void sleep_for(std::chrono::hours h) override;
    void sleep_for(std::chrono::minutes m) override;
    void sleep_for(std::chrono::seconds s) override;
    void sleep_for(std::chrono::milliseconds ms) override;
    void sleep_for(std::chrono::microseconds us) override;
    void sleep_for(std::chrono::nanoseconds ns) override;

private:
    // Atomic so that one thread can advance the fake time (sleep_for) while
    // other threads (e.g. the io thread polling TimeoutHandler and
    // CallEveryHandler) concurrently read it via steady_time().
    std::atomic<std::chrono::nanoseconds::rep> _current_ns{0};
    void add_time(std::chrono::nanoseconds ns);
};

class AutopilotTime {
public:
    AutopilotTime() = default;
    virtual ~AutopilotTime() = default;

    AutopilotTimePoint now();

    void shift_time_by(std::chrono::nanoseconds offset);

    AutopilotTimePoint time_in(SystemTimePoint local_system_time_point);

private:
    mutable std::mutex _autopilot_system_time_offset_mutex{};
    std::chrono::nanoseconds _autopilot_time_offset{};

    virtual SystemTimePoint system_time();
};

} // namespace mavsdk
