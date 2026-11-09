#include "rtt_estimator.hpp"

#include <cmath>

namespace mavsdk {

void RttEstimator::add_sample_s(double rtt_s)
{
    if (!std::isfinite(rtt_s) || rtt_s < 0.0) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    if (!_has_sample) {
        _srtt_s = rtt_s;
        _rttvar_s = rtt_s / 2.0;
        _has_sample = true;
        return;
    }

    _rttvar_s = 0.75 * _rttvar_s + 0.25 * std::fabs(_srtt_s - rtt_s);
    _srtt_s = 0.875 * _srtt_s + 0.125 * rtt_s;
}

void RttEstimator::reset()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _has_sample = false;
    _srtt_s = 0.0;
    _rttvar_s = 0.0;
}

double RttEstimator::smoothed_rtt_s() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _srtt_s;
}

double RttEstimator::timeout_s() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_has_sample) {
        return 0.0;
    }
    return _srtt_s + 4.0 * _rttvar_s;
}

} // namespace mavsdk
