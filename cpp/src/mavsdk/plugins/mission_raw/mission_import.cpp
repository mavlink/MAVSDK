#include "log.h"
#include "mission_import.h"
#include "mavlink_include.h"
#include <cmath> // for `std::round`
#include <sstream> // for `std::stringstream`

namespace mavsdk {

std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
MissionImport::parse_json(const std::string& raw_json, Autopilot autopilot)
{
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    JSONCPP_STRING err;

    if (!reader->parse(raw_json.c_str(), raw_json.c_str() + raw_json.length(), &root, &err)) {
        LogErr() << "Parse error: " << err;
        return {MissionRaw::Result::FailedToParseQgcPlan, {}};
    }

    if (!check_overall_version(root)) {
        return {MissionRaw::Result::FailedToParseQgcPlan, {}};
    }

    auto maybe_mission_items = import_mission(root, autopilot);
    if (!maybe_mission_items.has_value()) {
        return {MissionRaw::Result::FailedToParseQgcPlan, {}};
    }

    auto maybe_geofence_items = import_geofence(root);
    if (!maybe_geofence_items.has_value()) {
        return {MissionRaw::Result::FailedToParseQgcPlan, {}};
    }

    auto maybe_rally_items = import_rally_points(root);
    if (!maybe_rally_items.has_value()) {
        return {MissionRaw::Result::FailedToParseQgcPlan, {}};
    }

    MissionRaw::MissionImportData import_data;
    import_data.mission_items = maybe_mission_items.value();
    import_data.geofence_items = maybe_geofence_items.value();
    import_data.rally_items = maybe_rally_items.value();

    return {MissionRaw::Result::Success, import_data};
}

bool MissionImport::check_overall_version(const Json::Value& root)
{
    const auto supported_overall_version = 1;
    const auto overall_version = root["version"];
    if (overall_version.empty() || overall_version.asInt() != supported_overall_version) {
        LogErr() << "Overall .plan version not supported, found version: " << overall_version
                 << ", supported: " << supported_overall_version;
        return false;
    }

    return true;
}

std::optional<std::vector<MissionRaw::MissionItem>>
MissionImport::import_mission(const Json::Value& root, Autopilot autopilot)
{
    // We need a mission part.
    const auto mission = root["mission"];
    if (mission.empty()) {
        LogErr() << "No mission found in .plan.";
        return std::nullopt;
    }

    // Check the mission version.
    const auto supported_mission_version = 2;
    const auto mission_version = mission["version"];
    if (mission_version.empty() || mission_version.asInt() != supported_mission_version) {
        LogErr() << "mission version for .plan not supported, found version: "
                 << mission_version.asInt() << ", supported: " << supported_mission_version;
        return std::nullopt;
    }

    std::vector<MissionRaw::MissionItem> mission_items;

    // Go through items
    for (const auto& json_item : mission["items"]) {
        const auto type = json_item["type"];

        if (!type.isNull() && type.asString() == "SimpleItem") {
            const auto maybe_item = import_simple_mission_item(json_item);
            if (maybe_item.has_value()) {
                mission_items.push_back(maybe_item.value());
            } else {
                return std::nullopt;
            }

        } else if (!type.isNull() && type.asString() == "ComplexItem") {
            const auto maybe_items = import_complex_mission_items(json_item);
            if (maybe_items.has_value()) {
                mission_items.insert(
                    mission_items.end(), maybe_items.value().begin(), maybe_items.value().end());
            } else {
                return std::nullopt;
            }

        } else {
            LogErr() << "Type " << type.asString() << " not understood.";
            return std::nullopt;
        }
    }

    // Mark first item as current
    if (mission_items.size() > 0) {
        mission_items[0].current = 1;
    }

    // Add home position at 0 for ArduPilot
    if (autopilot == Autopilot::ArduPilot) {
        const auto home = mission["plannedHomePosition"];
        if (!home.empty()) {
            if (home.isArray() && home.size() != 3) {
                LogErr() << "Unknown plannedHomePosition format";
                return std::nullopt;
            }

            mission_items.insert(
                mission_items.begin(),
                MissionRaw::MissionItem{
                    0,
                    MAV_FRAME_GLOBAL_INT,
                    MAV_CMD_NAV_WAYPOINT,
                    0, // current
                    1, // autocontinue
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f,
                    set_int32(home[0], MAV_FRAME_GLOBAL_INT),
                    set_int32(home[1], MAV_FRAME_GLOBAL_INT),
                    home[2].asFloat(),
                    MAV_MISSION_TYPE_MISSION});
        }
    }

    // Don't forget sequence number
    unsigned sequence = 0;
    for (auto& mission_item : mission_items) {
        mission_item.seq = sequence++;
    }

    // Returning an empty vector is ok here if there were really no mission items.
    return {mission_items};
}

std::optional<std::vector<MissionRaw::MissionItem>>
MissionImport::import_geofence(const Json::Value& root)
{
    std::vector<MissionRaw::MissionItem> geofence_items;

    // Return early if there are no geofence items.
    const auto geofence = root["geoFence"];
    if (geofence.empty()) {
        return std::nullopt;
    }

    // Check the mission version.
    const auto supported_geofence_version = 2;
    const auto geofence_version = geofence["version"];
    if (geofence_version.empty() || geofence_version.asInt() != supported_geofence_version) {
        LogErr() << "geofence version for .plan not supported, found version: "
                 << geofence_version.asInt() << ", supported: " << supported_geofence_version;
        return std::nullopt;
    }

    // Import polygon geofences
    std::vector<MissionRaw::MissionItem> polygon_geofences;
    polygon_geofences = import_polygon_geofences(geofence["polygons"]);

    // Import circular geofences
    std::vector<MissionRaw::MissionItem> circular_geofences;
    circular_geofences = import_circular_geofences(geofence["circles"]);

    geofence_items.insert(geofence_items.end(), polygon_geofences.begin(), polygon_geofences.end());
    geofence_items.insert(
        geofence_items.end(), circular_geofences.begin(), circular_geofences.end());

    // Mark first item as current
    if (geofence_items.size() > 0) {
        geofence_items[0].current = 1;
    }

    // Don't forget sequence number
    unsigned sequence = 0;
    for (auto& item : geofence_items) {
        item.seq = sequence++;
    }

    return {geofence_items};
}

std::optional<std::vector<MissionRaw::MissionItem>>
MissionImport::import_rally_points(const Json::Value& root)
{
    std::vector<MissionRaw::MissionItem> rally_items;

    // Return early if there are no rally points.
    const auto rally_points = root["rallyPoints"];
    if (rally_points.empty()) {
        return std::nullopt;
    }

    // Check the rally points version.
    const auto supported_rally_points_version = 2;
    const auto rally_points_version = rally_points["version"];
    if (rally_points_version.empty() ||
        rally_points_version.asInt() != supported_rally_points_version) {
        LogErr() << "rally points version for .plan not supported, found version: "
                 << rally_points_version.asInt()
                 << ", supported: " << supported_rally_points_version;
        return std::nullopt;
    }

    // Go through items
    for (const auto& point : rally_points["points"]) {
        MissionRaw::MissionItem item{};

        item.command = MAV_CMD_NAV_RALLY_POINT;
        item.frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
        item.mission_type = MAV_MISSION_TYPE_RALLY;
        item.x = set_int32(point[0], item.frame);
        item.y = set_int32(point[1], item.frame);
        item.z = set_float(point[2]);

        rally_items.push_back(item);
    }

    // Mark first item as current
    if (rally_items.size() > 0) {
        rally_items[0].current = 1;
    }

    // Don't forget sequence number
    unsigned sequence = 0;
    for (auto& item : rally_items) {
        item.seq = sequence++;
    }

    return {rally_items};
}

std::optional<MissionRaw::MissionItem>
MissionImport::import_simple_mission_item(const Json::Value& json_item)
{
    if (json_item["command"].empty() || json_item["autoContinue"].empty() ||
        json_item["frame"].empty() || json_item["params"].empty()) {
        LogErr() << "Missing mission item field.";
        return std::nullopt;
    }

    if (!json_item["params"].isArray()) {
        LogErr() << "No param array found.";
        return std::nullopt;
    }

    MissionRaw::MissionItem item{};
    item.command = json_item["command"].asInt();
    item.autocontinue = json_item["autoContinue"].asBool() ? 1 : 0;
    item.frame = json_item["frame"].asInt();
    item.mission_type = MAV_MISSION_TYPE_MISSION;

    for (unsigned i = 0; i < 7; ++i) {
        switch (i) {
            case 0:
                item.param1 = set_float(json_item["params"][i]);
                break;
            case 1:
                item.param2 = set_float(json_item["params"][i]);
                break;
            case 2:
                item.param3 = set_float(json_item["params"][i]);
                break;
            case 3:
                item.param4 = set_float(json_item["params"][i]);
                break;
            case 4:
                item.x = set_int32(json_item["params"][i], item.frame);
                break;
            case 5:
                item.y = set_int32(json_item["params"][i], item.frame);
                break;
            case 6:
                item.z = set_float(json_item["params"][i]);
                break;
            default:
                // should never happen
                break;
        }
    }

    return {item};
}

std::optional<std::vector<MissionRaw::MissionItem>>
MissionImport::import_complex_mission_items(const Json::Value& json_item)
{
    if (json_item["complexItemType"].empty()) {
        LogErr() << "Could not determine complexItemType";
        return std::nullopt;
    }

    if (json_item["complexItemType"] != "survey") {
        LogErr() << "complexItemType: " << json_item["complexItemType"] << " not supported";
        return std::nullopt;
    }

    if (json_item["version"].empty()) {
        LogErr() << "version of complexItem not found";
        return std::nullopt;
    }

    const int supported_complex_item_version = 5;
    const int found_version = json_item["version"].asInt();
    if (found_version != 5) {
        LogErr() << "version of complexItem not supported, found version: " << found_version
                 << ", supported: " << supported_complex_item_version;
        return std::nullopt;
    }

    if (json_item["TransectStyleComplexItem"].empty()) {
        LogErr() << "TransectStyleComplexItem not found";
        return std::nullopt;
    }

    // These are Items (capitalized!) inside the  TransectStyleComplexItem.
    if (json_item["TransectStyleComplexItem"]["Items"].empty() ||
        !json_item["TransectStyleComplexItem"]["Items"].isArray()) {
        LogErr() << "No survey items found";
        return std::nullopt;
    }

    std::vector<MissionRaw::MissionItem> mission_items;
    for (const auto& json_item_subitem : json_item["TransectStyleComplexItem"]["Items"]) {
        const auto maybe_item = import_simple_mission_item(json_item_subitem);
        if (maybe_item.has_value()) {
            mission_items.push_back(maybe_item.value());
        }
    }

    return {mission_items};
}

std::vector<MissionRaw::MissionItem>
MissionImport::import_polygon_geofences(const Json::Value& polygons)
{
    std::vector<MissionRaw::MissionItem> polygon_geofences;

    for (const auto& polygon : polygons) {
        bool inclusion_missing = polygon["inclusion"].isNull();
        bool inclusion = inclusion_missing ? true : polygon["inclusion"].asBool();
        const auto& points = polygon["polygon"];
        for (const auto& point : points) {
            MissionRaw::MissionItem item{};

            item.command = inclusion ? MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION :
                                       MAV_CMD_NAV_FENCE_POLYGON_VERTEX_EXCLUSION;
            item.frame = MAV_FRAME_GLOBAL;
            item.param1 = set_float(points.size());
            item.x = set_int32(point[0], item.frame);
            item.y = set_int32(point[1], item.frame);
            item.mission_type = MAV_MISSION_TYPE_FENCE;

            polygon_geofences.push_back(item);
        }
    }

    return polygon_geofences;
}

std::vector<MissionRaw::MissionItem>
MissionImport::import_circular_geofences(const Json::Value& circles)
{
    std::vector<MissionRaw::MissionItem> circular_geofences;

    for (const auto& circle : circles) {
        bool inclusion = circle["inclusion"].asBool();
        const auto& center = circle["circle"]["center"];
        const auto& radius = circle["circle"]["radius"];

        MissionRaw::MissionItem item{};

        item.command =
            inclusion ? MAV_CMD_NAV_FENCE_CIRCLE_INCLUSION : MAV_CMD_NAV_FENCE_CIRCLE_EXCLUSION;
        item.frame = MAV_FRAME_GLOBAL;
        item.param1 = set_float(radius);
        item.x = set_int32(center[0], item.frame);
        item.y = set_int32(center[1], item.frame);
        item.mission_type = MAV_MISSION_TYPE_FENCE;

        circular_geofences.push_back(item);
    }

    return circular_geofences;
}

float MissionImport::set_float(const Json::Value& val)
{
    return val.isNull() ? NAN : val.asFloat();
}

int32_t MissionImport::set_int32(const Json::Value& val, uint32_t frame)
{
    // Don't apply 10^7 conversion for MAV_FRAME_MISSION
    if (frame == MAV_FRAME_MISSION) {
        return static_cast<int32_t>(val.isNull() ? 0 : val.asFloat());
    } else {
        return static_cast<int32_t>(val.isNull() ? 0 : (std::round(val.asDouble() * 1e7)));
    }
}

std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
MissionImport::parse_mission_planner(const std::string& raw_mission_planner, Autopilot autopilot)
{
    if (!is_mission_planner_format(raw_mission_planner)) {
        LogErr() << "Invalid Mission Planner format";
        return {MissionRaw::Result::FailedToParseMissionPlannerPlan, {}};
    }

    auto maybe_mission_items = parse_mission_planner_items(raw_mission_planner, autopilot);
    if (!maybe_mission_items.has_value()) {
        return {MissionRaw::Result::FailedToParseMissionPlannerPlan, {}};
    }

    MissionRaw::MissionImportData import_data;
    import_data.mission_items = maybe_mission_items.value();
    // Mission Planner format doesn't include geofence or rally points
    import_data.geofence_items = {};
    import_data.rally_items = {};

    return {MissionRaw::Result::Success, import_data};
}

bool MissionImport::is_mission_planner_format(const std::string& content)
{
    std::istringstream stream(content);
    std::string first_line;
    if (std::getline(stream, first_line)) {
        // Remove trailing whitespace
        first_line.erase(first_line.find_last_not_of(" \t\r\n") + 1);
        return first_line == "QGC WPL 110";
    }
    return false;
}

std::optional<std::vector<MissionRaw::MissionItem>>
MissionImport::parse_mission_planner_items(const std::string& content, Autopilot autopilot)
{
    std::vector<MissionRaw::MissionItem> mission_items;
    std::istringstream stream(content);
    std::string line;

    // Skip first line (header)
    if (!std::getline(stream, line)) {
        LogErr() << "Empty Mission Planner file";
        return std::nullopt;
    }

    unsigned sequence = 0;
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto maybe_item = parse_mission_planner_line(line);
        if (!maybe_item.has_value()) {
            LogErr() << "Failed to parse Mission Planner line: " << line;
            return std::nullopt;
        }

        auto item = maybe_item.value();
        item.seq = sequence++;

        // Mark first item as current
        if (sequence == 1) {
            item.current = 1;
        }

        mission_items.push_back(item);
    }

