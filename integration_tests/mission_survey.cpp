#include <iostream>
#include <functional>
#include <memory>
#include <atomic>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace dronecore;
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

//Mission::mission_result_t receive_mission_result;
static void receive_upload_mission_result(Mission::Result result);
static void receive_start_mission_result(Mission::Result result);
static void receive_mission_progress(int current, int total);
static void receive_arm_result(ActionResult result);
static void receive_return_to_launch_result(ActionResult result);
static void receive_disarm_result(ActionResult result);

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
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "System ready, let's start" << std::endl;

    std::vector<std::shared_ptr<MissionItem>> mis;

    mis.push_back(add_waypoint(47.4030765348, 8.4516599619, 20.0, 3.0, true, -90.0, 0.0, false));
    mis.push_back(add_waypoint(47.4030765348, 8.4516599619, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031028528, 8.4516834802, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031291709, 8.4517069984, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031554889, 8.4517305167, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031818069, 8.4517540350, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032081249, 8.4517775532, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032344429, 8.4518010715, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032607609, 8.4518245898, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032870789, 8.4518481080, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032590152, 8.4519157577, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032326972, 8.4518922394, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032063792, 8.4518687211, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031800612, 8.4518452029, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031537432, 8.4518216846, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031274252, 8.4517981664, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031011072, 8.4517746481, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030747891, 8.4517511298, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030484711, 8.4517276116, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030221531, 8.4517040933, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029940894, 8.4517717429, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030204074, 8.4517952612, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030467255, 8.4518187795, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030730435, 8.4518422977, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030993615, 8.4518658160, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031256795, 8.4518893343, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031519975, 8.4519128525, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031783155, 8.4519363708, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032046335, 8.4519598890, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032309515, 8.4519834073, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4032028879, 8.4520510570, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031765698, 8.4520275387, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031502518, 8.4520040204, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031239338, 8.4519805022, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030976158, 8.4519569839, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030712978, 8.4519334656, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030449798, 8.4519099474, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030186618, 8.4518864291, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029923438, 8.4518629109, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029660257, 8.4518393926, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029379621, 8.4519070422, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029642801, 8.4519305605, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029905981, 8.4519540788, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030169161, 8.4519775970, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030432341, 8.4520011153, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030695521, 8.4520246335, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030958701, 8.4520481518, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031221881, 8.4520716701, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031485062, 8.4520951883, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031748242, 8.4521187066, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031467605, 8.4521863562, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4031204425, 8.4521628380, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030941245, 8.4521393197, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030678064, 8.4521158015, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030414884, 8.4520922832, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4030151704, 8.4520687649, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029888524, 8.4520452467, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029625344, 8.4520217284, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029362164, 8.4519982101, 20.0, 3.0, true, -90.0, 0.0, true));
    mis.push_back(add_waypoint(47.4029098984, 8.4519746919, 20.0, 3.0, true, -90.0, 0.0, true));

    bool finished = false;
    while (!finished) {
        static auto _last_state = MissionState::INIT;
        if (_last_state != _mission_state) {
            std::cout << "state: " << int(_mission_state) << std::endl;
            _last_state = _mission_state;
        }
        switch (_mission_state) {
            case MissionState::INIT:
                mission->upload_mission_async(
                    mis,
                    std::bind(&receive_upload_mission_result, _1));
                _mission_state = MissionState::UPLOADING;
                break;
            case MissionState::UPLOADING:
                break;
            case MissionState::UPLOADING_DONE:
                std::cout << "arming!" << std::endl;
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
                mission->start_mission_async(
                    std::bind(&receive_start_mission_result, _1));
                _mission_state = MissionState::STARTING;
                break;
            case MissionState::STARTING:
                break;
            case MissionState::STARTING_DONE:
                _mission_state = MissionState::MISSION;

                mission->subscribe_progress(
                    std::bind(&receive_mission_progress, _1, _2));
                break;
            case MissionState::MISSION:
                break;
            case MissionState::MISSION_DONE:
                action->return_to_launch_async(
                    std::bind(&receive_return_to_launch_result, _1));
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
        std::cerr << "Error: mission send result: " << Mission::result_str(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}

void receive_start_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);

    if (result == Mission::Result::SUCCESS) {
        _mission_state = MissionState::STARTING_DONE;
    } else {
        std::cerr << "Error: mission start result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}

void receive_mission_progress(int current, int total)
{
    std::cout << "Mission status update: " << current << " / " << total << std::endl;

    if (current > 0 && current == total) {
        _mission_state = MissionState::MISSION_DONE;
    }
}

void receive_arm_result(ActionResult result)
{
    EXPECT_EQ(result, ActionResult::SUCCESS);

    if (result == ActionResult::SUCCESS) {
        _mission_state = MissionState::ARMING_DONE;
    } else {
        std::cerr << "Error: arming result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}

void receive_return_to_launch_result(ActionResult result)
{
    EXPECT_EQ(result, ActionResult::SUCCESS);

    if (result == ActionResult::SUCCESS) {
    } else {
        std::cerr << "Error: return to land result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}


void receive_disarm_result(ActionResult result)
{
    EXPECT_EQ(result, ActionResult::SUCCESS);

    if (result == ActionResult::SUCCESS) {
    } else {
        std::cerr << "Error: disarming result: " << unsigned(result) << std::endl;
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
