#include "libmav_receiver.h"
#include "log.h"
#include <gtest/gtest.h>
#include <chrono>
#include <sstream>
#include <cmath>
#include <limits>

using namespace mavsdk;

// Benchmark helper
template<typename Func> double benchmark_ns_per_call(Func func, int iterations)
{
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    auto elapsed = std::chrono::steady_clock::now() - start;
    return static_cast<double>(
               std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()) /
           iterations;
}

TEST(LibmavReceiver, JsonConversionFloat)
{
    constexpr int iterations = 100000;

    std::ostringstream oss;
    float test_value = 3.14159265f;

    double ns_per_call = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            value_to_json_stream(oss, test_value);
        },
        iterations);

    double ns_per_call_fast = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            value_to_json_stream_fast(oss, test_value);
        },
        iterations);

    LogDebug() << "Float conversion (original): " << ns_per_call << " ns/call";
    LogDebug() << "Float conversion (to_chars): " << ns_per_call_fast << " ns/call";
    LogDebug() << "  Speedup: " << (ns_per_call / ns_per_call_fast) << "x";

    // Verify correctness
    oss.str("");
    value_to_json_stream(oss, test_value);
    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str(), "null");

    oss.str("");
    value_to_json_stream_fast(oss, test_value);
    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str(), "null");
}

TEST(LibmavReceiver, JsonConversionDouble)
{
    constexpr int iterations = 100000;

    std::ostringstream oss;
    double test_value = 3.141592653589793;

    double ns_per_call = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            value_to_json_stream(oss, test_value);
        },
        iterations);

    double ns_per_call_fast = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            value_to_json_stream_fast(oss, test_value);
        },
        iterations);

    LogDebug() << "Double conversion (original): " << ns_per_call << " ns/call";
    LogDebug() << "Double conversion (to_chars): " << ns_per_call_fast << " ns/call";
    LogDebug() << "  Speedup: " << (ns_per_call / ns_per_call_fast) << "x";

    // Verify correctness
    oss.str("");
    value_to_json_stream(oss, test_value);
    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str(), "null");

    oss.str("");
    value_to_json_stream_fast(oss, test_value);
    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str(), "null");
}

TEST(LibmavReceiver, JsonConversionFloatNaN)
{
    std::ostringstream oss;
    float nan_value = std::numeric_limits<float>::quiet_NaN();

    value_to_json_stream(oss, nan_value);
    EXPECT_EQ(oss.str(), "null");
}

TEST(LibmavReceiver, JsonConversionFloatInf)
{
    std::ostringstream oss;
    float inf_value = std::numeric_limits<float>::infinity();

    value_to_json_stream(oss, inf_value);
    EXPECT_EQ(oss.str(), "null");
}

TEST(LibmavReceiver, JsonConversionInt32)
{
    constexpr int iterations = 100000;

    std::ostringstream oss;
    int32_t test_value = -123456789;

    double ns_per_call = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            value_to_json_stream(oss, test_value);
        },
        iterations);

    LogDebug() << "Int32 conversion: " << ns_per_call << " ns/call";

    // Verify correctness
    oss.str("");
    value_to_json_stream(oss, test_value);
    EXPECT_EQ(oss.str(), "-123456789");
}

TEST(LibmavReceiver, JsonConversionFloatVector)
{
    constexpr int iterations = 100000;

    std::ostringstream oss;
    std::vector<float> test_value = {1.0f, 2.0f, 3.0f, 4.0f};

    double ns_per_call = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            value_to_json_stream(oss, test_value);
        },
        iterations);

    LogDebug() << "Float vector (4 elements) conversion: " << ns_per_call << " ns/call";

    // Verify correctness
    oss.str("");
    value_to_json_stream(oss, test_value);
    EXPECT_EQ(oss.str()[0], '[');
    EXPECT_EQ(oss.str().back(), ']');
}

TEST(LibmavReceiver, JsonConversionFloatVectorWithNaN)
{
    std::ostringstream oss;
    std::vector<float> test_value = {1.0f, std::numeric_limits<float>::quiet_NaN(), 3.0f};

    value_to_json_stream(oss, test_value);
    // Should contain "null" for the NaN value
    EXPECT_NE(oss.str().find("null"), std::string::npos);
}

TEST(LibmavReceiver, JsonConversionString)
{
    std::ostringstream oss;
    std::string test_value = "hello";

    value_to_json_stream(oss, test_value);
    EXPECT_EQ(oss.str(), "\"hello\"");
}

