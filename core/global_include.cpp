#include "global_include.h"

#include <cfloat>
#include <cstdint>
#include <limits>

namespace dronecore {

using std::chrono::steady_clock;

Time::Time() {}
Time::~Time() {}

dl_time_t Time::steady_time()
{
    return steady_clock::now();
}

double Time::elapsed_s()
{
    auto now = steady_time().time_since_epoch();

    return (now.count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

double Time::elapsed_since_s(const dl_time_t &since)
{
    auto now = steady_time();

    return ((now - since).count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

dl_time_t Time::steady_time_in_future(double duration_s)
{
    auto now = steady_time();
    return now + std::chrono::milliseconds(int64_t(duration_s * 1e3));
}

void Time::shift_steady_time_by(dl_time_t &time, double offset_s)
{
    time += std::chrono::milliseconds(int64_t(offset_s * 1e3));
}

void Time::sleep_for(std::chrono::hours h)
{
    std::this_thread::sleep_for(h);
}

void Time::sleep_for(std::chrono::minutes m)
{
    std::this_thread::sleep_for(m);
}

void Time::sleep_for(std::chrono::seconds s)
{
    std::this_thread::sleep_for(s);
}

void Time::sleep_for(std::chrono::milliseconds ms)
{
    std::this_thread::sleep_for(ms);
}

void Time::sleep_for(std::chrono::microseconds us)
{
    std::this_thread::sleep_for(us);
}

void Time::sleep_for(std::chrono::nanoseconds ns)
{
    std::this_thread::sleep_for(ns);
}

FakeTime::FakeTime() : Time()
{
    // Start with current time so we don't start from 0.
    _current = steady_clock::now();
}

FakeTime::~FakeTime() {}

dl_time_t FakeTime::steady_time()
{
    return _current;
}

void FakeTime::sleep_for(std::chrono::hours h)
{
    _current += h;
    add_overhead();
}

void FakeTime::sleep_for(std::chrono::minutes m)
{
    _current += m;
    add_overhead();
}

void FakeTime::sleep_for(std::chrono::seconds s)
{
    _current += s;
    add_overhead();
}

void FakeTime::sleep_for(std::chrono::milliseconds ms)
{
    _current += ms;
    add_overhead();
}

void FakeTime::sleep_for(std::chrono::microseconds us)
{
    _current += us;
    add_overhead();
}

void FakeTime::sleep_for(std::chrono::nanoseconds ns)
{
    _current += ns;
    add_overhead();
}

void FakeTime::add_overhead()
{
    _current += std::chrono::microseconds(50);
}

double to_rad_from_deg(double deg)
{
    return deg / 180.0 * M_PI;
}

double to_deg_from_rad(double rad)
{
    return rad / M_PI * 180.0;
}

float to_rad_from_deg(float deg)
{
    return deg / 180.0f * M_PI_F;
}

float to_deg_from_rad(float rad)
{
    return rad / M_PI_F * 180.0f;
}

bool are_equal(float one, float two)
{
    return (fabsf(one - two) < std::numeric_limits<float>::epsilon());
}

bool are_equal(double one, double two)
{
    return (fabs(one - two) < std::numeric_limits<double>::epsilon());
}

} // namespace dronecore
