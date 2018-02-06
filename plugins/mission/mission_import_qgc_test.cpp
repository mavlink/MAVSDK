/**
  @file Unit test for #Mission::import_mission_items_from_QGC_plan().
  @author Shakthi Prashanth <shakthi.prashanth.m@intel.com>
*/

#include <gtest/gtest.h>
#include <atomic>
#include <atomic>
#include "mission.h"
#include "mavlink_include.h"
#include "global_include.h"
#include "log.h"

using namespace dronecore;

struct QGCMissionItem {
    MAV_CMD cmd;
    std::vector<double> params;
};

Mission::Result compose_mission_items(MAV_CMD cmd, std::vector<double> params,
                                      std::shared_ptr<MissionItem> &new_mission_item,
                                      Mission::mission_items_t &mission_items)
{
    Mission::Result result = Mission::Result::SUCCESS;
    static int i = 0;
    LogInfo() << ++i << "-> cmd " << cmd;

    // Choosen "Do-While(0)" loop for the convenience of using `break` statement.
    do {
        if (cmd == MAV_CMD_NAV_WAYPOINT ||
            cmd == MAV_CMD_NAV_TAKEOFF ||
            cmd == MAV_CMD_NAV_LAND ||
            cmd == MAV_CMD_NAV_LOITER_TIME ||
            cmd == MAV_CMD_NAV_RETURN_TO_LAUNCH) {
            // When we get Mission items with position/altitude contained,
            // it will become a new DroneCore mission item;
//            if (mission_items.size() != 0) { // If its non-first item
            mission_items.push_back(new_mission_item);
            new_mission_item = std::make_shared<MissionItem>();
//            }

            if (cmd == MAV_CMD_NAV_WAYPOINT) {
                auto is_fly_thru = !(int(params[0]) > 0);
                new_mission_item->set_fly_through(is_fly_thru);
            }
            if (cmd == MAV_CMD_NAV_LOITER_TIME) {
                auto loiter_time_s = float(params[0]);
                new_mission_item->set_loiter_time(loiter_time_s);
            }
//            if (cmd != MAV_CMD_NAV_TAKEOFF) {
            auto lat = params[4], lon = params[5];
            new_mission_item->set_position(lat, lon);
//            }
            auto rel_alt = float(params[6]);
            new_mission_item->set_relative_altitude(rel_alt);
            LogInfo() << cmd << ": Alt " << rel_alt;

        } else if (cmd == MAV_CMD_DO_MOUNT_CONTROL) {
            auto pitch = float(params[0]), yaw = float(params[2]);
            new_mission_item->set_gimbal_pitch_and_yaw(pitch, yaw);
            LogInfo() << cmd << ": Pitch " << pitch << ", Yaw " << yaw;

        } else if (cmd == MAV_CMD_IMAGE_START_CAPTURE) {
            auto photo_interval = int(params[1]),  photo_count = int(params[2]);

            if (photo_interval > 0 && photo_count == 0) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::START_PHOTO_INTERVAL);
                new_mission_item->set_camera_photo_interval(photo_interval);
            } else if (photo_interval == 0 && photo_count == 1) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::TAKE_PHOTO);
            } else {
                LogErr() << "Mission item START_CAPTURE params unsupported.";
                result = Mission::Result::UNSUPPORTED;
                break;
            }

        } else if (cmd == MAV_CMD_IMAGE_STOP_CAPTURE) {
            new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_PHOTO_INTERVAL);

        } else if (cmd == MAV_CMD_VIDEO_START_CAPTURE) {
            new_mission_item->set_camera_action(MissionItem::CameraAction::START_VIDEO);

        } else if (cmd == MAV_CMD_VIDEO_STOP_CAPTURE) {
            new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_VIDEO);

        } else if (cmd == MAV_CMD_DO_CHANGE_SPEED) {
            enum { AirSpeed, GroundSpeed };
            auto speed_type = int(params[0]);
            auto speed_m_s = float(params[1]);
            auto throttle = params[2];
            auto is_absolute = (params[3] == 0);

            if (speed_type == int(GroundSpeed) && throttle < 0 && is_absolute) {
                new_mission_item->set_speed(speed_m_s);
            } else {
                LogErr() << cmd << "Mission item DO_CHANGE_SPEED params unsupported";
                result = Mission::Result::UNSUPPORTED;
                break;
            }
        } else {
            LogWarn() << "UNSUPPORTED mission item command (" << cmd << ")";
        }
    } while (false); // Executed once per method invokation.


    return result;
}


