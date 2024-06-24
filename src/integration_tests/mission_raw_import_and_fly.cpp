#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include <future>

using namespace mavsdk;

MissionRaw::MissionItem create_mission_item(
    uint32_t _seq,
    uint32_t _frame,
    uint32_t _command,
    uint32_t _current,
    uint32_t _autocontinue,
    float _param1,
    float _param2,
    float _param3,
    float _param4,
    double _x,
    double _y,
    double _z,
    uint32_t _mission_type);

std::vector<MissionRaw::MissionItem> create_mission_raw();
void test_mission_raw(
    mavsdk::MissionRaw& mission_raw, mavsdk::Action& action, mavsdk::Telemetry& telemetry);

TEST(SitlTest, PX4MissionRawImportAndFly)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->is_connected());
    ASSERT_TRUE(system->has_autopilot());

    auto action = Action{system};
    auto mission_raw = MissionRaw{system};
    auto telemetry = Telemetry{system};

    auto import_result =
        mission_raw.import_qgroundcontrol_mission("./src/integration_tests/triangle.plan");
    ASSERT_EQ(import_result.first, MissionRaw::Result::Success);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [&telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry.health_all_ok();
        },
        std::chrono::seconds(10)));

    ASSERT_EQ(action.arm(), Action::Result::Success);

    ASSERT_EQ(
        mission_raw.upload_mission(import_result.second.mission_items),
        MissionRaw::Result::Success);
    ASSERT_EQ(mission_raw.start_mission(), MissionRaw::Result::Success);

    auto prom = std::promise<void>();
    auto fut = prom.get_future();

    MissionRaw::MissionProgressHandle handle =
        mission_raw.subscribe_mission_progress([&](MissionRaw::MissionProgress progress) {
            LogInfo() << "Progress: " << progress.current << "/" << progress.total;
            if (progress.current == progress.total) {
                mission_raw.unsubscribe_mission_progress(handle);
                prom.set_value();
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(120)), std::future_status::ready);
    fut.get();

    ASSERT_EQ(action.return_to_launch(), Action::Result::Success);

    while (telemetry.armed()) {
        LogInfo() << "Waiting for drone to be landed and disarmed.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(SitlTest, APMissionRawImportAndFly)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->is_connected());
    ASSERT_TRUE(system->has_autopilot());

    auto action = Action{system};
    auto mission_raw = MissionRaw{system};
    auto telemetry = Telemetry{system};

    std::vector<MissionRaw::MissionItem> mission_plan = create_mission_raw();

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [&telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry.health_all_ok();
        },
        std::chrono::seconds(10)));

    ASSERT_EQ(mission_raw.upload_mission(mission_plan), MissionRaw::Result::Success);

    ASSERT_EQ(action.set_takeoff_altitude(50.0f), Action::Result::Success);

    ASSERT_EQ(action.arm(), Action::Result::Success);

    ASSERT_EQ(action.takeoff(), Action::Result::Success);

    ASSERT_EQ(mission_raw.start_mission(), MissionRaw::Result::Success);

    auto prom = std::promise<void>();
    auto fut = prom.get_future();

    mission_raw.subscribe_mission_progress([&](MissionRaw::MissionProgress progress) {
        LogInfo() << "Progress: " << progress.current << "/" << progress.total;
        if (progress.current == progress.total) {
            mission_raw.subscribe_mission_progress(nullptr);
            prom.set_value();
        }
    });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(120)), std::future_status::ready);
    fut.get();

    ASSERT_EQ(action.return_to_launch(), Action::Result::Success);

    while (telemetry.armed()) {
        LogInfo() << "Waiting for drone to be landed and disarmed.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::vector<MissionRaw::MissionItem> create_mission_raw()
{
    std::vector<MissionRaw::MissionItem> mission_raw_items;

    // _seq, _frame, _command, _current, _autocontinue, _param1, _param2, _param3, _param4, _x,  _y,
    // _z,  _mission_type Add Home Position
    mission_raw_items.push_back(
        create_mission_item(0, 0, 16, 1, 1, 0, 0, 0, 0, 47.397742, 8.545594, 488, 0));

    // Add Takeoff
    mission_raw_items.push_back(create_mission_item(1, 3, 22, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0));

    // Add Mission Item 2-3
    mission_raw_items.push_back(create_mission_item(
        2, 3, 16, 0, 1, 0, 0, 0, 0, 47.39776911820642, 8.545794816614517, 30, 0));
    mission_raw_items.push_back(create_mission_item(
        3, 3, 16, 0, 1, 0, 0, 0, 0, 47.39814478901126, 8.544659618054993, 30, 0));

    // Return to Launch
    mission_raw_items.push_back(create_mission_item(4, 3, 20, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0));
    return mission_raw_items;
}

MissionRaw::MissionItem create_mission_item(
    uint32_t _seq,
    uint32_t _frame,
    uint32_t _command,
    uint32_t _current,
    uint32_t _autocontinue,
    float _param1,
    float _param2,
    float _param3,
    float _param4,
    double _x,
    double _y,
    double _z,
    uint32_t _mission_type)
{
    MissionRaw::MissionItem new_raw_item_nav{};
    new_raw_item_nav.seq = _seq;
    new_raw_item_nav.frame = _frame; // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT
    new_raw_item_nav.command = _command; // MAV_CMD_NAV_WAYPOINT
    new_raw_item_nav.current = _current;
    new_raw_item_nav.autocontinue = _autocontinue;
    new_raw_item_nav.param1 = _param1; // Hold
    new_raw_item_nav.param2 = _param2; // Accept Radius
    new_raw_item_nav.param3 = _param3; // Pass Radius
    new_raw_item_nav.param4 = _param4; // Yaw
    new_raw_item_nav.x = int32_t(std::round(_x * 1e7));
    new_raw_item_nav.y = int32_t(std::round(_y * 1e7));
    new_raw_item_nav.z = float(_z);
    new_raw_item_nav.mission_type = _mission_type;
    return new_raw_item_nav;
}
