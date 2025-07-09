#include "log.h"
#include "camera_definition.h"
#include <fstream>
#include <sstream>

// Custom error handler for rapidxml no-exceptions mode
namespace rapidxml {
void parse_error_handler(const char* what, void* where)
{
    (void)where; // Suppress unused parameter warning
    mavsdk::LogErr() << "RapidXML parse error: " << what;
    // In no-exceptions mode, we can't throw, so we just log the error
    // The calling code will check for parsing success
}
} // namespace rapidxml

namespace mavsdk {

bool CameraDefinition::load_file(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LogErr() << "Failed to open file: " << filepath;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    _xml_content = buffer.str();
    file.close();

    _doc.parse<0>(&_xml_content[0]);
    if (_doc.first_node() == nullptr) {
        LogErr() << "RapidXML parse failed for file: " << filepath;
        return false;
    }

    return parse_xml();
}

bool CameraDefinition::load_string(const std::string& content)
{
    _xml_content = content;

    _doc.parse<0>(&_xml_content[0]);
    if (_doc.first_node() == nullptr) {
        LogErr() << "RapidXML parse failed for string content";
        return false;
    }

    return parse_xml();
}

std::string CameraDefinition::get_model() const
{
    return _model;
}

std::string CameraDefinition::get_vendor() const
{
    return _vendor;
}

bool CameraDefinition::parse_xml()
{
    auto e_mavlinkcamera = _doc.first_node("mavlinkcamera");
    if (!e_mavlinkcamera) {
        LogErr() << "Tag mavlinkcamera not found";
        return false;
    }

    auto e_definition = e_mavlinkcamera->first_node("definition");
    if (!e_definition) {
        LogErr() << "definition not found";
        return false;
    }

    auto e_model = e_definition->first_node("model");
    if (!e_model) {
        LogErr() << "model not found";
        return false;
    }

    const char* model_text = e_model->value();
    _model = model_text ? model_text : "";

    auto e_vendor = e_definition->first_node("vendor");
    if (!e_vendor) {
        LogErr() << "vendor not found";
        return false;
    }

    const char* vendor_text = e_vendor->value();
    _vendor = vendor_text ? vendor_text : "";

    auto e_parameters = e_mavlinkcamera->first_node("parameters");
    if (!e_parameters) {
        LogErr() << "Tag parameters not found";
        return false;
    }

    std::unordered_map<std::string, std::string> type_map{};
    // We need all types first.
    for (auto e_parameter = e_parameters->first_node("parameter"); e_parameter != nullptr;
         e_parameter = e_parameter->next_sibling("parameter")) {
        auto name_attr = e_parameter->first_attribute("name");
        if (!name_attr) {
            LogErr() << "name attribute missing";
            return false;
        }
        const char* param_name = name_attr->value();

        auto type_attr = e_parameter->first_attribute("type");
        if (!type_attr) {
            LogErr() << "type attribute missing";
            return false;
        }
        const char* type_str = type_attr->value();

        type_map[param_name] = type_str;
    }

    for (auto e_parameter = e_parameters->first_node("parameter"); e_parameter != nullptr;
         e_parameter = e_parameter->next_sibling("parameter")) {
        auto new_parameter = std::make_shared<Parameter>();

        auto name_attr = e_parameter->first_attribute("name");
        if (!name_attr) {
            LogErr() << "name attribute missing";
            return false;
        }
        const char* param_name = name_attr->value();

        auto type_attr = e_parameter->first_attribute("type");
        if (!type_attr) {
            LogErr() << "type attribute missing for " << param_name;
            return false;
        }
        const char* type_str_res = type_attr->value();

        auto type_str = std::string(type_str_res);
        if (type_str == "string") {
            LogDebug() << "Ignoring string params: " << param_name;
            continue;
        }

        if (type_str == "custom") {
            LogDebug() << "Ignoring custom params: " << param_name;
            continue;
        }

        if (!new_parameter->type.set_empty_type_from_xml(type_str)) {
            LogErr() << "Unknown type attribute: " << type_str;
            return false;
        }

        // By default control is on.
        new_parameter->is_control = true;
        auto control_attr = e_parameter->first_attribute("control");
        if (control_attr) {
            const char* control_str = control_attr->value();
            if (control_str && std::string(control_str) == "0") {
                new_parameter->is_control = false;
            }
        }

        new_parameter->is_readonly = false;
        auto readonly_attr = e_parameter->first_attribute("readonly");
        if (readonly_attr) {
            const char* readonly_str = readonly_attr->value();
            if (readonly_str && std::string(readonly_str) == "1") {
                new_parameter->is_readonly = true;
            }
        }

        new_parameter->is_writeonly = false;
        auto writeonly_attr = e_parameter->first_attribute("writeonly");
        if (writeonly_attr) {
            const char* writeonly_str = writeonly_attr->value();
            if (writeonly_str && std::string(writeonly_str) == "1") {
                new_parameter->is_writeonly = true;
            }
        }

        if (new_parameter->is_readonly && new_parameter->is_writeonly) {
            LogErr() << "parameter can't be readonly and writeonly";
            return false;
        }

        // Be definition custom types do not have control.
        if (std::string(type_map[param_name]) == "custom") {
            new_parameter->is_control = false;
        }

        auto e_description = e_parameter->first_node("description");
        if (!e_description) {
            LogErr() << "Description missing";
            return false;
        }

        const char* description_text = e_description->value();
        new_parameter->description = description_text ? description_text : "";

        // LogDebug() << "Found: " << new_parameter->description
        //            << " (" << param_name
        //            << ", control: " << (new_parameter->is_control ? "yes" : "no")
        //            << ", readonly: " << (new_parameter->is_readonly ? "yes" : "no")
        //            << ", writeonly: " << (new_parameter->is_writeonly ? "yes" : "no")
        //            << ")";

        auto e_updates = e_parameter->first_node("updates");
        if (e_updates) {
            for (auto e_update = e_updates->first_node("update"); e_update != nullptr;
                 e_update = e_update->next_sibling("update")) {
                const char* update_text = e_update->value();
                if (update_text) {
                    new_parameter->updates.emplace_back(update_text);
                }
            }
        }

        auto default_attr = e_parameter->first_attribute("default");
        if (!default_attr) {
            LogWarn() << "Default missing for " << param_name;
            continue;
        }
        const char* default_str = default_attr->value();

        auto get_default_opt = [&]() {
            auto maybe_default = find_default(new_parameter->options, default_str);

            if (!maybe_default.first) {
                LogWarn() << "Default not found for " << param_name;
                return std::optional<Option>{};
            }

            return std::optional{maybe_default.second};
        };

        auto e_options = e_parameter->first_node("options");
        if (e_options) {
            auto maybe_options = parse_options(e_options, param_name, type_map);
            if (!maybe_options.first) {
                continue;
            }
            new_parameter->options = maybe_options.second;

            if (auto default_option = get_default_opt()) {
                new_parameter->default_option = *default_option;
            } else {
                return false;
            }
        } else if (type_str == "bool") {
            // Automaticaly create bool options if the parameter type is bool as per documentation.
            Option true_option;
            true_option.name = "on";
            true_option.value.set<uint8_t>(true);
            Option false_option;
            true_option.name = "off";
            false_option.value.set<uint8_t>(false);

            new_parameter->options = {
                std::make_shared<Option>(std::move(true_option)),
                std::make_shared<Option>(std::move(false_option))};

            if (auto default_option = get_default_opt()) {
                new_parameter->default_option = *default_option;
            } else {
                return false;
            }
        } else {
            auto maybe_range_options = parse_range_options(e_parameter, param_name, type_map);
            if (!std::get<0>(maybe_range_options)) {
                LogWarn() << "Range not found for: " << param_name;
                continue;
            }

            new_parameter->options = std::get<1>(maybe_range_options);
            new_parameter->is_range = true;
            new_parameter->default_option = std::get<2>(maybe_range_options);
        }

        _parameter_map[param_name] = new_parameter;

        InternalCurrentSetting empty_setting{};
        empty_setting.needs_updating = true;
        _current_settings[param_name] = empty_setting;
    }

    return true;
}

std::pair<bool, std::vector<std::shared_ptr<CameraDefinition::Option>>>
CameraDefinition::parse_options(
    const rapidxml::xml_node<>* options_handle,
    const std::string& param_name,
    std::unordered_map<std::string, std::string>& type_map)
{
    std::vector<std::shared_ptr<Option>> options{};

    for (auto e_option = options_handle->first_node("option"); e_option != nullptr;
         e_option = e_option->next_sibling("option")) {
        auto name_attr = e_option->first_attribute("name");
        if (!name_attr) {
            LogErr() << "no option name given";
            return std::make_pair<>(false, options);
        }
        const char* option_name = name_attr->value();

        auto value_attr = e_option->first_attribute("value");
        if (!value_attr) {
            LogErr() << "no option value given";
            return std::make_pair<>(false, options);
        }
        const char* option_value = value_attr->value();

        auto new_option = std::make_shared<Option>();

        new_option->name = option_name;

        new_option->value.set_from_xml(type_map[param_name], option_value);

        // LogDebug() << "Type: " << type_map[param_name] << ", name: " << option_name;

        auto e_exclusions = e_option->first_node("exclusions");
        if (e_exclusions) {
            for (auto e_exclude = e_exclusions->first_node("exclude"); e_exclude != nullptr;
                 e_exclude = e_exclude->next_sibling("exclude")) {
                const char* exclude_text = e_exclude->value();
                if (exclude_text) {
                    new_option->exclusions.emplace_back(exclude_text);
                }
            }
        }

        auto e_parameterranges = e_option->first_node("parameterranges");
        if (e_parameterranges) {
            for (auto e_parameterrange = e_parameterranges->first_node("parameterrange");
                 e_parameterrange != nullptr;
                 e_parameterrange = e_parameterrange->next_sibling("parameterrange")) {
                auto param_attr = e_parameterrange->first_attribute("parameter");
                if (!param_attr) {
                    LogErr() << "missing roption parameter name";
                    return std::make_pair<>(false, options);
                }
                const char* roption_parameter_str = param_attr->value();

                ParameterRange new_parameter_range;

                for (auto e_roption = e_parameterrange->first_node("roption"); e_roption != nullptr;
                     e_roption = e_roption->next_sibling("roption")) {
                    auto roption_name_attr = e_roption->first_attribute("name");
                    if (!roption_name_attr) {
                        LogErr() << "missing roption name attribute";
                        return std::make_pair<>(false, options);
                    }
                    const char* roption_name_str = roption_name_attr->value();

                    auto roption_value_attr = e_roption->first_attribute("value");
                    if (!roption_value_attr) {
                        LogErr() << "missing roption value attribute";
                        return std::make_pair<>(false, options);
                    }
                    const char* roption_value_str = roption_value_attr->value();

                    if (type_map.find(roption_parameter_str) == type_map.end()) {
                        LogErr() << "unknown roption type";
                        return std::make_pair<>(false, options);
                    }

                    ParamValue new_param_value;
                    new_param_value.set_from_xml(
                        type_map[roption_parameter_str], roption_value_str);
                    new_parameter_range[roption_name_str] = new_param_value;

                    // LogDebug() << "range option: "
                    //            << roption_name_str
                    //            << " -> "
                    //            << new_param_value
                    //            << " (" << new_param_value.typestr() << ")";
                }

                new_option->parameter_ranges[roption_parameter_str] = new_parameter_range;

                // LogDebug() << "adding to: " << roption_parameter_str;
            }
        }

        options.push_back(new_option);
    }
    return std::make_pair<>(true, options);
}

std::tuple<bool, std::vector<std::shared_ptr<CameraDefinition::Option>>, CameraDefinition::Option>
CameraDefinition::parse_range_options(
    const rapidxml::xml_node<>* param_handle,
    const std::string& param_name,
    std::unordered_map<std::string, std::string>& type_map)
{
    std::vector<std::shared_ptr<Option>> options{};
    Option default_option{};

    auto min_attr = param_handle->first_attribute("min");
    if (!min_attr) {
        LogDebug() << "min range missing for " << param_name;
        return std::make_tuple<>(false, options, default_option);
    }
    const char* min_str = min_attr->value();

    ParamValue min_value;
    min_value.set_from_xml(type_map[param_name], min_str);

    auto max_attr = param_handle->first_attribute("max");
    if (!max_attr) {
        LogDebug() << "max range missing for " << param_name;
        return std::make_tuple<>(false, options, default_option);
    }
    const char* max_str = max_attr->value();

    auto min_option = std::make_shared<Option>();
    min_option->name = "min";
    min_option->value = min_value;

    ParamValue max_value;
    max_value.set_from_xml(type_map[param_name], max_str);

    auto max_option = std::make_shared<Option>();
    max_option->name = "max";
    max_option->value = max_value;

    auto step_attr = param_handle->first_attribute("step");
    const char* step_str = step_attr ? step_attr->value() : nullptr;
    if (!step_str) {
        LogDebug() << "step range missing for " << param_name;
    }

    if (step_str) {
        ParamValue step_value;
        step_value.set_from_xml(type_map[param_name], step_str);

        auto step_option = std::make_shared<Option>();
        step_option->name = "step";
        step_option->value = step_value;

        options.push_back(min_option);
        options.push_back(max_option);
        options.push_back(step_option);
    } else {
        options.push_back(min_option);
        options.push_back(max_option);
    }

    auto default_attr = param_handle->first_attribute("default");
    if (!default_attr) {
        LogDebug() << "default range missing for " << param_name;
        return std::make_tuple<>(false, options, default_option);
    }
    const char* default_str = default_attr->value();

    ParamValue default_value;
    default_value.set_from_xml(type_map[param_name], default_str);

    default_option.name = default_str;
    default_option.value = default_value;

    return std::make_tuple<>(true, options, default_option);
}

std::pair<bool, CameraDefinition::Option> CameraDefinition::find_default(
    const std::vector<std::shared_ptr<Option>>& options, const std::string& default_str)
{
    Option default_option{};

    bool found_default = false;
    for (auto& option : options) {
        if (option->value == default_str) {
            if (!found_default) {
                default_option = *option;
                found_default = true;
            } else {
                LogErr() << "Found more than one default";
                return std::make_pair<>(false, default_option);
            }
        }
    }
    if (!found_default) {
        LogErr() << "No default found";
        return std::make_pair<>(false, default_option);
    }
    return std::make_pair<>(true, default_option);
}

void CameraDefinition::assume_default_settings()
{
    reset_to_default_settings(false);
}

void CameraDefinition::reset_to_default_settings(bool needs_updating)
{
    _current_settings.clear();

    for (const auto& parameter : _parameter_map) {
        InternalCurrentSetting new_setting;
        new_setting.value = parameter.second->default_option.value;
        new_setting.needs_updating = needs_updating;
        _current_settings[parameter.first] = new_setting;
    }
}

bool CameraDefinition::get_all_settings(std::unordered_map<std::string, ParamValue>& settings)
{
    settings.clear();
    for (const auto& current_setting : _current_settings) {
        settings[current_setting.first] = current_setting.second.value;
    }

    return !settings.empty();
}

bool CameraDefinition::get_possible_settings(std::unordered_map<std::string, ParamValue>& settings)
{
    settings.clear();

    // Find all exclusions
    // TODO: use set instead of vector
    std::vector<std::string> exclusions{};

    for (const auto& parameter : _parameter_map) {
        for (const auto& option : parameter.second->options) {
            if (_current_settings[parameter.first].value == option->value) {
                for (const auto& exclusion : option->exclusions) {
                    // LogDebug() << "found exclusion for " << parameter.first
                    //            << "(" << option->value << "): " << exclusion;
                    exclusions.push_back(exclusion);
                }
            }
        }
    }

    for (const auto& setting : _current_settings) {
        bool excluded = false;
        for (const auto& exclusion : exclusions) {
            if (setting.first == exclusion) {
                excluded = true;
            }
        }
        if (!_parameter_map[setting.first]->is_control) {
            continue;
        }

        if (excluded) {
            continue;
        }
        settings[setting.first] = setting.second.value;
    }

    return !settings.empty();
}

bool CameraDefinition::set_setting(const std::string& name, const ParamValue& value)
{
    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown setting to set: " << name;
        return false;
    }