    // Add home position at 0 for ArduPilot if there are mission items
    if (autopilot == Autopilot::ArduPilot && !mission_items.empty()) {
        // Use first waypoint position as home if it's a waypoint
        const auto& first_item = mission_items[0];
        if (first_item.command == MAV_CMD_NAV_WAYPOINT ||
            first_item.command == MAV_CMD_NAV_TAKEOFF) {
            MissionRaw::MissionItem home_item{
                0, // seq - will be updated later
                MAV_FRAME_GLOBAL_INT,
                MAV_CMD_NAV_WAYPOINT,
                0, // current - will be updated later
                1, // autocontinue
                0.0f, // param1
                0.0f, // param2
                0.0f, // param3
                0.0f, // param4
                first_item.x, // use same x as first waypoint
                first_item.y, // use same y as first waypoint
                first_item.z, // use same z as first waypoint
                MAV_MISSION_TYPE_MISSION};

            mission_items.insert(mission_items.begin(), home_item);
        }
    }

    // Update sequence numbers after potential home insertion
    for (size_t i = 0; i < mission_items.size(); ++i) {
        mission_items[i].seq = static_cast<unsigned>(i);
        // Mark first item as current
        mission_items[i].current = (i == 0) ? 1 : 0;
    }

    return {mission_items};
}

