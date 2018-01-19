/**
* @file fly_mission.cpp
*
* @brief Demonstrates how add & fly waypoint missions from QGC plan using DroneCore.
* The example is summarised below:
* 1. Extracts mission items from QGroundControl mission plan (passed via command-line).
* 2. Compose mission items; uploads them to vehicle via Mission plugin interface.
* 3. Starts mission from first mission item.
* 4. Exits after the mission is accomplished.
* Note: Before you run this example, plan your missions in QGroundControl & save them to a file.
*
* @author Julian Oes <julian@oes.ch>,
*         Shakthi Prashanth M <shakthi.prashanth.m@intel.com>

* @date 2017-09-06
*/


#include <dronecore/dronecore.h>
#include <iostream>
#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <cstdlib>
#include <cmath>
#include <functional>
#include <memory>
#include <future>
#include <json11.hpp> // for JSON parsing
#include "mavlink_include.h" // for MAVLink commands

using namespace dronecore;
using namespace std::placeholders; // for `_1`
using namespace std::this_thread; // for sleep_for()
using namespace std::chrono; // for milliseconds(), seconds().
using namespace json11;

static std::vector<std::shared_ptr<MissionItem>> get_mission_items_from_QGC_plan(
                                                  const std::string &qgc_plan_file);
static std::vector<std::shared_ptr<MissionItem>> compose_mission_items_from_json(
                                                  const Json &mission_json);
