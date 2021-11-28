#include <gtest/gtest.h>
#include <limits>

#include "plugins/mission/mission.h"

using namespace mavsdk;

TEST(MissionOperator, MissionItemEqualsOperatorIsValidForDefaultItems)
{
    Mission::MissionItem mission_item1;
    Mission::MissionItem mission_item2;

    ASSERT_EQ(mission_item1, mission_item2);
}

TEST(MissionOperator, MissionItemEqualsOperatorIsValidForArbitraryItems)
{
    Mission::MissionItem mission_item1;
    Mission::MissionItem mission_item2;

    mission_item1.latitude_deg = 47.3982;
    mission_item1.longitude_deg = 8.54567;
    mission_item1.relative_altitude_m = 13.8f;
    mission_item1.speed_m_s = 6.9f;
    mission_item1.is_fly_through = 0;
    mission_item1.gimbal_pitch_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item1.gimbal_yaw_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item1.camera_action = Mission::MissionItem::CameraAction::None;
    mission_item1.loiter_time_s = std::numeric_limits<double>::quiet_NaN();
    mission_item1.camera_photo_interval_s = 1;

    mission_item2.latitude_deg = 47.3982;
    mission_item2.longitude_deg = 8.54567;
    mission_item2.relative_altitude_m = 13.8f;
    mission_item2.speed_m_s = 6.9f;
    mission_item2.is_fly_through = 0;
    mission_item2.gimbal_pitch_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item2.gimbal_yaw_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item2.camera_action = Mission::MissionItem::CameraAction::None;
    mission_item2.loiter_time_s = std::numeric_limits<double>::quiet_NaN();
    mission_item2.camera_photo_interval_s = 1;

    ASSERT_EQ(mission_item1, mission_item2);
}

TEST(MissionOperator, MissionItemEqualsOperatorIsValidForDifferentItems)
{
    Mission::MissionItem mission_item1;
    Mission::MissionItem mission_item2;

    mission_item1.latitude_deg = 47.3982;
    mission_item1.longitude_deg = 8.54567;
    mission_item1.relative_altitude_m = 13.8f;
    mission_item1.speed_m_s = 6.9f;
    mission_item1.is_fly_through = 0;
    mission_item1.gimbal_pitch_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item1.gimbal_yaw_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item1.camera_action = Mission::MissionItem::CameraAction::None;
    mission_item1.loiter_time_s = std::numeric_limits<double>::quiet_NaN();
    mission_item1.camera_photo_interval_s = 1;

    ASSERT_FALSE(mission_item1 == mission_item2);
}

TEST(MissionOperator, MissionItemEqualsOperatorIsValidForEmptyVectors)
{
    std::vector<Mission::MissionItem> mission_items1;
    std::vector<Mission::MissionItem> mission_items2;

    ASSERT_EQ(mission_items1, mission_items2);
}

TEST(MissionOperator, MissionItemEqualsOperatorIsValidForVector)
{
    // First vector
    std::vector<Mission::MissionItem> mission_items1;
    Mission::MissionItem mission_item1;

    mission_item1.latitude_deg = 47.3982;
    mission_item1.longitude_deg = 8.54567;
    mission_item1.relative_altitude_m = 13.8f;
    mission_item1.speed_m_s = 6.9f;
    mission_item1.is_fly_through = 0;
    mission_item1.gimbal_pitch_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item1.gimbal_yaw_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item1.camera_action = Mission::MissionItem::CameraAction::None;
    mission_item1.loiter_time_s = std::numeric_limits<double>::quiet_NaN();
    mission_item1.camera_photo_interval_s = 1;

    mission_items1.push_back(mission_item1);

    // Second vector
    std::vector<Mission::MissionItem> mission_items2;
    Mission::MissionItem mission_item2;

    mission_item2.latitude_deg = 47.3982;
    mission_item2.longitude_deg = 8.54567;
    mission_item2.relative_altitude_m = 13.8f;
    mission_item2.speed_m_s = 6.9f;
    mission_item2.is_fly_through = 0;
    mission_item2.gimbal_pitch_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item2.gimbal_yaw_deg = std::numeric_limits<double>::quiet_NaN();
    mission_item2.camera_action = Mission::MissionItem::CameraAction::None;
    mission_item2.loiter_time_s = std::numeric_limits<double>::quiet_NaN();
    mission_item2.camera_photo_interval_s = 1;

    mission_items2.push_back(mission_item2);

    ASSERT_EQ(mission_items1, mission_items2);
}

TEST(MissionOperator, MissionPlanOperatorIsValidForArbitraryPlans)
{
    // First MissionPlan
    Mission::MissionPlan mission_plan1;

    std::vector<Mission::MissionItem> mission_items1;
    Mission::MissionItem mission_item1;

    mission_item1.latitude_deg = 47.39824189;
    mission_item1.longitude_deg = 8.545536999;
    mission_item1.relative_altitude_m = 10.0f;
    mission_item1.speed_m_s = 2.0f;
    mission_item1.is_fly_through = true;
    mission_item1.gimbal_pitch_deg = 0;
    mission_item1.gimbal_yaw_deg = -60;
    mission_item1.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
    mission_item1.loiter_time_s = 5;
    mission_item1.camera_photo_interval_s = 1;

    mission_items1.push_back(mission_item1);

    mission_plan1.mission_items = mission_items1;

    // Second MissionPlan
    Mission::MissionPlan mission_plan2;

    std::vector<Mission::MissionItem> mission_items2;
    Mission::MissionItem mission_item2;

    mission_item2.latitude_deg = 47.39824189;
    mission_item2.longitude_deg = 8.545536999;
    mission_item2.relative_altitude_m = 10.0f;
    mission_item2.speed_m_s = 2.0f;
    mission_item2.is_fly_through = true;
    mission_item2.gimbal_pitch_deg = 0;
    mission_item2.gimbal_yaw_deg = -60;
    mission_item2.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
    mission_item2.loiter_time_s = 5;
    mission_item2.camera_photo_interval_s = 1;

    mission_items2.push_back(mission_item2);

    mission_plan2.mission_items = mission_items2;

    ASSERT_EQ(mission_plan1, mission_plan2);
}

TEST(MissionOperator, MissionPlanOperatorIsValidForDifferentPlans)
{
    // First MissionPlan
    Mission::MissionPlan mission_plan1;

    std::vector<Mission::MissionItem> mission_items1;
    Mission::MissionItem mission_item1;

    mission_item1.latitude_deg = 47.39824189;
    mission_item1.longitude_deg = 8.545536999;
    mission_item1.relative_altitude_m = 10.0f;
    mission_item1.speed_m_s = 2.0f;
    mission_item1.is_fly_through = true;
    mission_item1.gimbal_pitch_deg = 0;
    mission_item1.gimbal_yaw_deg = -60;
    mission_item1.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
    mission_item1.loiter_time_s = 5;
    mission_item1.camera_photo_interval_s = 1;

    mission_items1.push_back(mission_item1);

    mission_plan1.mission_items = mission_items1;

    // Second MissionPlan
    Mission::MissionPlan mission_plan2;

    ASSERT_FALSE(mission_plan1 == mission_plan2);
}
