#pragma once

#define UNUSED(x) (void)(x)

#include <chrono>
#include <thread>
#include <mutex>

// Instead of using the constant from math.h or cmath we define it ourselves. This way
// we don't import all the other C math functions and make sure to use the C++ functions
// from the standard library (e.g. std::abs() instead of abs()).
#ifndef M_PI
constexpr double M_PI = 3.14159265358979323846;
#endif

#ifndef M_PI_F
constexpr float M_PI_F = float(M_PI);
#endif

#ifdef WINDOWS
#define STRNCPY strncpy_s
#else
#define STRNCPY strncpy
#endif

#if !defined(WINDOWS)
// Remove path and extract only filename.
#define FILENAME \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define FILENAME __FILE__
#endif

#define call_user_callback(...) call_user_callback_located(FILENAME, __LINE__, __VA_ARGS__)

namespace mavsdk {

typedef std::chrono::time_point<std::chrono::steady_clock> dl_time_t;
typedef std::chrono::time_point<std::chrono::system_clock> dl_system_time_t;
typedef std::chrono::time_point<std::chrono::system_clock> dl_autopilot_time_t;

class Time {
public:
    Time() = default;
    virtual ~Time() = default;

    virtual dl_time_t steady_time();
    virtual dl_system_time_t system_time();
    double elapsed_s();
    double elapsed_since_s(const dl_time_t& since);
    dl_time_t steady_time_in_future(double duration_s);
    static void shift_steady_time_by(dl_time_t& time, double offset_s);

    virtual void sleep_for(std::chrono::hours h);
    virtual void sleep_for(std::chrono::minutes m);
    virtual void sleep_for(std::chrono::seconds s);
    virtual void sleep_for(std::chrono::milliseconds ms);
    virtual void sleep_for(std::chrono::microseconds us);
    virtual void sleep_for(std::chrono::nanoseconds ns);
};

class FakeTime : public Time {
public:
    FakeTime();
    ~FakeTime() override = default;
    dl_time_t steady_time() override;
    void sleep_for(std::chrono::hours h) override;
    void sleep_for(std::chrono::minutes m) override;
    void sleep_for(std::chrono::seconds s) override;
    void sleep_for(std::chrono::milliseconds ms) override;
    void sleep_for(std::chrono::microseconds us) override;
    void sleep_for(std::chrono::nanoseconds ns) override;

private:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> _current{};
    void add_overhead();
};

class AutopilotTime {
public:
    AutopilotTime() = default;
    virtual ~AutopilotTime() = default;

    dl_autopilot_time_t now();

    void shift_time_by(std::chrono::nanoseconds offset);

    dl_autopilot_time_t time_in(dl_system_time_t local_system_time_point);

private:
    mutable std::mutex _autopilot_system_time_offset_mutex{};
    std::chrono::nanoseconds _autopilot_time_offset{};

    virtual dl_system_time_t system_time();
};

double to_rad_from_deg(double deg);
double to_deg_from_rad(double rad);

float to_rad_from_deg(float deg);
float to_deg_from_rad(float rad);

bool are_equal(float one, float two);
bool are_equal(double one, double two);

} // namespace mavsdk
