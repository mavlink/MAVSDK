#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <future>
#include <thread>

#include "mavsdk.h"
#include "log.h"
#include "plugins/component_metadata/component_metadata.h"
#include "plugins/component_metadata_server/component_metadata_server.h"
#include <gtest/gtest.h>

using namespace mavsdk;

static constexpr const char* parameter_json_metadata = R"JSON({
  "parameters": [
    {
      "category": "Standard",
      "default": 1,
      "group": "Autotune",
      "longDesc": "After the auto-tuning sequence is completed, a new set of gains is available and can be applied immediately or after landing. WARNING Applying the gains in air is dangerous as there is no guarantee that those new gains will be able to stabilize the drone properly.",
      "name": "MC_AT_APPLY",
      "shortDesc": "Controls when to apply the new gains",
      "type": "Int32",
      "values": [
        {
          "description": "Do not apply the new gains (logging only)",
          "value": 0
        },
        {
          "description": "Apply the new gains after disarm",
          "value": 1
        },
        {
          "description": "WARNING Apply the new gains in air",
          "value": 2
        }
      ]
    },
    {
      "category": "Standard",
      "default": 0,
      "group": "Autotune",
      "name": "MC_AT_EN",
      "shortDesc": "Multicopter autotune module enable",
      "type": "Int32",
      "values": [
        {
          "description": "Disabled",
          "value": 0
        },
        {
          "description": "Enabled",
          "value": 1
        }
      ]
    },
    {
      "category": "Standard",
      "decimalPlaces": 2,
      "default": 0.14,
      "group": "Autotune",
      "max": 0.5,
      "min": 0.01,
      "name": "MC_AT_RISE_TIME",
      "shortDesc": "Desired angular rate closed-loop rise time",
      "type": "Float",
      "units": "s"
    },
    {
      "category": "Standard",
      "default": 0,
      "group": "Autotune",
      "longDesc": "WARNING: this will inject steps to the rate controller and can be dangerous. Only activate if you know what you are doing, and in a safe environment. Any motion of the remote stick will abort the signal injection and reset this parameter Best is to perform the identification in position or hold mode. Increase the amplitude of the injected signal using MC_AT_SYSID_AMP for more signal/noise ratio",
      "name": "MC_AT_START",
      "shortDesc": "Start the autotuning sequence",
      "type": "Int32",
      "values": [
        {
          "description": "Disabled",
          "value": 0
        },
        {
          "description": "Enabled",
          "value": 1
        }
      ]
    },
    {
      "category": "Standard",
      "decimalPlaces": 1,
      "default": 0.7,
      "group": "Autotune",
      "max": 6.0,
      "min": 0.1,
      "name": "MC_AT_SYSID_AMP",
      "shortDesc": "Amplitude of the injected signal",
      "type": "Float"
    },
    {
      "category": "Standard",
      "decimalPlaces": 0,
      "default": -1.0,
      "group": "Battery Calibration",
      "increment": 50.0,
      "longDesc": "Defines the capacity of battery 1 in mAh.",
      "max": 100000.0,
      "min": -1.0,
      "name": "BAT1_CAPACITY",
      "rebootRequired": true,
      "shortDesc": "Battery 1 capacity",
      "type": "Float",
      "units": "mAh"
    }
  },
  "version": 1
}
)JSON";

static constexpr const char* events_json_metadata = R"JSON({
  "components": {
    "1": {
      "event_groups": {
        "default": {
          "events": {
            "10072599": {
              "message": "Calibration: Disabling RC input",
              "name": "commander_calib_rc_off"
            },
            "10162376": {
              "message": "Mission land item could not be read",
              "name": "rtl_failed_to_read_land_item"
            },
            "10229254": {
              "arguments": [
                {
                  "name": "arg0",
                  "type": "int32_t"
                }
              ],
              "message": "RTL Mission Land: climb to {1m_v}",
              "name": "rtl_mission_land_climb"
            },
            "10452355": {
              "message": "Takeoff airspeed reached, climbout",
              "name": "runway_takeoff_reached_airspeed"
            },
            "10493596": {
              "message": "Airspeed sensor failure detected. Return to launch (RTL) is advised",
              "name": "airspeed_selector_sensor_failure"
            }
        }
      }
    }
  },
  "version": 2
}
)JSON";

TEST(SystemTest, ComponentInformationConnect)
{
    // Enable more debug output
#ifndef WINDOWS // setenv is not available on Windows
    setenv("MAVSDK_COMPONENT_METADATA_DEBUGGING", "1", 1);
#endif

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);

    Mavsdk mavsdk_companion{Mavsdk::Configuration{Mavsdk::ComponentType::CompanionComputer}};
    ASSERT_EQ(
        mavsdk_companion.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    // Wait for companion system to connect
    auto prom = std::promise<std::shared_ptr<System>>();
    std::once_flag flag;
    mavsdk_groundstation.subscribe_on_new_system([&]() {
        std::call_once(flag, [&prom, &mavsdk_groundstation]() {
            prom.set_value(mavsdk_groundstation.systems().at(0));
        });
    });
    auto fut = prom.get_future();
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    auto system = fut.get();

    auto server = ComponentMetadataServer{mavsdk_companion.server_component()};

    // Register metadata
    std::vector<ComponentMetadataServer::Metadata> metadata;
    metadata.push_back(
        {ComponentMetadataServer::MetadataType::Parameter, std::string(parameter_json_metadata)});
    metadata.push_back(
        {ComponentMetadataServer::MetadataType::Events, std::string(events_json_metadata)});
    server.set_metadata(metadata);

    // Ask for metadata
    auto client = ComponentMetadata{system};
    client.request_component(MAV_COMP_ID_ONBOARD_COMPUTER);
    std::atomic_bool received_parameters{false};
    std::atomic_bool received_events{false};
    std::atomic_bool all_completed{false};
    client.subscribe_metadata_available([&received_events, &received_parameters, &all_completed](
                                            ComponentMetadata::MetadataUpdate data) {
        LogInfo() << "Got metadata, type: " << static_cast<int>(data.type);
        EXPECT_EQ(data.compid, MAV_COMP_ID_ONBOARD_COMPUTER);
        switch (data.type) {
            case ComponentMetadata::MetadataType::Parameter:
                EXPECT_EQ(data.json_metadata, parameter_json_metadata);
                received_parameters = true;
                break;
            case ComponentMetadata::MetadataType::Events:
                EXPECT_EQ(data.json_metadata, events_json_metadata);
                received_events = true;
                break;
            case ComponentMetadata::MetadataType::AllCompleted:
                all_completed = true;
                break;
            default:
                ASSERT_TRUE(false) << "Unexpected metadata type " << static_cast<int>(data.type);
        }
    });

    for (int i = 0; i < 100 && (!received_events || !received_parameters || !all_completed); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_TRUE(received_events) << "timeout, metadata not received";
    ASSERT_TRUE(received_parameters) << "timeout, metadata not received";
    ASSERT_TRUE(all_completed) << "timeout, metadata not received";
}