static void compare(const std::shared_ptr<MissionItem> local,
                    const std::shared_ptr<MissionItem> imported);

TEST(QGCMissionImport, ValidateQGCMissonItems)
{
    // These mission items are meant to match those in
    // file:://example/fly_qgc_mission/qgroundcontrol_sample.plan
    QGCMissionItem items_test[] = {
        { MAV_CMD_DO_CHANGE_SPEED, { 1., 25., -1., 0., 0., 0., 0. } },
        { MAV_CMD_NAV_TAKEOFF, { 0., 0., 0., 0., 47.397815180625855, 8.545421242149587, 15. } },
        { MAV_CMD_DO_MOUNT_CONTROL, { 40., 0., 50., 0., 0., 0., 0. } },
        { MAV_CMD_IMAGE_START_CAPTURE, { 0., 0., 1., 0., 0., 0., 0. } },
        { MAV_CMD_NAV_WAYPOINT, { 0., 0., 0., 0., 47.39773165601306, 8.545165094893491, 15. } },
        { MAV_CMD_NAV_WAYPOINT, { 0., 0., 0., 0., 47.39765177077532, 8.545249586057452, 15. } },
        { MAV_CMD_NAV_WAYPOINT, { 0., 0., 0., 0., 47.397651769568846, 8.545434657161934, 15. } },
        { MAV_CMD_IMAGE_STOP_CAPTURE, { 0., 0., 0., 0., 0., 0., 0. } },
        { MAV_CMD_NAV_RETURN_TO_LAUNCH, { 0., 0., 0., 0., 0., 0., 0. } }
    };

    // Build mission items for comparison
    Mission::mission_items_t mission_items_local;
    auto new_mission_item = std::make_shared<MissionItem>();
    Mission::Result result = Mission::Result::SUCCESS;

    for (auto &qgc_it : items_test) {
        auto cmd = qgc_it.cmd;
        auto params = qgc_it.params;
        result = compose_mission_items(cmd, params, new_mission_item, mission_items_local);
        EXPECT_EQ(result, Mission::Result::SUCCESS);
    }
    mission_items_local.push_back(new_mission_item);
    LogInfo() << "# of mission items: " << mission_items_local.size();


    // Import Mission items from QGC plan
    Mission::mission_items_t mission_items_imported;
    Mission::Result import_result = Mission::import_mission_items_from_QGC_plan(
                                        mission_items_imported,
                                        "example/fly_qgc_mission/qgroundcontrol_sample.plan");
    ASSERT_EQ(import_result, Mission::Result::SUCCESS);
    EXPECT_NE(mission_items_imported.size(), 0);

    // Compare local & parsed mission items
    ASSERT_EQ(mission_items_local.size(), mission_items_imported.size());
    for (unsigned i = 0; i < mission_items_imported.size(); ++i) {
        LogInfo() << "************ Mission Item #" << i;
        compare(mission_items_local.at(i), mission_items_imported.at(i));
    }
}

void compare(const std::shared_ptr<MissionItem> local,
             const std::shared_ptr<MissionItem> imported)
{
    if (local->get_camera_action() == MissionItem::CameraAction::NONE) {
        // Non-Camera commands
        if (std::isfinite(local->get_latitude_deg())) {
            EXPECT_NEAR(local->get_latitude_deg(), imported->get_latitude_deg(), 1e-6);
        }
        if (std::isfinite(local->get_longitude_deg())) {
            EXPECT_NEAR(local->get_longitude_deg(), imported->get_longitude_deg(), 1e-6);
        }
        if (std::isfinite(local->get_relative_altitude_m())) {
            EXPECT_FLOAT_EQ(local->get_relative_altitude_m(),
                            imported->get_relative_altitude_m());
        }

        EXPECT_EQ(local->get_fly_through(), imported->get_fly_through());
        if (std::isfinite(local->get_speed_m_s())) {
            EXPECT_FLOAT_EQ(local->get_speed_m_s(), imported->get_speed_m_s());
        }
    }

    EXPECT_EQ(local->get_camera_action(), imported->get_camera_action());

    if (local->get_camera_action() == MissionItem::CameraAction::START_PHOTO_INTERVAL &&
        // Camera commands
        std::isfinite(local->get_camera_photo_interval_s())) {

        EXPECT_DOUBLE_EQ(local->get_camera_photo_interval_s(),
                         imported->get_camera_photo_interval_s());
    }

    if (std::isfinite(local->get_loiter_time_s())) {
        EXPECT_FLOAT_EQ(local->get_loiter_time_s(),
                        imported->get_loiter_time_s());
    }
}
