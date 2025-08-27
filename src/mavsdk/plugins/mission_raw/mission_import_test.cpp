#include <algorithm>
#include <atomic>
#include <cmath>
#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <string>
#include <gtest/gtest.h>

#include "log.h"
#include "mavlink_include.h"
#include "plugins/mission_raw/mission_raw.h"
#include "mission_import.h"

using namespace mavsdk;

std::string path_prefix(const std::string& path)
{
    return std::string("src/mavsdk/plugins/mission_raw/test_plans/") + path;
}

std::vector<MissionRaw::MissionItem> create_reference_items()
{
    return {
        {0,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_TAKEOFF,
         1, // current
         1,
         15.0f,
         0.0f,
         0.0f,
         NAN,
         473977507,
         85456075,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {1,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473977711,
         85466122,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {2,
         MAV_FRAME_MISSION,
         MAV_CMD_IMAGE_START_CAPTURE,
         0,
         1,
         0.0f,
         0.0f,
         1.0f,
         0.0f,
         0,
         0,
         0.0f,
         MAV_MISSION_TYPE_MISSION},
        {3,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473982738,
         85466053,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {4,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473982784,
         85456082,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {5,
         MAV_FRAME_MISSION,
         MAV_CMD_NAV_RETURN_TO_LAUNCH,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         0.0f,
         0,
         0,
         0.0f,
         MAV_MISSION_TYPE_MISSION},
    };
}

TEST(MissionRaw, ImportSamplePlanSuccessfully)
{
    const auto reference_items = create_reference_items();

    std::ifstream file{path_prefix("qgroundcontrol_sample.plan")};
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);

    EXPECT_EQ(reference_items, result_pair.second.mission_items);
}

TEST(MissionRaw, ImportSamplePlanWithWrongOverallVersion)
{
    std::ifstream file(path_prefix("qgroundcontrol_sample_wrong_overall_version.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseQgcPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithWrongMissionVersion)
{
    std::ifstream file(path_prefix("qgroundcontrol_sample_wrong_mission_version.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseQgcPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithoutMission)
{
    std::ifstream file(path_prefix("qgroundcontrol_sample_without_mission.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseQgcPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithComplexMissionSurvey)
{
    const std::vector<MissionRaw::MissionItem> reference_items = {
        {0,
         MAV_FRAME_MISSION,
         MAV_CMD_SET_CAMERA_MODE,
         1, // current
         1,
         0.0f,
         2.0f,
         NAN,
         NAN,
         0,
         0,
         NAN,
         MAV_MISSION_TYPE_MISSION},
        {1,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473977060,
         85463397,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {2,
         MAV_FRAME_MISSION,
         MAV_CMD_DO_SET_CAM_TRIGG_DIST,
         0,
         1,
         25.0f,
         0.0f,
         1.0f,
         0.0f,
         0,
         0,
         0.0f,
         MAV_MISSION_TYPE_MISSION},
        {3,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473977060,
         85462068,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {4,
         MAV_FRAME_MISSION,
         MAV_CMD_DO_SET_CAM_TRIGG_DIST,
         0,
         1,
         25.0f,
         0.0f,
         1.0f,
         0.0f,
         0,
         0,
         0.0f,
         MAV_MISSION_TYPE_MISSION},
        {5,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473977060,
         85457443,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {6,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473977060,
         85456115,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {7,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473974811,
         85456316,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {8,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473974811,
         85457645,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {9,
         MAV_FRAME_MISSION,
         MAV_CMD_DO_SET_CAM_TRIGG_DIST,
         0,
         1,
         25.0f,
         0.0f,
         1.0f,
         0.0f,
         0,
         0,
         0.0f,
         MAV_MISSION_TYPE_MISSION},
        {10,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473974811,
         85462092,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {11,
         MAV_FRAME_GLOBAL_RELATIVE_ALT,
         MAV_CMD_NAV_WAYPOINT,
         0,
         1,
         0.0f,
         0.0f,
         0.0f,
         NAN,
         473974811,
         85463421,
         50.0f,
         MAV_MISSION_TYPE_MISSION},
        {12,
         MAV_FRAME_MISSION,
         MAV_CMD_DO_SET_CAM_TRIGG_DIST,
         0,
         1,
         0.0f,
         0.0f,
         1.0f,
         0.0f,
         0,
         0,
         0.0f,
         MAV_MISSION_TYPE_MISSION}};

    std::ifstream file(path_prefix("qgroundcontrol_sample_with_survey.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::Success);
    EXPECT_EQ(reference_items, result_pair.second.mission_items);

    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithComplexMissionSurveyWrongVersion)
{
    std::ifstream file(path_prefix("qgroundcontrol_sample_with_survey_wrong_version.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseQgcPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithComplexMissionStructuredScan)
{
    std::ifstream file(path_prefix("qgroundcontrol_sample_with_structured_scan.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseQgcPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithComplexMissionSurveyMissingItems)
{
    std::ifstream file(path_prefix("qgroundcontrol_sample_with_survey_missing_items.plan"));
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseQgcPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportSamplePlanWithArduPilot)
{
    auto reference_items = create_reference_items();

    // We need to adjust the sequence plus one because we're about to add in home
    // at 0.

    std::for_each(reference_items.begin(), reference_items.end(), [](auto& item) { ++item.seq; });

    auto home_item = MissionRaw::MissionItem{
        0,
        MAV_FRAME_GLOBAL_INT,
        MAV_CMD_NAV_WAYPOINT,
        0, // current
        1, // autocontinue
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        473977507,
        85456075,
        488.93101752001763f,
        MAV_MISSION_TYPE_MISSION};

    reference_items.insert(reference_items.begin(), home_item);

    std::ifstream file{path_prefix("qgroundcontrol_sample.plan")};
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::ArduPilot);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);

    EXPECT_EQ(reference_items, result_pair.second.mission_items);
}

TEST(MissionRaw, ImportSamplePlanWithDigicamControl)
{
    auto digicam_control_items = std::vector<MissionRaw::MissionItem>{
        {0,
         MAV_FRAME_MISSION,
         MAV_CMD_DO_DIGICAM_CONTROL,
         1, // current
         1, // autocontinue
         1,
         0,
         0,
         0,
         1,
         0,
         0,
         MAV_MISSION_TYPE_MISSION}};

    std::ifstream file{path_prefix("qgroundcontrol_sample_with_digicam_control.plan")};
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_json(buf.str(), Autopilot::Px4);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);

    EXPECT_EQ(digicam_control_items, result_pair.second.mission_items);
}

std::vector<MissionRaw::MissionItem> create_mission_planner_reference_items()
{
    return {
        {0,
         MAV_FRAME_GLOBAL_INT, // frame
         MAV_CMD_NAV_TAKEOFF,
         1, // current
         1, // autocontinue
         0.0f, // param1
         0.0f, // param2
         0.0f, // param3
         0.0f, // param4
         473978101, // x (lat * 1e7)
         85455380, // y (lon * 1e7)
         15.0f, // z
         MAV_MISSION_TYPE_MISSION},
        {1,
         MAV_FRAME_GLOBAL_INT, // frame
         MAV_CMD_NAV_WAYPOINT,
         0, // current
         1, // autocontinue
         0.0f, // param1
         0.0f, // param2
         0.0f, // param3
         0.0f, // param4
         473977992, // x (lat * 1e7)
         85454669, // y (lon * 1e7)
         15.0f, // z
         MAV_MISSION_TYPE_MISSION},
        {2,
         MAV_FRAME_GLOBAL_INT, // frame
         MAV_CMD_NAV_WAYPOINT,
         0, // current
         1, // autocontinue
         0.0f, // param1
         0.0f, // param2
         0.0f, // param3
         0.0f, // param4
         473977883, // x (lat * 1e7)
         85453958, // y (lon * 1e7)
         15.0f, // z
         MAV_MISSION_TYPE_MISSION},
        {3,
         MAV_FRAME_GLOBAL_INT, // frame
         MAV_CMD_NAV_RETURN_TO_LAUNCH,
         0, // current
         1, // autocontinue
         0.0f, // param1
         0.0f, // param2
         0.0f, // param3
         0.0f, // param4
         473977774, // x (lat * 1e7)
         85453247, // y (lon * 1e7)
         0.0f, // z
         MAV_MISSION_TYPE_MISSION}};
}

TEST(MissionRaw, ImportMissionPlannerSuccessfully)
{
    auto reference_items = create_mission_planner_reference_items();

    std::ifstream file{path_prefix("mission_planner_sample.txt")};
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_mission_planner(buf.str(), Autopilot::Px4);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);

    EXPECT_EQ(reference_items.size(), result_pair.second.mission_items.size());
    for (size_t i = 0; i < reference_items.size(); ++i) {
        EXPECT_EQ(reference_items[i].seq, result_pair.second.mission_items[i].seq);
        EXPECT_EQ(reference_items[i].command, result_pair.second.mission_items[i].command);
        EXPECT_EQ(reference_items[i].x, result_pair.second.mission_items[i].x);
        EXPECT_EQ(reference_items[i].y, result_pair.second.mission_items[i].y);
        EXPECT_EQ(reference_items[i].z, result_pair.second.mission_items[i].z);
        EXPECT_EQ(
            reference_items[i].mission_type, result_pair.second.mission_items[i].mission_type);
    }

    // Mission Planner format doesn't include geofence or rally points
    EXPECT_EQ(result_pair.second.geofence_items.size(), 0);
    EXPECT_EQ(result_pair.second.rally_items.size(), 0);
}

TEST(MissionRaw, ImportMissionPlannerWithArduPilotHomePosition)
{
    auto reference_items = create_mission_planner_reference_items();

    std::ifstream file{path_prefix("mission_planner_sample.txt")};
    ASSERT_TRUE(file);

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    const auto result_pair = MissionImport::parse_mission_planner(buf.str(), Autopilot::ArduPilot);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);

    // ArduPilot should have one extra item (home position) at the beginning
    EXPECT_EQ(reference_items.size() + 1, result_pair.second.mission_items.size());

    // First item should be home position
    const auto& home_item = result_pair.second.mission_items[0];
    EXPECT_EQ(home_item.seq, 0);
    EXPECT_EQ(home_item.command, MAV_CMD_NAV_WAYPOINT);
    EXPECT_EQ(home_item.frame, MAV_FRAME_GLOBAL_INT);
    EXPECT_EQ(home_item.current, 1); // home should be current
    EXPECT_EQ(home_item.mission_type, MAV_MISSION_TYPE_MISSION);

    // Subsequent items should match reference (with updated sequence numbers)
    for (size_t i = 0; i < reference_items.size(); ++i) {
        const auto& item = result_pair.second.mission_items[i + 1];
        EXPECT_EQ(item.seq, i + 1);
        EXPECT_EQ(item.command, reference_items[i].command);
        EXPECT_EQ(item.current, 0); // only home should be current
    }
}

TEST(MissionRaw, ImportMissionPlannerInvalidFormat)
{
    const std::string invalid_mission =
        "Invalid Mission Format\n0\t1\t0\t22\t0\t0\t0\t0\t47.39781011\t8.54553801\t15\t1\n";

    const auto result_pair = MissionImport::parse_mission_planner(invalid_mission, Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseMissionPlannerPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
}

TEST(MissionRaw, ImportMissionPlannerInvalidLineFormat)
{
    const std::string invalid_mission =
        "QGC WPL 110\n0\t1\t0\t22\t0\t0\t0\t0\t47.39781011\t8.54553801\t15\t1\t99\t88\n"; // too
                                                                                          // many
                                                                                          // fields

    const auto result_pair = MissionImport::parse_mission_planner(invalid_mission, Autopilot::Px4);
    EXPECT_EQ(result_pair.first, MissionRaw::Result::FailedToParseMissionPlannerPlan);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
}

TEST(MissionRaw, ImportMissionPlannerEmptyMission)
{
    const std::string empty_mission = "QGC WPL 110\n";

    const auto result_pair = MissionImport::parse_mission_planner(empty_mission, Autopilot::Px4);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);
    EXPECT_EQ(result_pair.second.mission_items.size(), 0);
}

TEST(MissionRaw, ImportMissionPlannerFromString)
{
    const std::string mission_string =
        "QGC WPL 110\n0\t1\t0\t22\t0\t0\t0\t0\t47.39781011\t8.54553801\t15\t1\n1\t0\t0\t16\t0\t0\t0\t0\t47.39779921\t8.54546693\t15\t1\n";

    const auto result_pair = MissionImport::parse_mission_planner(mission_string, Autopilot::Px4);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);
    EXPECT_EQ(result_pair.second.mission_items.size(), 2);

    // Verify first item
    const auto& first_item = result_pair.second.mission_items[0];
    EXPECT_EQ(first_item.seq, 0);
    EXPECT_EQ(first_item.command, MAV_CMD_NAV_TAKEOFF);
    EXPECT_EQ(first_item.current, 1);

    // Verify second item
    const auto& second_item = result_pair.second.mission_items[1];
    EXPECT_EQ(second_item.seq, 1);
    EXPECT_EQ(second_item.command, MAV_CMD_NAV_WAYPOINT);
    EXPECT_EQ(second_item.current, 0);
}

TEST(MissionRaw, ImportMissionPlannerWithGlobalFrame)
{
    // Test with MAV_FRAME_GLOBAL (0) which should also apply 1e7 scaling
    const std::string mission_string =
        "QGC WPL 110\n0\t1\t0\t22\t0\t0\t0\t0\t47.3978101\t8.5455380\t15\t1\n";

    const auto result_pair = MissionImport::parse_mission_planner(mission_string, Autopilot::Px4);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);
    EXPECT_EQ(result_pair.second.mission_items.size(), 1);

    const auto& item = result_pair.second.mission_items[0];
    EXPECT_EQ(item.frame, MAV_FRAME_GLOBAL);
    EXPECT_EQ(item.command, MAV_CMD_NAV_TAKEOFF);
    EXPECT_EQ(item.x, 473978101); // Should be scaled by 1e7
    EXPECT_EQ(item.y, 85455380); // Should be scaled by 1e7
}
