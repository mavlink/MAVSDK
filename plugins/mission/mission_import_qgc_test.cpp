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

// Anonymous enum is intentional to avoid explicit qualification.
enum {
    CHANGE_SPEED,
    TAKEOFF,
    GIMBAL_CTRL,
    IMG_START,
    WP_1,
    WP_2,
    WP_3,
    IMG_STOP,
    RTL
};

struct QGCMissionItem {
    static const size_t N_PARAMS = 7;
    MAV_CMD cmd;
    float params[N_PARAMS];
};

static std::shared_ptr<MissionItem> make_mission_item(double latitude_deg,
                                                      double longitude_deg,
                                                      float relative_altitude_m,
                                                      float speed_m_s = NAN,
                                                      bool is_fly_through = false,
                                                      float gimbal_pitch_deg = NAN,
                                                      float gimbal_yaw_deg = NAN,
                                                      float camera_photo_interval_s = NAN,
                                                      MissionItem::CameraAction camera_action = MissionItem::CameraAction::NONE);

static void compare(const std::shared_ptr<MissionItem> local,
                    const std::shared_ptr<MissionItem> imported);

TEST(QGCMissionImport, ValidateQGCMissonItems)
{
    // These mission items are meant to match those in
    // file:://plugins/mission/qgroundcontrol_sample.plan
    QGCMissionItem items_test[] = {
        { MAV_CMD_DO_CHANGE_SPEED, { 1.f, 25.f, -1.f, 0.f, 0.f, 0.f, 0.f } },
        { MAV_CMD_NAV_TAKEOFF, { 0.f, 0.f, 0.f, 0.f, 47.397815180625855f, 8.545421242149587f, 15.f } },
        { MAV_CMD_DO_MOUNT_CONTROL, { 40.f, 0.f, 50.f, 0.f, 0.f, 0.f, 0.f } },
        { MAV_CMD_IMAGE_START_CAPTURE, { 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f } },
        { MAV_CMD_NAV_WAYPOINT, { 0.f, 0.f, 0.f, 0.f, 47.39773165601306f, 8.545165094893491f, 15.f } },
        { MAV_CMD_NAV_WAYPOINT, { 0.f, 0.f, 0.f, 0.f, 47.39765177077532f, 8.545249586057452f, 15.f } },
        { MAV_CMD_NAV_WAYPOINT, { 0.f, 0.f, 0.f, 0.f, 47.397651769568846f, 8.545434657161934f, 15.f } },
        { MAV_CMD_IMAGE_STOP_CAPTURE, { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f } },
        { MAV_CMD_NAV_RETURN_TO_LAUNCH, { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f } }
    };

    // Build mission items for comparison
    Mission::mission_items_t mission_items_local;

    QGCMissionItem item = items_test[CHANGE_SPEED];
    mission_items_local.push_back(make_mission_item(double(NAN),
                                                    double(NAN),
                                                    NAN,
                                                    item.params[1],
                                                    (item.params[0] == 0.f),
                                                    NAN, NAN, NAN));

    item = items_test[TAKEOFF];
    mission_items_local.push_back(make_mission_item(double(item.params[4]),
                                                    double(item.params[5]),
                                                    item.params[6], NAN));

    item = items_test[GIMBAL_CTRL];
    mission_items_local.push_back(make_mission_item(double(NAN),
                                                    double(NAN),
                                                    NAN, NAN, false,
                                                    item.params[0],
                                                    item.params[2], NAN));

    item = items_test[IMG_START];
    mission_items_local.push_back(make_mission_item(double(item.params[4]),
                                                    double(item.params[5]),
                                                    item.params[6],
                                                    NAN,
                                                    false, NAN, NAN,
                                                    item.params[1],
                                                    MissionItem::CameraAction::START_PHOTO_INTERVAL));

    item = items_test[WP_1];
    mission_items_local.push_back(make_mission_item(double(item.params[4]),
                                                    double(item.params[5]),
                                                    item.params[6],
                                                    NAN, true));

    item = items_test[WP_2];
    mission_items_local.push_back(make_mission_item(double(item.params[4]),
                                                    double(item.params[5]),
                                                    item.params[6],
                                                    NAN, true));

    item = items_test[WP_3];
    mission_items_local.push_back(make_mission_item(double(item.params[4]),
                                                    double(item.params[5]),
                                                    item.params[6],
                                                    NAN, true));

    item = items_test[IMG_STOP];
    mission_items_local.push_back(make_mission_item(double(item.params[4]),
                                                    double(item.params[5]),
                                                    item.params[6], NAN,
                                                    false, NAN, NAN, 0.f,
                                                    MissionItem::CameraAction::STOP_PHOTO_INTERVAL));

    item = items_test[RTL];
    mission_items_local.push_back(make_mission_item(double(NAN), double(NAN), NAN, NAN));

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
        compare(mission_items_local.at(i), mission_items_imported.at(i));
    }
}

std::shared_ptr<MissionItem> make_mission_item(double latitude_deg,
                                               double longitude_deg,
                                               float relative_altitude_m,
                                               float speed_m_s,
                                               bool is_fly_through,
                                               float gimbal_pitch_deg,
                                               float gimbal_yaw_deg,
                                               float camera_photo_interval_s,
                                               MissionItem::CameraAction camera_action)
{
    auto new_item = std::make_shared<MissionItem>();
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    new_item->set_speed(speed_m_s);
    new_item->set_fly_through(is_fly_through);
    new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    new_item->set_camera_action(camera_action);
    // In order to test setting the interval, add it here.
    if (camera_action == MissionItem::CameraAction::START_PHOTO_INTERVAL) {
        new_item->set_camera_photo_interval(double(camera_photo_interval_s));
    }

    return new_item;
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
