#include "log.h"
#include "mission_import.h"
#include "mavlink_include.h"
#include <cmath> // for `std::round`
#include <sstream> // for `std::stringstream`

namespace mavsdk {

std::pair<MissionRaw::Result, MissionRaw::MissionImportData>
MissionImport::parse_json(const std::string& raw_json, Sender::Autopilot autopilot)
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

    MissionRaw::MissionImportData import_data;
    import_data.mission_items = maybe_mission_items.value();
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
MissionImport::import_mission(const Json::Value& root, Sender::Autopilot autopilot)
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

    // Don't forget sequence number
    unsigned sequence = 0;
    for (auto& mission_item : mission_items) {
        mission_item.seq = sequence++;
    }

    // Add home position at 0 for ArduPilot
    if (autopilot == Sender::Autopilot::ArduPilot) {
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

    // Returning an empty vector is ok here if there were really no mission items.
    return {mission_items};
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

    const auto set_float = [](const Json::Value& val) {
        return val.isNull() ? NAN : val.asFloat();
    };

    const auto set_int32 = [](const Json::Value& val) {
        return static_cast<int32_t>(val.isNull() ? 0 : (std::round(val.asDouble() * 1e7)));
    };

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

} // namespace mavsdk