    // For range params, we need to verify the range.
    if (_parameter_map[name]->is_range) {
        // Check against the minimum
        if (value < _parameter_map[name]->options[0]->value) {
            LogErr() << "Chosen value smaller than minimum";
            return false;
        }

        if (value > _parameter_map[name]->options[1]->value) {
            LogErr() << "Chosen value bigger than maximum";
            return false;
        }

        // TODO: Check step as well, until now we have only seen steps of 1 in the wild though.
    }

    _current_settings[name].value = value;
    _current_settings[name].needs_updating = false;

    // Some param changes cause other params to change, so they need to be updated.
    // The camera definition just keeps track of these params but the actual param fetching
    // needs to happen outside of this class.
    for (const auto& update : _parameter_map[name]->updates) {
        if (_current_settings.find(update) == _current_settings.end()) {
            LogDebug() << "Update to '" << update << "' not understood.";
            continue;
        }
        _current_settings[update].needs_updating = true;
    }

    return true;
}

bool CameraDefinition::get_setting(const std::string& name, ParamValue& value)
{
    if (_current_settings.find(name) == _current_settings.end()) {
        LogErr() << "Unknown setting to get: " << name;
        return false;
    }

    if (!_current_settings.at(name).needs_updating) {
        value = _current_settings.at(name).value;
        return true;
    } else {
        return false;
    }
}

