#include <iostream>
#include <functional>
#include <memory>
#include <atomic>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace dronecode_sdk;
using namespace std::placeholders; // for `_1`

enum class MissionState : unsigned {
    INIT = 0,
    UPLOADING,
    UPLOADING_DONE,
    ARMING,
    ARMING_DONE,
    STARTING,
    STARTING_DONE,
    MISSION,
    MISSION_DONE,
    RETURN,
    RETURN_DONE,
    DONE,
    ERROR
};

static MissionState _mission_state = MissionState::INIT;

// Mission::mission_result_t receive_mission_result;
static void receive_upload_mission_result(Mission::Result result);
static void receive_start_mission_result(Mission::Result result);
static void receive_mission_progress(int current, int total);
static void receive_arm_result(Action::Result result);
static void receive_return_to_launch_result(Action::Result result);
static void receive_disarm_result(Action::Result result);

static std::shared_ptr<MissionItem> add_waypoint(double latitude_deg,
                                                 double longitude_deg,
                                                 float relative_altitude_m,
                                                 float speed_m_s,
                                                 bool is_fly_through,
                                                 float gimbal_pitch_deg,
                                                 float gimbal_yaw_deg,
                                                 bool take_photo);

TEST_F(SitlTest, MissionSurvey)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        LogInfo() << "Waiting for system to be ready";
        LogDebug() << "Health: " << telemetry->health();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "System ready, let's start";

    std::vector<std::shared_ptr<MissionItem>> mis;

    mis.push_back(
        add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    mis.push_back(
        add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981966450545, 8.54567254016397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982229631545, 8.54569605836397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982492811545, 8.54571957666396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982755991545, 8.54574309496397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983019171545, 8.54576661316396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983282351545, 8.54579013146397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983545531545, 8.54581364976396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983808711545, 8.54583716796397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983528074545, 8.54590481766397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983264894545, 8.54588129936396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983001714545, 8.54585778106397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982738534545, 8.54583426286396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982475354545, 8.54581074456397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982212174545, 8.54578722636397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981948994545, 8.54576370806397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981685813545, 8.54574018976397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981422633545, 8.54571667156397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981159453545, 8.54569315326397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980878816545, 8.54576080286397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981141996545, 8.54578432116397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981405177545, 8.54580783946397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981668357545, 8.54583135766397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981931537545, 8.54585487596397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982194717545, 8.54587839426397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982457897545, 8.54590191246396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982721077545, 8.54592543076397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982984257545, 8.54594894896396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3983247437545, 8.54597246726397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982966801545, 8.54604011696397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982703620545, 8.54601659866396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982440440545, 8.54599308036397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982177260545, 8.54596956216396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981914080545, 8.54594604386397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981650900545, 8.54592252556397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981387720545, 8.54589900736397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981124540545, 8.54587548906397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980861360545, 8.54585197086396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980598179545, 8.54582845256397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980317543545, 8.54589610216397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980580723545, 8.54591962046396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980843903545, 8.54594313876397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981107083545, 8.54596665696397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981370263545, 8.54599017526397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981633443545, 8.54601369346397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981896623545, 8.54603721176397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982159803545, 8.54606073006397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982422984545, 8.54608424826396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982686164545, 8.54610776656397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982405527545, 8.54617541616397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3982142347545, 8.54615189796396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981879167545, 8.54612837966397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981615986545, 8.54610486146397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981352806545, 8.54608134316397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3981089626545, 8.54605782486397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980826446545, 8.54603430666396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980563266545, 8.54601078836397, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980300086545, 8.54598727006396, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(
        add_waypoint(47.3980036906545, 8.54596375186397, 20.0, 3.0, true, -90.0, 0.0, true));

    bool finished = false;
    while (!finished) {
        static auto _last_state = MissionState::INIT;
        if (_last_state != _mission_state) {
            LogDebug() << "state: " << int(_mission_state);
            _last_state = _mission_state;
        }
        switch (_mission_state) {
            case MissionState::INIT:
                mission->upload_mission_async(mis, std::bind(&receive_upload_mission_result, _1));
                _mission_state = MissionState::UPLOADING;
                break;
            case MissionState::UPLOADING:
                break;
            case MissionState::UPLOADING_DONE:
                LogInfo() << "arming!";
                if (!telemetry->armed()) {
                    action->arm_async(std::bind(&receive_arm_result, _1));
                    _mission_state = MissionState::ARMING;
                }
                break;
            case MissionState::ARMING:
                break;
            case MissionState::ARMING_DONE:
                // TODO: There can be a race here if PX4 still listens to the armed flag in
                // the message DO_SET_MODE. Once it ignores it as in the spec, this is not
                // needed anymore.
                std::this_thread::sleep_for(std::chrono::seconds(2));
                mission->start_mission_async(std::bind(&receive_start_mission_result, _1));
                _mission_state = MissionState::STARTING;
                break;
            case MissionState::STARTING:
                break;
            case MissionState::STARTING_DONE:
                _mission_state = MissionState::MISSION;

                mission->subscribe_progress(std::bind(&receive_mission_progress, _1, _2));
                break;
            case MissionState::MISSION:
                break;
            case MissionState::MISSION_DONE:
                action->return_to_launch_async(std::bind(&receive_return_to_launch_result, _1));
                _mission_state = MissionState::RETURN;
                break;
            case MissionState::RETURN:
                if (!telemetry->in_air()) {
                    _mission_state = MissionState::RETURN_DONE;
                }
                break;
            case MissionState::RETURN_DONE:
                action->disarm_async(std::bind(&receive_disarm_result, _1));
                break;
            case MissionState::DONE:
                finished = true;
                break;

            case MissionState::ERROR:
                finished = true;
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_EQ(_mission_state, MissionState::DONE);
}

void receive_upload_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);

    if (result == Mission::Result::SUCCESS) {
        _mission_state = MissionState::UPLOADING_DONE;
    } else {
        LogErr() << "Error: mission send result: " << Mission::result_str(result);
        _mission_state = MissionState::ERROR;
    }
}

void receive_start_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);

    if (result == Mission::Result::SUCCESS) {
        _mission_state = MissionState::STARTING_DONE;
    } else {
        LogErr() << "Error: mission start result: " << Mission::result_str(result);
        _mission_state = MissionState::ERROR;
    }
}

