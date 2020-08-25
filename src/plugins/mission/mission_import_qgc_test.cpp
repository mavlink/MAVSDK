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
static const std::string QGC_COMPLEX_SAMPLE_PLAN =
    "src/plugins/mission/qgroundcontrol_sample_with_survey.plan";

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

// capture array size
template<size_t N>
static void build_local_mission_items(
    QGCMissionItem (&items_test)[N], std::vector<Mission::MissionItem>& mission_items_local)
{
    MissionItem new_mission_item{};
    Mission::Result result = Mission::Result::Success;

    for (auto& qgc_it : items_test) {
        auto command = qgc_it.command;
        auto params = qgc_it.params;
        result = compose_mission_items(command, params, new_mission_item, mission_items_local);
        EXPECT_EQ(result, Mission::Result::Success);
    }
    mission_items_local.push_back(new_mission_item);
}

static void test_mission_items(
    std::string qgc_mission_plan_path, std::vector<Mission::MissionItem>& expected_mission_items)
{
    // Import Mission items from QGC plan
    auto import_result = MissionImpl::import_qgroundcontrol_mission(qgc_mission_plan_path);
    ASSERT_EQ(import_result.first, Mission::Result::Success);
    EXPECT_NE(import_result.second.mission_items.size(), 0);

    // Compare local & parsed mission items
    ASSERT_EQ(expected_mission_items.size(), import_result.second.mission_items.size());
    for (unsigned i = 0; i < import_result.second.mission_items.size(); ++i) {
        compare(expected_mission_items.at(i), import_result.second.mission_items.at(i));
    }
}

TEST(QGCComplextMissionImport, ValidateQGCComplexMissionItems)
{
    // These mission items are meant to match those in
    // file:://plugins/mission/qgroundcontrol_sample_with_survey.plan
    QGCMissionItem items_test[] = {
        {MAV_CMD_NAV_TAKEOFF, {0., 0., 0., double(NAN), 47.39781011, 8.54553801, 15.}},
        // Survey starts here
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39784192965106, 8.545413449078293, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39790440182785, 8.545317879157443, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {250., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397974177081146, 8.545211136580374, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {0., 0., 0., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.398036649089796, 8.545115566179781, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39800123401638, 8.545074115797751, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39793876204219, 8.545169686183264, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {250., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397879855533304, 8.545259801757693, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {0., 0., 0., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.3978173834048, 8.545355371702888, 50}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397792837129174, 8.545297294381587, 50}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397855309209426, 8.545201724412054, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {250., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397894043423285, 8.545142468524519, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {0., 0., 0., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397956515374865, 8.545046898187849, 50}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.397911199101806, 8.545020594879665, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39784872717209, 8.545116165165322, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {250., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39783076285621, 8.545143647120529, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {0., 0., 0., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39776829082423, 8.545239217114402, 50}},
        // Survey ends here
        // Corridor Scan starts here
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.3976842915956, 8.545111950891618, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {25., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39767426032188, 8.545243979383299, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {25., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39766439222148, 8.545373858243078, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39766180157361, 8.545586905499258, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39779107211276, 8.545911807235703, 50}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.398005467277635, 8.545879128614397, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39809492445337, 8.54586549325971, 50}},
        {MAV_CMD_NAV_WAYPOINT,
         {0., 0., 0., double(NAN), 47.398099539336236, 8.545931571871845, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39801008215267, 8.54594520711554, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {25., 0., 1., 0., 0., 0., 0.}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39776694725207, 8.545982265990538, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.3976166151349, 8.545604428957498, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39761947130397, 8.545369547331699, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39762957485054, 8.545236569673113, 50}},
        {MAV_CMD_NAV_WAYPOINT, {0., 0., 0., double(NAN), 47.39763960611574, 8.545104541291987, 50}},
        {MAV_CMD_DO_SET_CAM_TRIGG_DIST, {0., 0., 0., 0., 0., 0., 0.}},
        // Corridor Scan ends here
        {MAV_CMD_NAV_RETURN_TO_LAUNCH, {0., 0., 0., 0., 0., 0., 0.}}};

    // Build mission items for comparison
    std::vector<Mission::MissionItem> mission_items_local;
    build_local_mission_items<>(items_test, mission_items_local);
    test_mission_items(QGC_COMPLEX_SAMPLE_PLAN, mission_items_local);
}

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
    build_local_mission_items(items_test, mission_items_local);

    test_mission_items(QGC_SAMPLE_PLAN, mission_items_local);
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
