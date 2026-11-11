#include "rtt_estimator.hpp"
#include <gtest/gtest.h>

#include <limits>

using namespace mavsdk;

TEST(RttEstimator, NoSampleMeansNoTimeout)
{
    RttEstimator estimator;
    EXPECT_DOUBLE_EQ(estimator.timeout_s(), 0.0);
    EXPECT_DOUBLE_EQ(estimator.smoothed_rtt_s(), 0.0);
}

TEST(RttEstimator, FirstSample)
{
    RttEstimator estimator;
    estimator.add_sample_s(1.0);
    EXPECT_DOUBLE_EQ(estimator.smoothed_rtt_s(), 1.0);
    // srtt + 4 * (srtt / 2)
    EXPECT_DOUBLE_EQ(estimator.timeout_s(), 3.0);
}

TEST(RttEstimator, ConvergesOnSteadyRtt)
{
    RttEstimator estimator;
    for (int i = 0; i < 100; ++i) {
        estimator.add_sample_s(0.2);
    }
    EXPECT_NEAR(estimator.smoothed_rtt_s(), 0.2, 1e-9);
    // The variance decays away, leaving the timeout close to the RTT itself.
    EXPECT_NEAR(estimator.timeout_s(), 0.2, 1e-6);
}

TEST(RttEstimator, SpikeRaisesTimeout)
{
    RttEstimator estimator;
    for (int i = 0; i < 100; ++i) {
        estimator.add_sample_s(0.2);
    }
    const double before = estimator.timeout_s();
    estimator.add_sample_s(1.0);
    // rttvar = 1/4 * 0.8, srtt = 0.2 + 0.1
    EXPECT_NEAR(estimator.smoothed_rtt_s(), 0.3, 1e-6);
    EXPECT_NEAR(estimator.timeout_s(), 0.3 + 4.0 * 0.2, 1e-6);
    EXPECT_GT(estimator.timeout_s(), before);
}

TEST(RttEstimator, IgnoresInvalidSamples)
{
    RttEstimator estimator;
    estimator.add_sample_s(-1.0);
    estimator.add_sample_s(std::numeric_limits<double>::quiet_NaN());
    estimator.add_sample_s(std::numeric_limits<double>::infinity());
    EXPECT_DOUBLE_EQ(estimator.timeout_s(), 0.0);

    estimator.add_sample_s(0.5);
    estimator.add_sample_s(-1.0);
    EXPECT_DOUBLE_EQ(estimator.smoothed_rtt_s(), 0.5);
}

TEST(RttEstimator, Reset)
{
    RttEstimator estimator;
    estimator.add_sample_s(2.0);
    estimator.reset();
    EXPECT_DOUBLE_EQ(estimator.timeout_s(), 0.0);

    estimator.add_sample_s(0.1);
    EXPECT_DOUBLE_EQ(estimator.smoothed_rtt_s(), 0.1);
    EXPECT_DOUBLE_EQ(estimator.timeout_s(), 0.3);
}
