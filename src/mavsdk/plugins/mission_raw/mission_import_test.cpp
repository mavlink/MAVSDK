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
