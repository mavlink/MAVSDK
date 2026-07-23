#include "mavsdk.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Mavsdk, version)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_GT(mavsdk.version().size(), 5);
}

TEST(Mavsdk, Configuration)
{
    Mavsdk::Configuration configuration{ComponentType::Autopilot};

    ASSERT_EQ(configuration.get_mav_type(), MAV_TYPE::MAV_TYPE_GENERIC); // Default
    configuration.set_mav_type(MAV_TYPE::MAV_TYPE_FIXED_WING);
    ASSERT_EQ(configuration.get_mav_type(), MAV_TYPE::MAV_TYPE_FIXED_WING);
}

TEST(Mavsdk, ConfigurationComponentTypeDefaults)
{
    {
        Mavsdk::Configuration cfg{ComponentType::GroundStation};
        EXPECT_EQ(cfg.get_component_type(), ComponentType::GroundStation);
        EXPECT_EQ(cfg.get_system_id(), Mavsdk::DEFAULT_SYSTEM_ID_GCS);
        EXPECT_EQ(cfg.get_component_id(), Mavsdk::DEFAULT_COMPONENT_ID_GCS);
        EXPECT_FALSE(cfg.get_always_send_heartbeats());
        EXPECT_EQ(cfg.get_mav_type(), MAV_TYPE::MAV_TYPE_GCS);
    }
    {
        Mavsdk::Configuration cfg{ComponentType::CompanionComputer};
        EXPECT_EQ(cfg.get_component_type(), ComponentType::CompanionComputer);
        EXPECT_EQ(cfg.get_system_id(), Mavsdk::DEFAULT_SYSTEM_ID_CC);
        EXPECT_EQ(cfg.get_component_id(), Mavsdk::DEFAULT_COMPONENT_ID_CC);
        EXPECT_TRUE(cfg.get_always_send_heartbeats());
        EXPECT_EQ(cfg.get_mav_type(), MAV_TYPE::MAV_TYPE_ONBOARD_CONTROLLER);
    }
    {
        Mavsdk::Configuration cfg{ComponentType::Camera};
        EXPECT_EQ(cfg.get_system_id(), Mavsdk::DEFAULT_SYSTEM_ID_CAMERA);
        EXPECT_EQ(cfg.get_component_id(), Mavsdk::DEFAULT_COMPONENT_ID_CAMERA);
        EXPECT_TRUE(cfg.get_always_send_heartbeats());
        EXPECT_EQ(cfg.get_mav_type(), MAV_TYPE::MAV_TYPE_CAMERA);
    }
    {
        Mavsdk::Configuration cfg{ComponentType::Gimbal};
        EXPECT_EQ(cfg.get_component_id(), Mavsdk::DEFAULT_COMPONENT_ID_GIMBAL);
        EXPECT_EQ(cfg.get_mav_type(), MAV_TYPE::MAV_TYPE_GIMBAL);
    }
    {
        Mavsdk::Configuration cfg{ComponentType::RemoteId};
        EXPECT_EQ(cfg.get_component_id(), Mavsdk::DEFAULT_COMPONENT_ID_REMOTEID);
        EXPECT_EQ(cfg.get_mav_type(), MAV_TYPE::MAV_TYPE_ODID);
    }
    {
        Mavsdk::Configuration cfg{ComponentType::Custom};
        // Custom leaves ctor default ids as GCS defaults then default switch leaves them.
        EXPECT_EQ(cfg.get_component_type(), ComponentType::Custom);
    }
}

TEST(Mavsdk, ConfigurationFromIdsMapsComponentType)
{
    Mavsdk::Configuration cfg(
        /*system_id=*/42,
        /*component_id=*/static_cast<uint8_t>(Mavsdk::DEFAULT_COMPONENT_ID_CAMERA),
        /*always_send_heartbeats=*/true);
    EXPECT_EQ(cfg.get_system_id(), 42);
    EXPECT_EQ(cfg.get_component_id(), Mavsdk::DEFAULT_COMPONENT_ID_CAMERA);
    EXPECT_TRUE(cfg.get_always_send_heartbeats());
    EXPECT_EQ(cfg.get_component_type(), ComponentType::Camera);

    Mavsdk::Configuration custom_ids(9, 200, false);
    EXPECT_EQ(custom_ids.get_component_type(), ComponentType::Custom);
}

TEST(Mavsdk, ConfigurationSetters)
{
    Mavsdk::Configuration cfg{ComponentType::GroundStation};
    cfg.set_system_id(11);
    cfg.set_component_id(22);
    cfg.set_always_send_heartbeats(true);
    cfg.set_component_type(ComponentType::Custom);
    cfg.set_autopilot(Autopilot::ArduPilot);
    cfg.set_compatibility_mode(CompatibilityMode::Px4);
    EXPECT_EQ(cfg.get_system_id(), 11);
    EXPECT_EQ(cfg.get_component_id(), 22);
    EXPECT_TRUE(cfg.get_always_send_heartbeats());
    EXPECT_EQ(cfg.get_component_type(), ComponentType::Custom);
    EXPECT_EQ(cfg.get_autopilot(), Autopilot::ArduPilot);
    EXPECT_EQ(cfg.get_compatibility_mode(), CompatibilityMode::Px4);
}
