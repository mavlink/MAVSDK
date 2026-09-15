#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

// The MAV_FRAME values used in ODOMETRY (BODY_NED=8, VISION_NED=16, ESTIM_NED=18)
// are not sequential, so they must be mapped by name rather than cast into the
// generated C++ enum.
TEST(Telemetry, OdometryFrameIds)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15223"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15223"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    auto prom = std::make_shared<std::promise<Telemetry::Odometry>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto handle = telemetry.subscribe_odometry([prom, flag](const Telemetry::Odometry& odometry) {
        std::call_once(*flag, [&]() { prom->set_value(odometry); });
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "ODOMETRY";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    // frame_id: MAV_FRAME_VISION_NED (16), child_frame_id: MAV_FRAME_BODY_NED (8)
    msg.fields_json = R"({"time_usec":123456,"frame_id":16,"child_frame_id":8,
        "x":1.0,"y":2.0,"z":3.0,"q":[1.0,0.0,0.0,0.0],
        "vx":0.1,"vy":0.2,"vz":0.3,"rollspeed":0.0,"pitchspeed":0.0,"yawspeed":0.0,
        "pose_covariance":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        "velocity_covariance":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
        "reset_counter":0,"estimator_type":0,"quality":0})";

    ASSERT_EQ(sender.send_message(msg), MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto odometry = fut.get();
    EXPECT_EQ(odometry.frame_id, Telemetry::Odometry::MavFrame::VisionNed);
    EXPECT_EQ(odometry.child_frame_id, Telemetry::Odometry::MavFrame::BodyNed);
    EXPECT_EQ(odometry.time_usec, 123456u);
    EXPECT_FLOAT_EQ(odometry.position_body.x_m, 1.0f);

    telemetry.unsubscribe_odometry(handle);
}
