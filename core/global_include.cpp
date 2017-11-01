#define POISON_DISABLED
#include "global_include.h"

#include <cfloat>
#include <cstdint>
#include <limits>

namespace dronecore {

using std::chrono::steady_clock;

#ifdef MOCK_TIME
class MockTime
{
public:
    // Singleton
    static MockTime &instance()
    {
        static MockTime instance;
        return instance;
    }

    std::chrono::time_point<steady_clock, std::chrono::nanoseconds> get_now()
    {
        return _current;
    }

    template <typename Duration>
    void add(Duration d)
    {
        _current += d;
    }

    MockTime(MockTime const &) = delete;
    void operator=(MockTime const &) = delete;

private:
    MockTime()
    {
        // Start with current time so we don't start from 0.
        _current = steady_clock::now();
    }

    std::chrono::time_point<steady_clock, std::chrono::nanoseconds> _current {};
};

// Explicitly instantiate for all durations we know and potentially need.
template void MockTime::add<std::chrono::hours>(std::chrono::hours);
template void MockTime::add<std::chrono::minutes>(std::chrono::minutes);
template void MockTime::add<std::chrono::seconds>(std::chrono::seconds);
template void MockTime::add<std::chrono::milliseconds>(std::chrono::milliseconds);
template void MockTime::add<std::chrono::microseconds>(std::chrono::microseconds);
template void MockTime::add<std::chrono::nanoseconds>(std::chrono::nanoseconds);

#endif



dl_time_t steady_time()
{
#ifndef MOCK_TIME
    return steady_clock::now();
#else
    return MockTime::instance().get_now();
#endif
}

double elapsed_s()
{
    auto now = steady_time().time_since_epoch();

    return (now.count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

double elapsed_since_s(const dl_time_t &since)
{
    auto now = steady_time();

    return ((now - since).count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

template <typename Duration>
void dc_sleep_for(Duration d)
{
#ifndef MOCK_TIME
    std::this_thread::sleep_for(d);
#else
    MockTime::instance().add(d);
    // We also add some overhead that sleep usually has.
    MockTime::instance().add(std::chrono::microseconds(10));
#endif
}

template void dc_sleep_for<>(std::chrono::hours);
template void dc_sleep_for<>(std::chrono::minutes);
template void dc_sleep_for<>(std::chrono::seconds);
template void dc_sleep_for<>(std::chrono::milliseconds);
template void dc_sleep_for<>(std::chrono::microseconds);
template void dc_sleep_for<>(std::chrono::nanoseconds);


dl_time_t steady_time_in_future(double duration_s)
{
    auto now = steady_time();
    return now + std::chrono::milliseconds(int64_t(duration_s * 1e3));
}

void shift_steady_time_by(dl_time_t &time, double offset_s)
{
    time += std::chrono::milliseconds(int64_t(offset_s * 1e3));
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

