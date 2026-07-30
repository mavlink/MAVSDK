#pragma once

#include "plugins/mission_raw/mission_raw.hpp"
#include "mavsdk_export.h"
#include "sender.hpp"
#include "autopilot.hpp"
#include <optional>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>

namespace mavsdk {

class MAVSDK_TEST_EXPORT MissionImport {
public:
    static std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
    parse_json(const std::string& raw_json, Autopilot autopilot);

    static std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
    parse_mission_planner(const std::string& raw_mission_planner, Autopilot autopilot);

private:
    static bool check_overall_version(const nlohmann::json& root);
    static std::optional<std::vector<MissionRaw::MissionItem>>
    import_geofence(const nlohmann::json& root);
    static std::optional<std::vector<MissionRaw::MissionItem>>
    import_rally_points(const nlohmann::json& root);
    static std::optional<std::vector<MissionRaw::MissionItem>>
    import_mission(const nlohmann::json& root, Autopilot autopilot);
    static std::optional<MissionRaw::MissionItem>
    import_simple_mission_item(const nlohmann::json& json_item);
    static std::optional<std::vector<MissionRaw::MissionItem>>
    import_complex_mission_items(const nlohmann::json& json_item);
    static std::vector<MissionRaw::MissionItem>
    import_polygon_geofences(const nlohmann::json& json_item);
    static std::vector<MissionRaw::MissionItem>
    import_circular_geofences(const nlohmann::json& json_item);
    static float set_float(const nlohmann::json& val);
    static int32_t set_int32(const nlohmann::json& val, uint32_t frame);

    // Mission Planner format parsing methods
    static std::optional<std::vector<MissionRaw::MissionItem>>
    parse_mission_planner_items(const std::string& content, Autopilot autopilot);
    static std::optional<MissionRaw::MissionItem>
    parse_mission_planner_line(const std::string& line);
    static bool is_mission_planner_format(const std::string& content);
};

} // namespace mavsdk
