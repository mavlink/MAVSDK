#pragma once

#include <mutex>

#include "mavsdk_export.h"

namespace mavsdk {

/*
 * Smoothed round-trip time of a link, and a retransmission timeout derived
 * from it, using the estimator TCP uses (RFC 6298):
 *
 *   first sample R:  srtt = R,  rttvar = R / 2
 *   later samples:   rttvar = 3/4 rttvar + 1/4 |srtt - R|
 *                    srtt   = 7/8 srtt   + 1/8 R
 *   timeout = srtt + 4 rttvar
 *
 * Before the first sample timeout_s() returns 0, so callers can take the
 * maximum with their configured timeout.
 *
 * All methods are safe to call from any thread. The internal mutex is a leaf.
 */
class MAVSDK_TEST_EXPORT RttEstimator {
public:
    void add_sample_s(double rtt_s);
    void reset();

    [[nodiscard]] double smoothed_rtt_s() const;
    [[nodiscard]] double timeout_s() const;

private:
    mutable std::mutex _mutex;
    bool _has_sample{false};
    double _srtt_s{0.0};
    double _rttvar_s{0.0};
};

} // namespace mavsdk
