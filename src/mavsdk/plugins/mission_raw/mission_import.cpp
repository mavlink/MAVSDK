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
                    static_cast<int32_t>(std::round(home[0].asDouble() * 1e7)),
                    static_cast<int32_t>(std::round(home[1].asDouble() * 1e7)),
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
        item.x = set_int32(point[0]);
        item.y = set_int32(point[1]);
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
                item.x = set_int32(json_item["params"][i]);
                break;
            case 5:
                item.y = set_int32(json_item["params"][i]);
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
            item.x = set_int32(point[0]);
            item.y = set_int32(point[1]);
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
        item.x = set_int32(center[0]);
        item.y = set_int32(center[1]);
        item.mission_type = MAV_MISSION_TYPE_FENCE;

        circular_geofences.push_back(item);
    }

    return circular_geofences;
}

float MissionImport::set_float(const Json::Value& val)
{
    return val.isNull() ? NAN : val.asFloat();
}

int32_t MissionImport::set_int32(const Json::Value& val)
{
    return static_cast<int32_t>(val.isNull() ? 0 : (std::round(val.asDouble() * 1e7)));
}

} // namespace mavsdk
