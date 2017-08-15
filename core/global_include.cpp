#include "global_include.h"
#include <cfloat>
#include <cstdint>
#include <limits>

namespace dronecore {


dl_time_t steady_time()
{
    using std::chrono::steady_clock;

    return steady_clock::now();
}

double elapsed_s()
{
    using std::chrono::steady_clock;

    auto now = steady_clock::now().time_since_epoch();

    return (now.count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

double elapsed_since_s(const dl_time_t &since)
{
    using std::chrono::steady_clock;

    auto now = steady_clock::now();

    return ((now - since).count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

dl_time_t steady_time_in_future(double duration_s)
{
    auto now = std::chrono::steady_clock::now();
    return now + std::chrono::milliseconds(int64_t(duration_s * 1e3));
}


double to_rad_from_deg(double deg)
{
    return deg / 180.0 * M_PI;
}

double to_deg_from_rad(double rad)
{
    return rad / M_PI * 180.0;
}

bool are_equal(float one, float two)
{
    return (fabs(one - two) < std::numeric_limits<float>::epsilon());
}

bool are_equal(double one, double two)
{
    return (fabs(one - two) < std::numeric_limits<double>::epsilon());
}

} // namespace dronecore