// Benchmark that simulates a typical MAVLink message with multiple float fields
TEST(LibmavReceiver, JsonConversionTypicalMessage)
{
    constexpr int iterations = 10000;

    std::ostringstream oss;

    // Simulate fields from a typical message like ATTITUDE
    float roll = 0.1f;
    float pitch = -0.05f;
    float yaw = 1.57f;
    float rollspeed = 0.01f;
    float pitchspeed = -0.02f;
    float yawspeed = 0.0f;
    uint32_t time_boot_ms = 123456;

    double ns_per_call = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            oss << "{";
            oss << "\"time_boot_ms\":";
            value_to_json_stream(oss, time_boot_ms);
            oss << ",\"roll\":";
            value_to_json_stream(oss, roll);
            oss << ",\"pitch\":";
            value_to_json_stream(oss, pitch);
            oss << ",\"yaw\":";
            value_to_json_stream(oss, yaw);
            oss << ",\"rollspeed\":";
            value_to_json_stream(oss, rollspeed);
            oss << ",\"pitchspeed\":";
            value_to_json_stream(oss, pitchspeed);
            oss << ",\"yawspeed\":";
            value_to_json_stream(oss, yawspeed);
            oss << "}";
        },
        iterations);

    double ns_per_call_fast = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            oss << "{";
            oss << "\"time_boot_ms\":";
            value_to_json_stream_fast(oss, time_boot_ms);
            oss << ",\"roll\":";
            value_to_json_stream_fast(oss, roll);
            oss << ",\"pitch\":";
            value_to_json_stream_fast(oss, pitch);
            oss << ",\"yaw\":";
            value_to_json_stream_fast(oss, yaw);
            oss << ",\"rollspeed\":";
            value_to_json_stream_fast(oss, rollspeed);
            oss << ",\"pitchspeed\":";
            value_to_json_stream_fast(oss, pitchspeed);
            oss << ",\"yawspeed\":";
            value_to_json_stream_fast(oss, yawspeed);
            oss << "}";
        },
        iterations);

    LogDebug() << "Typical message (original): " << ns_per_call << " ns/call";
    LogDebug() << "Typical message (to_chars): " << ns_per_call_fast << " ns/call";
    LogDebug() << "  Speedup: " << (ns_per_call / ns_per_call_fast) << "x";

    // At typical MAVLink rates of 50Hz, we'd be doing 50 calls/sec
    // Each call should take less than 20ms (1/50) to not be a bottleneck
    // In reality, we want it to be much faster - sub-millisecond
    EXPECT_LT(ns_per_call_fast, 1000000); // Less than 1ms per call
}

// Benchmark simulating a GPS message with double fields (latitude/longitude)
TEST(LibmavReceiver, JsonConversionGpsMessage)
{
    constexpr int iterations = 10000;

    std::ostringstream oss;

    // Simulate fields from GLOBAL_POSITION_INT (using doubles for lat/lon)
    double lat = 47.397742;
    double lon = 8.545594;
    double alt = 488.123;
    double relative_alt = 10.5;
    int32_t vx = 100;
    int32_t vy = -50;
    int32_t vz = 5;
    uint16_t hdg = 1800;

    double ns_per_call = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            oss << "{";
            oss << "\"lat\":";
            value_to_json_stream(oss, lat);
            oss << ",\"lon\":";
            value_to_json_stream(oss, lon);
            oss << ",\"alt\":";
            value_to_json_stream(oss, alt);
            oss << ",\"relative_alt\":";
            value_to_json_stream(oss, relative_alt);
            oss << ",\"vx\":";
            value_to_json_stream(oss, vx);
            oss << ",\"vy\":";
            value_to_json_stream(oss, vy);
            oss << ",\"vz\":";
            value_to_json_stream(oss, vz);
            oss << ",\"hdg\":";
            value_to_json_stream(oss, hdg);
            oss << "}";
        },
        iterations);

    double ns_per_call_fast = benchmark_ns_per_call(
        [&]() {
            oss.str("");
            oss << "{";
            oss << "\"lat\":";
            value_to_json_stream_fast(oss, lat);
            oss << ",\"lon\":";
            value_to_json_stream_fast(oss, lon);
            oss << ",\"alt\":";
            value_to_json_stream_fast(oss, alt);
            oss << ",\"relative_alt\":";
            value_to_json_stream_fast(oss, relative_alt);
            oss << ",\"vx\":";
            value_to_json_stream_fast(oss, vx);
            oss << ",\"vy\":";
            value_to_json_stream_fast(oss, vy);
            oss << ",\"vz\":";
            value_to_json_stream_fast(oss, vz);
            oss << ",\"hdg\":";
            value_to_json_stream_fast(oss, hdg);
            oss << "}";
        },
        iterations);

    LogDebug() << "GPS message (original): " << ns_per_call << " ns/call";
    LogDebug() << "GPS message (to_chars): " << ns_per_call_fast << " ns/call";
    LogDebug() << "  Speedup: " << (ns_per_call / ns_per_call_fast) << "x";

    EXPECT_LT(ns_per_call_fast, 1000000); // Less than 1ms per call
}
