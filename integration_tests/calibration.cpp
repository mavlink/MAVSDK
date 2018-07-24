#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecode_sdk.h"
#include "plugins/calibration/calibration.h"

using namespace dronecode_sdk;
using namespace std::placeholders; // for `_1`

static void receive_calibration_callback(Calibration::Result result,
                                         float progress,
                                         const std::string text,
                                         const std::string calibration_type,
                                         bool &done);

TEST(HardwareTest, CalibrationGyro)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto calibration = std::make_shared<Calibration>(system);

    bool done = false;

    calibration->calibrate_gyro_async(
        std::bind(&receive_calibration_callback, _1, _2, _3, "gyro", std::ref(done)));

    while (!done) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(HardwareTest, CalibrationAccelerometer)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto calibration = std::make_shared<Calibration>(system);

    bool done = false;

    calibration->calibrate_accelerometer_async(
        std::bind(&receive_calibration_callback, _1, _2, _3, "accelerometer", std::ref(done)));

    while (!done) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(HardwareTest, CalibrationMagnetometer)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();

    auto calibration = std::make_shared<Calibration>(system);
    ASSERT_TRUE(system.has_autopilot());

    bool done = false;

    calibration->calibrate_magnetometer_async(
        std::bind(&receive_calibration_callback, _1, _2, _3, "magnetometer", std::ref(done)));

    while (!done) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(HardwareTest, CalibrationGimbalAccelerometer)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();
    ASSERT_TRUE(system.has_gimbal());

    auto calibration = std::make_shared<Calibration>(system);

    bool done = false;

    calibration->calibrate_gimbal_accelerometer_async(std::bind(
        &receive_calibration_callback, _1, _2, _3, "gimbal accelerometer", std::ref(done)));

    while (!done) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void receive_calibration_callback(Calibration::Result result,
                                  float progress,
                                  const std::string text,
                                  const std::string calibration_type,
                                  bool &done)
{
    if (result == Calibration::Result::IN_PROGRESS) {
        LogInfo() << calibration_type << " calibration in progress: " << progress;
    } else if (result == Calibration::Result::INSTRUCTION) {
        LogInfo() << calibration_type << " calibration instruction: " << text;
    } else {
        EXPECT_EQ(result, Calibration::Result::SUCCESS);

        if (result != Calibration::Result::SUCCESS) {
            LogErr() << calibration_type
                     << " calibration error: " << Calibration::result_str(result);
            if (result == Calibration::Result::FAILED) {
                LogErr() << calibration_type << " cailbration failed: " << text;
            }
        }
        done = true;
    }
}
