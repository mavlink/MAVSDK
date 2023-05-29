#include "log.h"
#include "camera_definition.h"

namespace mavsdk {

CameraDefinition::CameraDefinition() {}

CameraDefinition::~CameraDefinition() {}

bool CameraDefinition::load_file(const std::string& filepath)
{
    tinyxml2::XMLError xml_error = _doc.LoadFile(filepath.c_str());
    if (xml_error != tinyxml2::XML_SUCCESS) {
        LogErr() << "tinyxml2::LoadFile failed: " << _doc.ErrorStr();
        return false;
    }

    return parse_xml();
}

bool CameraDefinition::load_string(const std::string& content)
{
    tinyxml2::XMLError xml_error = _doc.Parse(content.c_str());
    if (xml_error != tinyxml2::XML_SUCCESS) {
        LogErr() << "tinyxml2::Parse failed: " << _doc.ErrorStr();
        return false;
    }

    return parse_xml();
}

std::string CameraDefinition::get_model() const
{
    std::lock_guard<std::mutex> lock(_mutex);

    return _model;
}

std::string CameraDefinition::get_vendor() const
{
    std::lock_guard<std::mutex> lock(_mutex);

    return _vendor;
}

bool CameraDefinition::parse_xml()
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto e_mavlinkcamera = _doc.FirstChildElement("mavlinkcamera");
    if (!e_mavlinkcamera) {
        LogErr() << "Tag mavlinkcamera not found";
        return false;
    }

    auto e_definition = e_mavlinkcamera->FirstChildElement("definition");
    if (!e_definition) {
        LogErr() << "definition not found";
        return false;
    }

    auto e_model = e_definition->FirstChildElement("model");
    if (!e_model) {
        LogErr() << "model not found";
        return false;
    }

    _model = e_model->GetText();

    auto e_vendor = e_definition->FirstChildElement("vendor");
    if (!e_vendor) {
        LogErr() << "vendor not found";
        return false;
    }

    _vendor = e_vendor->GetText();

    auto e_parameters = e_mavlinkcamera->FirstChildElement("parameters");
    if (!e_parameters) {
        LogErr() << "Tag parameters not found";
        return false;
    }

    std::unordered_map<std::string, std::string> type_map{};
    // We need all types first.
    for (auto e_parameter = e_parameters->FirstChildElement("parameter"); e_parameter != nullptr;
         e_parameter = e_parameter->NextSiblingElement("parameter")) {
        const char* param_name = e_parameter->Attribute("name");
        if (!param_name) {
            LogErr() << "name attribute missing";
            return false;
        }

        const char* type_str = e_parameter->Attribute("type");
        if (!type_str) {
            LogErr() << "type attribute missing";
            return false;
        }

        type_map[param_name] = type_str;
    }

