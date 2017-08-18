#include <iostream>
#include <functional>
#include <memory>
#include <atomic>
#include "integration_test_helper.h"
#include "dronecore.h"

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
    MISSION_PAUSING,
    PAUSE,
    MISSION_RESUMING,
    MISSION_DONE,
    RETURN,
    RETURN_DONE,
    DONE,
    ERROR
};

static MissionState _mission_state = MissionState::INIT;

//Mission::mission_result_t receive_mission_result;
static void receive_send_mission_result(Mission::Result result);
static void receive_start_mission_result(Mission::Result result);
static void receive_pause_mission_result(Mission::Result result);
static void receive_continue_mission_result(Mission::Result result);
static void receive_set_current_mission_item_result(Mission::Result result);
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
                                                 MissionItem::CameraAction camera_action);



TEST_F(SitlTest, MissionAddWaypointsAndFly)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Device ready, let's start" << std::endl;

    std::vector<std::shared_ptr<MissionItem>> mission_items;

    mission_items.push_back(
        add_waypoint(47.398170327054473,
                     8.5456490218639658,
                     10.0f, 5.0f, false,
                     20.0f, 60.0f,
                     MissionItem::CameraAction::NONE));

    mission_items.push_back(
        add_waypoint(47.398241338125118,
                     8.5455360114574432,
                     10.0f, 2.0f, true,
                     0.0f, -60.0f,
                     MissionItem::CameraAction::TAKE_PHOTO));

    mission_items.push_back(
        add_waypoint(47.398139363821485, 8.5453846156597137,
                     10.0f, 5.0f, true,
                     -46.0f, 0.0f,
                     MissionItem::CameraAction::START_VIDEO));

    mission_items.push_back(
        add_waypoint(47.398058617228855,
                     8.5454618036746979,
                     10.0f, 2.0f, false,
                     -90.0f, 30.0f,
                     MissionItem::CameraAction::STOP_VIDEO));

    mission_items.push_back(
        add_waypoint(47.398100366082858,
                     8.5456969141960144,
                     10.0f, 5.0f, false,
                     -45.0f, -30.0f,
                     MissionItem::CameraAction::START_PHOTO_INTERVAL));

    mission_items.push_back(
        add_waypoint(47.398001890458097,
                     8.5455576181411743,
                     10.0f, 5.0f, false,
                     0.0f, 0.0f,
                     MissionItem::CameraAction::STOP_PHOTO_INTERVAL));

    bool finished = false;
    while (!finished) {
        static auto _last_state = MissionState::INIT;
        if (_last_state != _mission_state) {
            std::cout << "state: " << int(_mission_state) << std::endl;
            _last_state = _mission_state;
        }

        static bool _jump_done = false;

        switch (_mission_state) {
            case MissionState::INIT:
                device.mission().send_mission_async(
                    mission_items,
                    std::bind(&receive_send_mission_result, _1));
                _mission_state = MissionState::UPLOADING;
                break;
            case MissionState::UPLOADING:
                break;
            case MissionState::UPLOADING_DONE:
                std::cout << "arming!" << std::endl;
                if (!device.telemetry().armed()) {
                    device.action().arm_async(std::bind(&receive_arm_result, _1));
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
                device.mission().start_mission_async(
                    std::bind(&receive_start_mission_result, _1));
                _mission_state = MissionState::STARTING;
                break;
            case MissionState::STARTING:
                break;
            case MissionState::STARTING_DONE:
                _mission_state = MissionState::MISSION;

                device.mission().subscribe_progress(
                    std::bind(&receive_mission_progress, _1, _2));
                break;
            case MissionState::MISSION:
                // Pause after 15s
                std::this_thread::sleep_for(std::chrono::seconds(15));
                std::cout << "Pause mission again" << std::endl;
                device.mission().pause_mission_async(
                    std::bind(&receive_pause_mission_result, _1));
                _mission_state = MissionState::MISSION_PAUSING;
                break;
            case MissionState::MISSION_PAUSING:
                break;
            case MissionState::PAUSE:
                // pause 2s, then carry on
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "Start mission again" << std::endl;
                device.mission().start_mission_async(
                    std::bind(&receive_continue_mission_result, _1));
                _mission_state = MissionState::MISSION_RESUMING;
                break;
            case MissionState::MISSION_RESUMING:

                if (!_jump_done) {
                    // Jump back to previous mission item
                    std::cout << "Send current mission item" << std::endl;
                    device.mission().set_current_mission_item_async(
                        3,
                        std::bind(&receive_set_current_mission_item_result, _1));
                    _jump_done = true;
                }
                break;
            case MissionState::MISSION_DONE:
                device.action().return_to_launch_async(
                    std::bind(&receive_return_to_launch_result, _1));
                _mission_state = MissionState::RETURN;
                break;
            case MissionState::RETURN:
                if (!device.telemetry().in_air()) {
                    _mission_state = MissionState::RETURN_DONE;
                }
                break;
            case MissionState::RETURN_DONE:
                device.action().disarm_async(std::bind(&receive_disarm_result, _1));
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

void receive_send_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);

    if (result == Mission::Result::SUCCESS) {
        _mission_state = MissionState::UPLOADING_DONE;
    } else {
        std::cerr << "Error: mission send result: " << unsigned(result) << std::endl;
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

void receive_pause_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);

    if (result == Mission::Result::SUCCESS) {
        _mission_state = MissionState::PAUSE;
    } else {
        std::cerr << "Error: mission start result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}

void receive_continue_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);

    if (result == Mission::Result::SUCCESS) {
        _mission_state = MissionState::MISSION_RESUMING;
    } else {
        std::cerr << "Error: mission start result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}

void receive_set_current_mission_item_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::SUCCESS);
    std::cout << "got answer: " << int(result) << std::endl;

    if (result != Mission::Result::SUCCESS) {
        std::cerr << "Got set current mission item result: " << int(result) << std::endl;
    }
}

void receive_mission_progress(int current, int total)
{
    std::cout << "Mission status update: " << current << " / " << total << std::endl;

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
        std::cerr << "Error: arming result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}

void receive_return_to_launch_result(Action::Result result)
{
    EXPECT_EQ(result, Action::Result::SUCCESS);

    if (result == Action::Result::SUCCESS) {
    } else {
        std::cerr << "Error: return to land result: " << unsigned(result) << std::endl;
        _mission_state = MissionState::ERROR;
    }
}


void receive_disarm_result(Action::Result result)
{
    EXPECT_EQ(result, Action::Result::SUCCESS);

    if (result == Action::Result::SUCCESS) {
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
                                          MissionItem::CameraAction camera_action)
{
    std::shared_ptr<MissionItem> new_item(new MissionItem());
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    new_item->set_speed(speed_m_s);
    new_item->set_fly_through(is_fly_through);
    new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    new_item->set_camera_action(camera_action);
    return new_item;
}