std::optional<MissionRaw::MissionItem>
MissionImport::parse_mission_planner_line(const std::string& line)
{
    // Mission Planner format: seq current frame command param1 param2 param3 param4 x y z
    // autocontinue Tab-separated values, 12 fields total
    std::istringstream stream(line);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(stream, field, '\t')) {
        fields.push_back(field);
    }

    if (fields.size() != 12) {
        LogErr() << "Invalid Mission Planner line format, expected 12 fields, got "
                 << fields.size();
        return std::nullopt;
    }

    MissionRaw::MissionItem item{};

    // Parse numeric fields with basic validation
    char* endptr = nullptr;

    // Parse seq
    unsigned long seq_val = std::strtoul(fields[0].c_str(), &endptr, 10);
    if (endptr == fields[0].c_str() || *endptr != '\0') {
        LogErr() << "Invalid seq field: " << fields[0];
        return std::nullopt;
    }
    item.seq = static_cast<uint32_t>(seq_val);

    // Parse current
    unsigned long current_val = std::strtoul(fields[1].c_str(), &endptr, 10);
    if (endptr == fields[1].c_str() || *endptr != '\0') {
        LogErr() << "Invalid current field: " << fields[1];
        return std::nullopt;
    }
    item.current = static_cast<uint32_t>(current_val);

    // Parse frame
    unsigned long frame_val = std::strtoul(fields[2].c_str(), &endptr, 10);
    if (endptr == fields[2].c_str() || *endptr != '\0') {
        LogErr() << "Invalid frame field: " << fields[2];
        return std::nullopt;
    }
    item.frame = static_cast<uint32_t>(frame_val);

    // Parse command
    unsigned long command_val = std::strtoul(fields[3].c_str(), &endptr, 10);
    if (endptr == fields[3].c_str() || *endptr != '\0') {
        LogErr() << "Invalid command field: " << fields[3];
        return std::nullopt;
    }
    item.command = static_cast<uint32_t>(command_val);

    // Parse float parameters (param1-4)
    item.param1 = static_cast<float>(std::strtod(fields[4].c_str(), &endptr));
    if (endptr == fields[4].c_str()) {
        LogErr() << "Invalid param1 field: " << fields[4];
        return std::nullopt;
    }

    item.param2 = static_cast<float>(std::strtod(fields[5].c_str(), &endptr));
    if (endptr == fields[5].c_str()) {
        LogErr() << "Invalid param2 field: " << fields[5];
        return std::nullopt;
    }

    item.param3 = static_cast<float>(std::strtod(fields[6].c_str(), &endptr));
    if (endptr == fields[6].c_str()) {
        LogErr() << "Invalid param3 field: " << fields[6];
        return std::nullopt;
    }

    item.param4 = static_cast<float>(std::strtod(fields[7].c_str(), &endptr));
    if (endptr == fields[7].c_str()) {
        LogErr() << "Invalid param4 field: " << fields[7];
        return std::nullopt;
    }

    // Parse x (latitude)
    double x_val = std::strtod(fields[8].c_str(), &endptr);
    if (endptr == fields[8].c_str()) {
        LogErr() << "Invalid x field: " << fields[8];
        return std::nullopt;
    }

    // Parse y (longitude)
    double y_val = std::strtod(fields[9].c_str(), &endptr);
    if (endptr == fields[9].c_str()) {
        LogErr() << "Invalid y field: " << fields[9];
        return std::nullopt;
    }

    // For lat/lon coordinates, convert from degrees to degrees * 10^7
    if (item.frame == MAV_FRAME_GLOBAL || item.frame == MAV_FRAME_GLOBAL_INT ||
        item.frame == MAV_FRAME_GLOBAL_RELATIVE_ALT ||
        item.frame == MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
        item.x = static_cast<int32_t>(std::round(x_val * 1e7));
        item.y = static_cast<int32_t>(std::round(y_val * 1e7));
    } else {
        item.x = static_cast<int32_t>(std::round(x_val));
        item.y = static_cast<int32_t>(std::round(y_val));
    }

    // Parse z (altitude)
    item.z = static_cast<float>(std::strtod(fields[10].c_str(), &endptr));
    if (endptr == fields[10].c_str()) {
        LogErr() << "Invalid z field: " << fields[10];
        return std::nullopt;
    }

    // Parse autocontinue
    unsigned long autocontinue_val = std::strtoul(fields[11].c_str(), &endptr, 10);
    if (endptr == fields[11].c_str() || *endptr != '\0') {
        LogErr() << "Invalid autocontinue field: " << fields[11];
        return std::nullopt;
    }
    item.autocontinue = static_cast<uint32_t>(autocontinue_val);

    item.mission_type = MAV_MISSION_TYPE_MISSION;

    return {item};
}

} // namespace mavsdk