void receive_mission_progress(int current, int total)
{
    LogInfo() << "Mission status update: " << current << " / " << total;

    if (current > 0 && current == total) {
        _mission_state = MissionState::MISSION_DONE;
    }
}

void receive_arm_result(Action::Result result)
{
    EXPECT_EQ(result, Action::Result::SUCCESS);

    if (result == Action::Result::SUCCESS) {
        _mission_state = MissionState::ARMING_DONE;
    } else {
        LogErr() << "Error: arming result: " << Action::result_str(result);
        _mission_state = MissionState::ERROR;
    }
}

void receive_return_to_launch_result(Action::Result result)
{
    EXPECT_EQ(result, Action::Result::SUCCESS);

    if (result == Action::Result::SUCCESS) {
    } else {
        LogErr() << "Error: return to land result: " << Action::result_str(result);
        _mission_state = MissionState::ERROR;
    }
}

void receive_disarm_result(Action::Result result)
{
    EXPECT_EQ(result, Action::Result::SUCCESS);

    if (result == Action::Result::SUCCESS) {
    } else {
        LogErr() << "Error: disarming result: " << Action::result_str(result);
    }

    _mission_state = MissionState::DONE;
}

std::shared_ptr<MissionItem> add_waypoint(double latitude_deg,
                                          double longitude_deg,
                                          float relative_altitude_m,
                                          float speed_m_s,
                                          bool is_fly_through,
                                          float gimbal_pitch_deg,
                                          float gimbal_yaw_deg,
                                          bool take_photo)
{
    std::shared_ptr<MissionItem> new_item(new MissionItem());
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    new_item->set_speed(speed_m_s);
    new_item->set_fly_through(is_fly_through);
    new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    if (take_photo) {
        new_item->set_camera_action(MissionItem::CameraAction::TAKE_PHOTO);
    }
    return new_item;
}