static std::shared_ptr<MissionItem> add_mission_item(double latitude_deg,
                                                     double longitude_deg,
                                                     float relative_altitude_m,
                                                     float speed_m_s,
                                                     bool is_fly_through,
                                                     float gimbal_pitch_deg,
                                                     float gimbal_yaw_deg,
                                                     MissionItem::CameraAction camera_action);

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: fly_mission <path of QGC mission plan file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    DroneCore dc;

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        std::cout << "Waiting to discover device..." << std::endl;
        dc.register_on_discover([prom](uint64_t uuid) {
            std::cout << "Discovered device with UUID: " << uuid << std::endl;
            prom->set_value();
        });

        DroneCore::ConnectionResult connection_result = dc.add_udp_connection();
        if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
            std::cout << "Connection failed: " << DroneCore::connection_result_str(
                          connection_result) << std::endl;
            exit(EXIT_FAILURE);
        }

        future_result.get();
    }

    dc.register_on_timeout([](uint64_t uuid) {
        std::cout << "Device with UUID timed out: " << uuid << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(EXIT_SUCCESS);
    });

    // We don't need to specifiy the UUID if it's only one device anyway.
    // If there were multiple, we could specify it with:
    // dc.device(uint64_t uuid);
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "Waiting for device to be ready" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "Device ready" << std::endl;

    std::vector<std::shared_ptr<MissionItem>> mission_items;
    try {
        mission_items = get_mission_items_from_QGC_plan(argv[1]);
    } catch (std::exception const &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        abort();
    }

    if (mission_items.size() == 0) {
        std::cerr << "No missions! Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Found " << mission_items.size() << " mission items in the given QGC plan." <<
              std::endl;

    {
        std::cout << "Uploading mission..." << std::endl;
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        device.mission().upload_mission_async(
        mission_items, [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Mission upload failed (" << Mission::result_str(result) << "), exiting." << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Mission uploaded." << std::endl;
    }

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = device.action().arm();
    if (arm_result != Action::Result::SUCCESS) {
        std::cout << "Arming failed (" << Action::result_str(arm_result) << "), exiting." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Armed." << std::endl;

    std::atomic<bool> want_to_pause {false};
    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    device.mission().subscribe_progress(
    [&want_to_pause](int current, int total) {
        std::cout << "Mission status update: " << current << " / " << total << std::endl;

        if (current >= 2) {
            // We can only set a flag here. If we do more request inside the callback,
            // we risk blocking the system.
            want_to_pause = true;
        }
    });

    {
        std::cout << "Starting mission." << std::endl;
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        device.mission().start_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
            std::cout << "Started mission." << std::endl;
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Mission start failed (" << Mission::result_str(result) << "), exiting." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    while (!want_to_pause) {
        sleep_for(seconds(1));
    }

    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();

        std::cout << "Pausing mission..." << std::endl;
        device.mission().pause_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Failed to pause mission (" << Mission::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Mission paused." << std::endl;
        }
    }

    // Pause for 5 seconds.
    sleep_for(seconds(5));

    // Then continue.
    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();

        std::cout << "Resuming mission..." << std::endl;
        device.mission().start_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Failed to resume mission (" << Mission::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Resumed mission." << std::endl;
        }
    }

    while (!device.mission().mission_finished()) {
        sleep_for(seconds(1));
    }

    {
        // We are done, and can do RTL to go home.
        std::cout << "Commanding RTL..." << std::endl;
        const Action::Result result = device.action().return_to_launch();
        if (result != Action::Result::SUCCESS) {
            std::cout << "Failed to command RTL (" << Action::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Commanded RTL." << std::endl;
        }
    }

    // We need to wait a bit, otherwise the armed state might not be correct yet.
    sleep_for(seconds(2));

    while (device.telemetry().armed()) {
        // Wait until we're done.
        sleep_for(seconds(1));
    }
    std::cout << "Disarmed, exiting." << std::endl;
}

std::shared_ptr<MissionItem> add_mission_item(double latitude_deg,
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

std::vector<std::shared_ptr<MissionItem>> get_mission_items_from_QGC_plan(
                                           const std::string &qgc_plan_file)
{
    std::vector<std::shared_ptr<MissionItem>> mission_items;
    {
        try {
            std::ifstream file(qgc_plan_file);
            if (!file.is_open()) {
                throw std::runtime_error("Error opening QGC plan" + qgc_plan_file + "\n");
            }

            std::stringstream ss;
            ss << file.rdbuf();
            file.close();

            std::string err;
            const auto parsed_plan = Json::parse(ss.str(), err); // parse QGC plan
            if (!err.empty()) {
                // TODO Error handling
                throw std::runtime_error("Error in loading waypoints from the file: " + qgc_plan_file);
            }
            mission_items = compose_mission_items_from_json(parsed_plan["mission"]);
        } catch (std::exception const &e) {
            std::cerr << e.what();
            throw;
        }
        return mission_items;
    }
}

std::vector<std::shared_ptr<MissionItem>> compose_mission_items_from_json(const Json &mission_json)
{
    std::vector<std::shared_ptr<MissionItem>> mission_items;
    // NOTE: Unexpected type while reading values will cause an exception.
    for (auto &mission_item : mission_json["items"].array_items()) {
        // Parameters of Mission item & MAV command of it.
        float speed_m_s = 0.f;
        float gimbal_pitch_deg = 0.f, gimbal_yaw_deg = 0.f;
        bool is_fly_through = false;
        double lat_deg = NAN, lon_deg = NAN, rel_alt_deg = NAN;
        size_t no_of_photos = 0;
        auto camera_action = MissionItem::CameraAction::NONE;
        auto command = mission_item["command"].int_value();

        // Extract parameters
        std::vector<double> params;
        for (auto &p : mission_item["params"].array_items()) {
            params.push_back(p.number_value());
        }

        // This looks bit complicated at app users.
        // Should we handle them in plugin ?
        switch (command) {
            case MAV_CMD_IMAGE_START_CAPTURE:
                no_of_photos = static_cast<size_t>(params[3]);
                if (no_of_photos > 1) {
                    camera_action = MissionItem::CameraAction::START_PHOTO_INTERVAL;
                } else if (no_of_photos == 1) {
                    camera_action = MissionItem::CameraAction::TAKE_PHOTO;
                }
                break;
            case MAV_CMD_VIDEO_START_STREAMING:
                camera_action = MissionItem::CameraAction::START_VIDEO;
                break;
            case MAV_CMD_VIDEO_STOP_STREAMING:
                camera_action = MissionItem::CameraAction::START_VIDEO;
                break;
            case MAV_CMD_DO_CHANGE_SPEED:
                speed_m_s = params[2];
                break;
            case MAV_CMD_DO_MOUNT_CONTROL:
                // Possible bug in QGroundcontrol. It stores -ve values for pitch in deg.
                gimbal_pitch_deg = -params[0];
                gimbal_yaw_deg = params[3];
                break;
            case MAV_CMD_NAV_WAYPOINT:
                is_fly_through = (params[0] == 0.0) ? true : false;
            case MAV_CMD_NAV_TAKEOFF:
                lat_deg = params[4];
                lon_deg = params[5];
                rel_alt_deg = params[6];
                break;
        }
        // Add a mission item
        mission_items.push_back(add_mission_item(lat_deg,
                                                 lon_deg,
                                                 rel_alt_deg,
                                                 speed_m_s,
                                                 is_fly_through,
                                                 gimbal_pitch_deg,
                                                 gimbal_yaw_deg,
                                                 camera_action));
    }
    return mission_items;
}