bool CameraDefinition::get_option_value(
    const std::string& param_name, const std::string& option_value, ParamValue& value)
{
    if (_parameter_map.find(param_name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get option: " << param_name;
        return false;
    }

    for (const auto& option : _parameter_map[param_name]->options) {
        if (option->value == option_value) {
            value = option->value;
            return true;
        }
    }

    return false;
}

bool CameraDefinition::get_all_options(const std::string& name, std::vector<ParamValue>& values)
{
    values.clear();

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get all options";
        return false;
    }

    for (const auto& option : _parameter_map[name]->options) {
        values.push_back(option->value);
    }

    return true;
}

bool CameraDefinition::get_possible_options(
    const std::string& name, std::vector<ParamValue>& values)
{
    values.clear();

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get possible options";
        return false;
    }

    std::unordered_map<std::string, ParamValue> settings;
    if (!get_possible_settings(settings)) {
        return false;
    }
    if (settings.find(name) == settings.end()) {
        LogErr() << "Setting " << name << " currently not applicable";
        return false;
    }

    // Find all exclusions because excluded parameters need to be neglected for range
    // check below.
    // TODO: use set instead of vector
    std::vector<std::string> exclusions{};

    for (const auto& parameter : _parameter_map) {
        for (const auto& option : parameter.second->options) {
            if (_current_settings[parameter.first].needs_updating) {
                // LogWarn() << parameter.first << " needs updating";
                continue;
            }
            if (_current_settings[parameter.first].value == option->value) {
                for (const auto& exclusion : option->exclusions) {
                    // LogDebug() << "found exclusion for " << parameter.first
                    //            << "(" << option->value << "): " << exclusion;
                    exclusions.push_back(exclusion);
                }
            }
        }
    }

    // TODO: use set instead of vector for this
    std::vector<ParamValue> allowed_ranges{};

    // Check allowed ranges.
    for (const auto& parameter : _parameter_map) {
        if (!parameter.second->is_control) {
            continue;
        }

        bool excluded = false;
        for (const auto& exclusion : exclusions) {
            if (parameter.first == exclusion) {
                excluded = true;
            }
        }
        if (excluded) {
            continue;
        }

        for (const auto& option : parameter.second->options) {
            if (_current_settings[parameter.first].needs_updating) {
                // LogWarn() << parameter.first << " needs updating";
                continue;
            }
            // Only look at current set option.
            if (_current_settings[parameter.first].value == option->value) {
                // Go through parameter ranges but only concerning the parameter that
                // we're interested in.
                if (option->parameter_ranges.find(name) != option->parameter_ranges.end()) {
                    for (const auto& range : option->parameter_ranges[name]) {
                        allowed_ranges.push_back(range.second);
                    }
                }
            }
        }
    }

    // Intersect
    for (const auto& option : _parameter_map[name]->options) {
        bool option_allowed = false;
        for (const auto& allowed_range : allowed_ranges) {
            if (option->value == allowed_range) {
                option_allowed = true;
            }
        }
        if (option_allowed || allowed_ranges.empty()) {
            values.push_back(option->value);
        }
    }

    return true;
}

