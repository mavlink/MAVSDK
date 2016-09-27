#include "global_include.h"

namespace dronelink {


dl_time_t steady_time()
{
    using std::chrono::steady_clock;

    return steady_clock::now();
}

double elapsed_s(dl_time_t &since)
{
    using std::chrono::steady_clock;

    auto now = steady_clock::now();

    return ((now - since).count()) * steady_clock::period::num /
           static_cast<double>(steady_clock::period::den);
}

dl_time_t steady_time_in_future(double duration_s)
{
    auto now = std::chrono::steady_clock::now();
    return now + std::chrono::nanoseconds((long int)(duration_s * 1e9));
}


} // namespace dronelink
