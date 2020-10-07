#include <atomic>
#include <cmath>
#include <gtest/gtest.h>

#include "global_include.h"
#include "log.h"
#include "mavlink_include.h"
#include "plugins/mission/mission.h"
#include "mission_impl.h"

using namespace mavsdk;
using MissionItem = Mission::MissionItem;
using CameraAction = Mission::MissionItem::CameraAction;

static const std::string QGC_SAMPLE_PLAN = "src/plugins/mission/qgroundcontrol_sample.plan";

struct QGCMissionItem {
    MAV_CMD command;
    std::vector<double> params;
};

Mission::Result compose_mission_items(
    MAV_CMD command,
    std::vector<double> params,
    MissionItem& new_mission_item,
    std::vector<Mission::MissionItem>& mission_items);

static void compare(const MissionItem& local, const MissionItem& imported);

TEST(QGCMissionImport, ValidateQGCMissonItems)
{
    // These mission items are meant to match those in
    // file:://plugins/mission/qgroundcontrol_sample.plan
    QGCMissionItem items_test[] = {
        {MAV_CMD_NAV_TAKEOFF, {0., 0., 0., double(NAN), 47.39781011, 8.54553801, 15.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39779921, 8.54546693, 15.}},
        {MAV_CMD_DO_MOUNT_CONTROL, {25.0, 0., 50.0}}, // Gimbal pitch & yaw in deg
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., 0., 47.39773658, 8.54543743, 15.}},
        {MAV_CMD_IMAGE_START_CAPTURE,
         {
             0.,
             0.,
             1.,
         }}, // Take 1 photo
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., 0., 47.39768937, 8.54548034, 15.}},
        {MAV_CMD_IMAGE_START_CAPTURE,
         {
             0,
             1.,
             0.,
         }}, // Start image capture
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39768029, 8.54561177, 15.}},
        {MAV_CMD_DO_CHANGE_SPEED, {1., 100., -1., 0}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39779649, 8.54566005, 15.}},
        {MAV_CMD_NAV_LOITER_TIME, {30.}}, // Loiter for 30 seconds
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39779468, 8.54561445, 15.}},
        {MAV_CMD_VIDEO_START_CAPTURE, {}}, // Start video capture
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39784279, 8.54553533, 15.}},
        {MAV_CMD_IMAGE_STOP_CAPTURE, {}}, // Stop image capture
        {MAV_CMD_VIDEO_STOP_CAPTURE, {}}, // Stop video capture
    };

    // Build mission items for comparison
    std::vector<Mission::MissionItem> mission_items_local;
    MissionItem new_mission_item{};
    Mission::Result result = Mission::Result::Success;

    for (auto& qgc_it : items_test) {
        auto command = qgc_it.command;
        auto params = qgc_it.params;
        result = compose_mission_items(command, params, new_mission_item, mission_items_local);
        EXPECT_EQ(result, Mission::Result::Success);
    }
    mission_items_local.push_back(new_mission_item);

    // Import Mission items from QGC plan
    auto import_result = MissionImpl::import_qgroundcontrol_mission(QGC_SAMPLE_PLAN);
    ASSERT_EQ(import_result.first, Mission::Result::Success);
    EXPECT_NE(import_result.second.mission_items.size(), 0);

    // Compare local & parsed mission items
    ASSERT_EQ(mission_items_local.size(), import_result.second.mission_items.size());
    for (unsigned i = 0; i < import_result.second.mission_items.size(); ++i) {
        compare(mission_items_local.at(i), import_result.second.mission_items.at(i));
    }
}