    for (auto e_parameter = e_parameters->FirstChildElement("parameter"); e_parameter != nullptr;
         e_parameter = e_parameter->NextSiblingElement("parameter")) {
        auto new_parameter = std::make_shared<Parameter>();

        const char* param_name = e_parameter->Attribute("name");
        if (!param_name) {
            LogErr() << "name attribute missing";
            return false;
        }

        const char* type_str_res = e_parameter->Attribute("type");
        if (!type_str_res) {
            LogErr() << "type attribute missing for " << param_name;
            return false;
        }

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
        const char* control_str = e_parameter->Attribute("control");
        if (control_str) {
            if (std::string(control_str) == "0") {
                new_parameter->is_control = false;
            }
        }

        new_parameter->is_readonly = false;
        const char* readonly_str = e_parameter->Attribute("readonly");
        if (readonly_str) {
            if (std::string(readonly_str) == "1") {
                new_parameter->is_readonly = true;
            }
        }

        new_parameter->is_writeonly = false;
        const char* writeonly_str = e_parameter->Attribute("writeonly");
        if (writeonly_str) {
            if (std::string(writeonly_str) == "1") {
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

        auto e_description = e_parameter->FirstChildElement("description");
        if (!e_description) {
            LogErr() << "Description missing";
            return false;
        }

        new_parameter->description = e_description->GetText();

        // LogDebug() << "Found: " << new_parameter->description
        //            << " (" << param_name
        //            << ", control: " << (new_parameter->is_control ? "yes" : "no")
        //            << ", readonly: " << (new_parameter->is_readonly ? "yes" : "no")
        //            << ", writeonly: " << (new_parameter->is_writeonly ? "yes" : "no")
        //            << ")";

        auto e_updates = e_parameter->FirstChildElement("updates");
        if (e_updates) {
            for (auto e_update = e_updates->FirstChildElement("update"); e_update != nullptr;
                 e_update = e_update->NextSiblingElement("update")) {
                // LogDebug() << "Updates: " << e_update->GetText();
                new_parameter->updates.push_back(e_update->GetText());
            }
        }

        const char* default_str = e_parameter->Attribute("default");
        if (!default_str) {
            LogWarn() << "Default missing for " << param_name;
            continue;
        }

        auto get_default_opt = [&]() {
            auto maybe_default = find_default(new_parameter->options, default_str);

            if (!maybe_default.first) {
                LogWarn() << "Default not found for " << param_name;
                return std::optional<Option>{};
            }

            return std::optional{maybe_default.second};
        };

        auto e_options = e_parameter->FirstChildElement("options");
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
                LogWarn() << "Not found: " << param_name;
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
    const tinyxml2::XMLElement* options_handle,
    const std::string& param_name,
    std::unordered_map<std::string, std::string>& type_map)
{
    std::vector<std::shared_ptr<Option>> options{};

    for (auto e_option = options_handle->FirstChildElement("option"); e_option != nullptr;
         e_option = e_option->NextSiblingElement("option")) {
        const char* option_name = e_option->Attribute("name");
        if (!option_name) {
            LogErr() << "no option name given";
            return std::make_pair<>(false, options);
        }

        const char* option_value = e_option->Attribute("value");
        if (!option_value) {
            LogErr() << "no option value given";
            return std::make_pair<>(false, options);
        }

        auto new_option = std::make_shared<Option>();

        new_option->name = option_name;

        new_option->value.set_from_xml(type_map[param_name], option_value);

        // LogDebug() << "Type: " << type_map[param_name] << ", name: " << option_name;

        auto e_exclusions = e_option->FirstChildElement("exclusions");
        if (e_exclusions) {
            for (auto e_exclude = e_exclusions->FirstChildElement("exclude"); e_exclude != nullptr;
                 e_exclude = e_exclude->NextSiblingElement("exclude")) {
                // LogDebug() << "Exclude: " << e_exclude->GetText();
                new_option->exclusions.push_back(e_exclude->GetText());
            }
        }

        auto e_parameterranges = e_option->FirstChildElement("parameterranges");
        if (e_parameterranges) {
            for (auto e_parameterrange = e_parameterranges->FirstChildElement("parameterrange");
                 e_parameterrange != nullptr;
                 e_parameterrange = e_parameterrange->NextSiblingElement("parameterrange")) {
                const char* roption_parameter_str = e_parameterrange->Attribute("parameter");
                if (!roption_parameter_str) {
                    LogErr() << "missing roption parameter name";
                    return std::make_pair<>(false, options);
                }

                ParameterRange new_parameter_range;

                for (auto e_roption = e_parameterrange->FirstChildElement("roption");
                     e_roption != nullptr;
                     e_roption = e_roption->NextSiblingElement("roption")) {
                    const char* roption_name_str = e_roption->Attribute("name");
                    if (!roption_name_str) {
                        LogErr() << "missing roption name attribute";
                        return std::make_pair<>(false, options);
                    }

                    const char* roption_value_str = e_roption->Attribute("value");
                    if (!roption_value_str) {
                        LogErr() << "missing roption value attribute";
                        return std::make_pair<>(false, options);
                    }

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
    const tinyxml2::XMLElement* param_handle,
    const std::string& param_name,
    std::unordered_map<std::string, std::string>& type_map)
{
    std::vector<std::shared_ptr<Option>> options{};
    Option default_option{};

    const char* min_str = param_handle->Attribute("min");
    if (!min_str) {
        LogErr() << "min range missing for " << param_name;
        return std::make_tuple<>(false, options, default_option);
    }

    ParamValue min_value;
    min_value.set_from_xml(type_map[param_name], min_str);

    const char* max_str = param_handle->Attribute("max");
    if (!max_str) {
        LogErr() << "max range missing for " << param_name;
        return std::make_tuple<>(false, options, default_option);
    }

    auto min_option = std::make_shared<Option>();
    min_option->name = "min";
    min_option->value = min_value;

    ParamValue max_value;
    max_value.set_from_xml(type_map[param_name], max_str);

    auto max_option = std::make_shared<Option>();
    max_option->name = "max";
    max_option->value = max_value;

    const char* step_str = param_handle->Attribute("step");
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

    const char* default_str = param_handle->Attribute("default");
    if (!default_str) {
        LogDebug() << "default range missing for " << param_name;
        return std::make_tuple<>(false, options, default_option);
    }

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
    std::lock_guard<std::mutex> lock(_mutex);

    _current_settings.clear();

    for (const auto& parameter : _parameter_map) {
        // if (parameter.second->is_range) {

        InternalCurrentSetting new_setting;
        new_setting.value = parameter.second->default_option.value;
        new_setting.needs_updating = false;
        _current_settings[parameter.first] = new_setting;

        //} else {

        //    for (const auto &option : parameter.second->options) {
        //        if (!option->is_default) {
        //            //LogDebug() << option->name << " not default";
        //            continue;
        //        }
        //        //LogDebug() << option->name << " default value: " << option->value << " (type: "
        //        <<
        //        // option->value.typestr() << ")";

        //        InternalCurrentSetting new_setting;
        //        new_setting.value = option->value;
        //        new_setting.needs_updating = false;
        //        _current_settings[parameter.first] = new_setting;
        //    }
        //}
    }
}

bool CameraDefinition::get_all_settings(std::unordered_map<std::string, ParamValue>& settings)
{
    std::lock_guard<std::mutex> lock(_mutex);

    settings.clear();
    for (const auto& current_setting : _current_settings) {
        settings[current_setting.first] = current_setting.second.value;
    }

    return (settings.size() > 0);
}

bool CameraDefinition::get_possible_settings(std::unordered_map<std::string, ParamValue>& settings)
{
    std::lock_guard<std::mutex> lock(_mutex);

    return get_possible_settings_locked(settings);
}

bool CameraDefinition::get_possible_settings_locked(
    std::unordered_map<std::string, ParamValue>& settings)
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

    return (settings.size() > 0);
}

bool CameraDefinition::set_setting(const std::string& name, const ParamValue& value)
{
    std::lock_guard<std::mutex> lock(_mutex);

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
            // LogDebug() << "Update to '" << update << "' not understood.";
            continue;
        }
        _current_settings[update].needs_updating = true;
    }

    return true;
}

bool CameraDefinition::get_setting(const std::string& name, ParamValue& value)
{
    std::lock_guard<std::mutex> lock(_mutex);

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
    std::lock_guard<std::mutex> lock(_mutex);

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
    std::lock_guard<std::mutex> lock(_mutex);

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
    std::lock_guard<std::mutex> lock(_mutex);

    values.clear();

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get possible options";
        return false;
    }

    std::unordered_map<std::string, ParamValue> settings;
    if (!get_possible_settings_locked(settings)) {
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
                // we're interested in..
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
        if (option_allowed || allowed_ranges.size() == 0) {
            values.push_back(option->value);
        }
    }

    return true;
}

void CameraDefinition::get_unknown_params(std::vector<std::pair<std::string, ParamValue>>& params)
{
    std::lock_guard<std::mutex> lock(_mutex);

    params.clear();

    for (const auto& parameter : _parameter_map) {
        if (_current_settings[parameter.first].needs_updating) {
            params.push_back(std::make_pair<>(parameter.first, parameter.second->type));
        }
    }
}

void CameraDefinition::set_all_params_unknown()
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto& parameter : _parameter_map) {
        _current_settings[parameter.first].needs_updating = true;
    }
}

bool CameraDefinition::is_setting_range(const std::string& name)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogWarn() << "Setting " << name << " not found.";
        return false;
    }

    return _parameter_map[name]->is_range;
}

bool CameraDefinition::get_setting_str(const std::string& name, std::string& description)
{
    std::lock_guard<std::mutex> lock(_mutex);

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
    std::lock_guard<std::mutex> lock(_mutex);

    description.clear();

    if (_parameter_map.find(setting_name) == _parameter_map.end()) {
        LogWarn() << "Setting " << setting_name << " not found.";
        return false;
    }

    for (const auto& option : _parameter_map[setting_name]->options) {
        std::stringstream value_ss{};
        value_ss << option->value;
        if (option->value == option_name) {
            description = option->name;
            return true;
        }
    }
    LogWarn() << "Option " << option_name << " not found";
    return false;
}

} // namespace mavsdk
