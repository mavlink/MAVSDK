#include "global_include.h"
#include "log.h"
#include "camera_definition.h"

namespace dronecore {

CameraDefinition::CameraDefinition() {}

CameraDefinition::~CameraDefinition() {}

bool CameraDefinition::load_file(const char *filename)
{
    tinyxml2::XMLError xml_error = _doc.LoadFile(filename);
    if (xml_error != tinyxml2::XML_SUCCESS) {
        LogErr() << "tinyxml2::LoadFile failed: " << _doc.ErrorStr();
        return false;
    }

    return parse_xml();
}

bool CameraDefinition::load_string(const std::string &content)
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
    return _model;
}

std::string CameraDefinition::get_vendor() const
{
    return _vendor;
}

bool CameraDefinition::parse_xml()
{
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

    std::map<std::string, std::string> type_map {};
    // We need all types first.
    for (auto e_parameter = e_parameters->FirstChildElement("parameter");
         e_parameter != nullptr;
         e_parameter = e_parameter->NextSiblingElement("parameter")) {

        const char *param_name = e_parameter->Attribute("name");
        if (!param_name) {
            LogErr() << "name attribute missing";
            return false;
        }

        const char *type_str = e_parameter->Attribute("type");
        if (!type_str) {
            LogErr() << "type attribute missing";
            return false;
        }

        type_map[param_name] = type_str;
    }

    for (auto e_parameter = e_parameters->FirstChildElement("parameter");
         e_parameter != nullptr;
         e_parameter = e_parameter->NextSiblingElement("parameter")) {

        auto new_parameter = std::make_shared<Parameter>();

        const char *param_name = e_parameter->Attribute("name");
        if (!param_name) {
            LogErr() << "name attribute missing";
            return false;
        }

        // By default control is on.
        new_parameter->is_control = true;
        const char *control_str = e_parameter->Attribute("control");
        if (control_str) {
            if (strcmp(control_str, "0") == 0) {
                new_parameter->is_control = false;
            }
        }

        new_parameter->is_readonly = false;
        const char *readonly_str = e_parameter->Attribute("readonly");
        if (readonly_str) {
            if (strcmp(readonly_str, "1") == 0) {
                new_parameter->is_readonly = true;
            }
        }

        new_parameter->is_writeonly = false;
        const char *writeonly_str = e_parameter->Attribute("writeonly");
        if (writeonly_str) {
            if (strcmp(writeonly_str, "1") == 0) {
                new_parameter->is_writeonly = true;
            }
        }

        if (new_parameter->is_readonly && new_parameter->is_writeonly) {
            LogErr() << "parameter can't be readonly and writeonly";
            return false;
        }

        // Be definition custom types do not have control.
        if (strcmp(type_map[param_name].c_str(), "custom") == 0) {
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
            for (auto e_update = e_updates->FirstChildElement("update");
                 e_update != nullptr;
                 e_update = e_update->NextSiblingElement("update")) {

                // LogDebug() << "Updates: " << e_update->GetText();
                new_parameter->updates.push_back(e_update->GetText());
            }
        }

        auto e_options = e_parameter->FirstChildElement("options");
        if (!e_options) {
            continue;
        }

        // We only need a default if we have options.
        const char *default_str = e_parameter->Attribute("default");
        if (!default_str) {
            LogErr() << "Default missing for " << param_name;
            return false;
        }

        bool found_default = false;

        for (auto e_option = e_options->FirstChildElement("option");
             e_option != nullptr;
             e_option = e_option->NextSiblingElement("option")) {

            const char *option_name = e_option->Attribute("name");
            if (!option_name) {
                LogErr() << "no option name given";
                return false;
            }

            const char *option_value = e_option->Attribute("value");
            if (!option_value) {
                LogErr() << "no option value given";
                return false;
            }

            auto new_option = std::make_shared<Option>();

            new_option->name = option_name;

            new_option->value.set_from_xml(type_map[param_name], option_value);

            if (std::strcmp(option_value, default_str) == 0) {
                new_option->is_default = true;
                found_default = true;
            }

            // LogDebug() << "Type: " << type_map[param_name] << ", name: " << option_name;

            auto e_exclusions = e_option->FirstChildElement("exclusions");
            if (e_exclusions) {
                for (auto e_exclude = e_exclusions->FirstChildElement("exclude");
                     e_exclude != nullptr;
                     e_exclude = e_exclude->NextSiblingElement("exclude")) {

                    //LogDebug() << "Exclude: " << e_exclude->GetText();
                    new_option->exclusions.push_back(e_exclude->GetText());
                }
            }

            auto e_parameterranges = e_option->FirstChildElement("parameterranges");
            if (e_parameterranges) {

                for (auto e_parameterrange = e_parameterranges->FirstChildElement("parameterrange");
                     e_parameterrange != nullptr;
                     e_parameterrange = e_parameterrange->NextSiblingElement("parameterrange")) {

                    const char *roption_parameter_str = e_parameterrange->Attribute("parameter");
                    if (!roption_parameter_str) {
                        LogErr() << "missing roption parameter name";
                        return false;
                    }

                    parameter_range_t new_parameter_range;

                    for (auto e_roption = e_parameterrange->FirstChildElement("roption");
                         e_roption != nullptr;
                         e_roption = e_roption->NextSiblingElement("roption")) {

                        const char *roption_name_str = e_roption->Attribute("name");
                        if (!roption_name_str) {
                            LogErr() << "missing roption name attribute";
                            return false;
                        }

                        const char *roption_value_str = e_roption->Attribute("value");
                        if (!roption_value_str) {
                            LogErr() << "missing roption value attribute";
                            return false;
                        }

                        if (type_map.find(roption_parameter_str) == type_map.end()) {
                            LogErr() << "unknown roption type";
                            return false;
                        }

                        MAVLinkParameters::ParamValue new_param_value;
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

            new_parameter->options.push_back(new_option);
        }

        if (!found_default) {
            LogWarn() << "Default not found for " << param_name;
            // TODO: in the future we should fail completely here
            // return false;
            continue;
        }

        _parameter_map[param_name] = new_parameter;

        InternalCurrentSetting empty_setting {};
        empty_setting.needs_updating = true;
        _current_settings[param_name] = empty_setting;
    }

    return true;
}

void CameraDefinition::assume_default_settings()
{
    _current_settings.clear();

    for (const auto &parameter : _parameter_map) {
        for (const auto &option : parameter.second->options) {

            if (!option->is_default) {
                // LogDebug() << option->name << " not default";
                continue;
            }
            // LogDebug() << option->name << " default value: " << option->value << " (type: " << option->value.typestr() << ")";

            _current_settings[parameter.first] = InternalCurrentSetting {option->value, false};
        }
    }
}

bool CameraDefinition::get_all_settings(std::map<std::string, MAVLinkParameters::ParamValue>
                                        &settings)
{
    settings.clear();
    for (const auto &current_setting : _current_settings) {
        settings[current_setting.first] = current_setting.second.value;
    }

    return (settings.size() > 0);
}

bool CameraDefinition::get_possible_settings(
    std::map<std::string, MAVLinkParameters::ParamValue> &settings)
{
    settings.clear();

    // Find all exclusions
    // TODO: use set instead of vector
    std::vector<std::string> exclusions {};

    for (const auto &parameter : _parameter_map) {
        for (const auto &option : parameter.second->options) {
            if (_current_settings[parameter.first].needs_updating) {
                continue;
            }
            if (_current_settings[parameter.first].value == option->value) {
                for (const auto &exclusion : option->exclusions) {
                    // LogDebug() << "found exclusion for " << parameter.first
                    //            << "(" << option->value << "): " << exclusion;
                    exclusions.push_back(exclusion);
                }
            }
        }
    }

    for (const auto &setting : _current_settings) {
        bool excluded = false;
        for (const auto &exclusion : exclusions) {
            if (setting.first == exclusion) {
                excluded = true;
            }
        }
        if (!_parameter_map[setting.first]->is_control) {
            continue;
        }

        if (!excluded) {
            settings[setting.first] = setting.second.value;
        }
    }

    return (settings.size() > 0);
}

std::string CameraDefinition::get_setting_str(const std::string &param_name)
{
    if (_parameter_map.find(param_name) == _parameter_map.end()) {
        LogErr() << "Unknown setting to provide string";
        return "Unknown";
    }
    return _parameter_map[param_name]->description;
}

bool CameraDefinition::set_setting(const std::string &name,
                                   const MAVLinkParameters::ParamValue &value)
{
    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown setting to set";
        return false;
    }

    // FIXME: this is a hack because some params have the wrong type
    //        when they come from the camera.
    MAVLinkParameters::ParamValue changed_value = value;
    if (name.compare("CAM_MODE") == 0 ||
        name.compare("CAM_COLORMODE") == 0 ||
        name.compare("CAM_EXPMODE") == 0 ||
        name.compare("CAM_METERING") == 0 ||
        name.compare("CAM_PHOTOFMT") == 0 ||
        name.compare("CAM_PHOTOQUAL") == 0 ||
        name.compare("CAM_VIDRES") == 0 ||
        name.compare("CAM_WBMODE") == 0 ||
        name.compare("CAM_COLORENCODE") == 0 ||
        name.compare("CAM_FLICKER") == 0) {
        if (changed_value.is_uint8()) {
            uint8_t temp = changed_value.get_uint8();
            changed_value.set_uint32(uint32_t(temp));
        }
    }

    // LogDebug() << "Setting " << name << " of type: " << changed_value.typestr();
    _current_settings[name].value = changed_value;
    _current_settings[name].needs_updating = false;

    // Some param changes cause other params to change, so they need to be updated.
    // The camera definition just keeps track of these params but the actual param fetching
    // needs to happen outside of this class.
    for (const auto &update : _parameter_map[name]->updates) {
        if (_current_settings.find(update) == _current_settings.end()) {
            // LogDebug() << "Update to '" << update << "' not understood.";
            continue;
        }
        _current_settings[update].needs_updating = true;
    }

    return true;
}

bool CameraDefinition::get_setting(const std::string &name,
                                   MAVLinkParameters::ParamValue &value)
{
    if (_current_settings.find(name) == _current_settings.end()) {
        LogErr() << "Unknown setting to get";
        return false;
    }

    if (!_current_settings.at(name).needs_updating) {
        value = _current_settings.at(name).value;
        return true;
    } else {
        return false;
    }
}

std::string CameraDefinition::get_option_str(const std::string &param_name,
                                             const std::string &option_value)
{
    if (_parameter_map.find(param_name) == _parameter_map.end()) {
        LogErr() << "Unknown setting to provide option string";
        return "Unknown";
    }

    for (const auto &option : _parameter_map[param_name]->options) {
        std::stringstream value_ss {};
        value_ss << option->value;
        if (option->value == option_value) {
            return option->name;
        }
    }

    LogErr() << "Unknown option string";
    return "Unknown";
}

bool CameraDefinition::get_option_value(const std::string &param_name,
                                        const std::string &option_value,
                                        MAVLinkParameters::ParamValue &value)
{
    if (_parameter_map.find(param_name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get option";
        return false;
    }

    for (const auto &option : _parameter_map[param_name]->options) {
        if (option->value == option_value) {
            value = option->value;
            return true;
        }
    }


    return false;
}

bool CameraDefinition::get_all_options(
    const std::string &name, std::vector<MAVLinkParameters::ParamValue> &values)
{
    values.clear();

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get all options";
        return false;
    }

    for (const auto &option : _parameter_map[name]->options) {
        values.push_back(option->value);
    }

    return true;
}

bool CameraDefinition::get_possible_options(
    const std::string &name, std::vector<MAVLinkParameters::ParamValue> &values)
{
    values.clear();

    if (_parameter_map.find(name) == _parameter_map.end()) {
        LogErr() << "Unknown parameter to get possible options";
        return false;
    }

    std::map<std::string, MAVLinkParameters::ParamValue> settings;
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
    std::vector<std::string> exclusions {};

    for (const auto &parameter : _parameter_map) {
        for (const auto &option : parameter.second->options) {
            if (_current_settings[parameter.first].needs_updating) {
                // LogWarn() << parameter.first << " needs updating";
                continue;
            }
            if (_current_settings[parameter.first].value == option->value) {
                for (const auto &exclusion : option->exclusions) {
                    // LogDebug() << "found exclusion for " << parameter.first
                    //            << "(" << option->value << "): " << exclusion;
                    exclusions.push_back(exclusion);
                }
            }
        }
    }

    // TODO: use set instead of vector for this
    std::vector<MAVLinkParameters::ParamValue> allowed_ranges {};

    // Check allowed ranges.
    for (const auto &parameter : _parameter_map) {

        if (!parameter.second->is_control) {
            continue;
        }

        bool excluded = false;
        for (const auto &exclusion : exclusions) {
            if (parameter.first == exclusion) {
                excluded = true;
            }
        }
        if (excluded) {
            continue;
        }

        for (const auto &option : parameter.second->options) {
            if (_current_settings[parameter.first].needs_updating) {
                // LogWarn() << parameter.first << " needs updating";
                continue;
            }
            // Only look at current set option.
            if (_current_settings[parameter.first].value == option->value) {
                // Go through parameter ranges but only concerning the parameter that
                // we're interested in..
                if (option->parameter_ranges.find(name)
                    != option->parameter_ranges.end()) {

                    for (const auto &range : option->parameter_ranges[name]) {
                        allowed_ranges.push_back(range.second);
                    }
                }
            }
        }
    }

    // Intersect
    for (const auto &option : _parameter_map[name]->options) {
        bool option_allowed = false;
        for (const auto &allowed_range : allowed_ranges) {
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

bool CameraDefinition::get_unknown_params(std::vector<std::string> &params)
{
    params.clear();

    for (const auto &parameter : _parameter_map) {
        if (_current_settings[parameter.first].needs_updating) {
            params.push_back(parameter.first);
        }
    }
    return true;
}

void CameraDefinition::set_all_params_unknown()
{
    for (auto &parameter : _parameter_map) {
        _current_settings[parameter.first].needs_updating = true;
    }
}

} // namespace dronecore