void CameraDefinition::get_unknown_params(std::vector<std::pair<std::string, ParamValue>>& params)
{
    params.clear();

    for (const auto& parameter : _parameter_map) {
        if (_current_settings[parameter.first].needs_updating) {
            params.push_back(std::make_pair<>(parameter.first, parameter.second->type));
        }
    }
}

void CameraDefinition::set_all_params_unknown()
{
    for (auto& parameter : _parameter_map) {
        _current_settings[parameter.first].needs_updating = true;
    }
}

bool CameraDefinition::is_setting_range(const std::string& name)
{
    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogWarn() << "Setting " << name << " not found.";
        return false;
    }

    return _parameter_map[name]->is_range;
}

bool CameraDefinition::get_setting_str(const std::string& name, std::string& description)
{
    description.clear();

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogWarn() << "Setting " << name << " not found.";
        return false;
    }

    description = _parameter_map[name]->description;
    return true;
}

bool CameraDefinition::get_option_str(
    const std::string& setting_name, const std::string& option_name, std::string& description)
{
    description.clear();

    if (_parameter_map.find(setting_name) == _parameter_map.end()) {
        LogWarn() << "Setting " << setting_name << " not found.";
        return false;
    }

    for (const auto& option : _parameter_map[setting_name]->options) {
        if (option->value == option_name) {
            description = option->name;
            return true;
        }
    }
    LogWarn() << "Option " << option_name << " not found";
    return false;
}

} // namespace mavsdk