Mission::Result compose_mission_items(
    MAV_CMD command,
    std::vector<double> params,
    MissionItem& new_mission_item,
    std::vector<Mission::MissionItem>& mission_items)
{
    Mission::Result result = Mission::Result::Success;

    // Choosen "Do - While(0)" loop for the convenience of using `break` statement.
    do {
        if (command == MAV_CMD_NAV_WAYPOINT || command == MAV_CMD_NAV_TAKEOFF ||
            command == MAV_CMD_NAV_LAND) {
            if (std::isfinite(new_mission_item.latitude_deg) &&
                std::isfinite(new_mission_item.longitude_deg) &&
                std::isfinite(new_mission_item.relative_altitude_m)) {
                mission_items.push_back(new_mission_item);
                new_mission_item = {};
            }
            if (command == MAV_CMD_NAV_WAYPOINT) {
                auto is_fly_thru = !(int(params[0]) > 0);
                new_mission_item.is_fly_through = is_fly_thru;
            }
            auto lat = params[4], lon = params[5];
            new_mission_item.latitude_deg = lat;
            new_mission_item.longitude_deg = lon;

            auto rel_alt = float(params[6]);
            new_mission_item.relative_altitude_m = rel_alt;

        } else if (command == MAV_CMD_DO_MOUNT_CONTROL) {
            auto pitch = float(params[0]), yaw = float(params[2]);
            new_mission_item.gimbal_pitch_deg = pitch;
            new_mission_item.gimbal_yaw_deg = yaw;

        } else if (command == MAV_CMD_NAV_LOITER_TIME) {
            auto loiter_time_s = float(params[0]);
            new_mission_item.loiter_time_s = loiter_time_s;

        } else if (command == MAV_CMD_IMAGE_START_CAPTURE) {
            auto photo_interval = int(params[1]), photo_count = int(params[2]);

            if (photo_interval > 0 && photo_count == 0) {
                new_mission_item.camera_action = CameraAction::StartPhotoInterval;
                new_mission_item.camera_photo_interval_s = photo_interval;
            } else if (photo_interval == 0 && photo_count == 1) {
                new_mission_item.camera_action = CameraAction::TakePhoto;
            } else {
                LogErr() << "Mission item START_CAPTURE params unsupported.";
                result = Mission::Result::Unsupported;
                break;
            }

        } else if (command == MAV_CMD_IMAGE_STOP_CAPTURE) {
            new_mission_item.camera_action = CameraAction::StopPhotoInterval;

        } else if (command == MAV_CMD_VIDEO_START_CAPTURE) {
            new_mission_item.camera_action = CameraAction::StartVideo;

        } else if (command == MAV_CMD_VIDEO_STOP_CAPTURE) {
            new_mission_item.camera_action = CameraAction::StopVideo;

        } else if (command == MAV_CMD_DO_CHANGE_SPEED) {
            enum { AirSpeed, GroundSpeed };
            auto speed_type = int(params[0]);
            auto speed_m_s = float(params[1]);
            auto throttle = params[2];
            auto is_absolute = (params[3] == 0);

            if (speed_type == int(GroundSpeed) && throttle < 0 && is_absolute) {
                new_mission_item.speed_m_s = speed_m_s;
            } else {
                LogErr() << command << "Mission item DO_CHANGE_SPEED params unsupported";
                result = Mission::Result::Unsupported;
                break;
            }
        } else {
            LogWarn() << "UNSUPPORTED mission item command(" << command << ")";
        }
    } while (false); // Executed once per method invokation.

    return result;
}

void compare(const MissionItem& local, const MissionItem& imported)
{
    if (local.camera_action == CameraAction::None) {
        // Non-Camera commands
        if (std::isfinite(local.latitude_deg)) {
            EXPECT_NEAR(local.latitude_deg, imported.latitude_deg, 1e-6);
        }
        if (std::isfinite(local.longitude_deg)) {
            EXPECT_NEAR(local.longitude_deg, imported.longitude_deg, 1e-6);
        }
        if (std::isfinite(local.relative_altitude_m)) {
            EXPECT_FLOAT_EQ(local.relative_altitude_m, imported.relative_altitude_m);
        }

        EXPECT_EQ(local.is_fly_through, imported.is_fly_through);
        if (std::isfinite(local.speed_m_s)) {
            EXPECT_FLOAT_EQ(local.speed_m_s, imported.speed_m_s);
        }
    }

    EXPECT_EQ(local.camera_action, imported.camera_action);

    if (local.camera_action == CameraAction::StartPhotoInterval &&
        // Camera commands
        std::isfinite(local.camera_photo_interval_s)) {
        EXPECT_DOUBLE_EQ(local.camera_photo_interval_s, imported.camera_photo_interval_s);
    }

    if (std::isfinite(local.loiter_time_s)) {
        EXPECT_FLOAT_EQ(local.loiter_time_s, imported.loiter_time_s);
    }
}
