#include "global_include.h"
#include "log.h"
#include "camera_definition.h"

namespace dronecore {

CameraDefinition::CameraDefinition() :
    _doc()
{
}

CameraDefinition::~CameraDefinition()
{
}

void CameraDefinition::load_file(const char *filename)
{
    _doc.LoadFile(filename);
}

void CameraDefinition::load_string(const std::string &content)
{
    _doc.Parse(content.c_str());
}

const char *CameraDefinition::get_model() const
{
    return _doc.FirstChildElement("mavlinkcamera")->
           FirstChildElement("definition")->
           FirstChildElement("model")->GetText();
}

const char *CameraDefinition::get_vendor() const
{
    return _doc.FirstChildElement("mavlinkcamera")->
           FirstChildElement("definition")->
           FirstChildElement("vendor")->GetText();
}

bool CameraDefinition::get_parameters(parameter_map_t &parameters, bool filter_possible)
{
    // Reset input/output first.
    parameters.clear();

    auto e_mavlinkcamera = _doc.FirstChildElement("mavlinkcamera");
    if (!e_mavlinkcamera) {
        return false;
    }

    auto e_parameters = e_mavlinkcamera->FirstChildElement("parameters");
    if (!e_parameters) {
        return false;
    }

    for (auto e_parameter = e_parameters->FirstChildElement("parameter");
         e_parameter != nullptr;
         e_parameter = e_parameter->NextSiblingElement("parameter")) {

        const char *param_name = e_parameter->Attribute("name");
        if (!param_name) {
            return false;
        }

        const char *param_type = e_parameter->Attribute("type");
        if (!param_type) {
            return false;
        }

        // By default control is on.
        bool is_control = true;
        const char *control_str = e_parameter->Attribute("control");
        if (control_str) {
            if (strcmp(control_str, "0") == 0) {
                is_control = false;
            }
        }

        bool is_readonly = false;
        const char *readonly_str = e_parameter->Attribute("readonly");
        if (readonly_str) {
            if (strcmp(readonly_str, "1") == 0) {
                is_readonly = true;
            }
        }

        bool is_writeonly = false;
        const char *writeonly_str = e_parameter->Attribute("writeonly");
        if (writeonly_str) {
            if (strcmp(writeonly_str, "1") == 0) {
                is_writeonly = true;
            }
        }

        if (is_readonly && is_writeonly) {
            return false;
        }

        const char *type_str = e_parameter->Attribute("type");
        if (!type_str) {
            return false;
        }

        auto e_description = e_parameter->FirstChildElement("description");
        if (!e_description) {
            return false;
        }

        LogDebug() << "Found: " << e_description->GetText()
                   << " (" << param_name
                   << ", control: " << (is_control ? "yes" : "no")
                   << ", readonly: " << (is_readonly ? "yes" : "no")
                   << ", writeonly: " << (is_writeonly ? "yes" : "no")
                   << ")";

        // TODO: parse default which is of param type.

        auto e_options = e_parameter->FirstChildElement("options");
        if (!e_options) {
            LogDebug() << "No options found";
            continue;
        }

        for (auto e_option = e_options->FirstChildElement("option");
             e_option != nullptr;
             e_option = e_option->NextSiblingElement("option")) {

            const char *option_name = e_option->Attribute("name");
            if (!option_name) {
                LogErr() << "no option name given";
                return false;
            }

            auto new_param = std::make_shared<MavlinkParameters::ParamValue>();
            new_param->set_from_xml(type_str, option_name);

            // TODO: do something smart here
        }

    }

#if 0
    auto new_param = std::make_shared<Parameter>();
    const char *name = parameter_list->Attribute("name");
    if (!name) {
        LogErr() << "Could not find parameter name attribute";
        return false;
    }

    const char *type_str = parameter_list->Attribute("type");
    if (!name) {
        LogErr() << "Could not find parameter type attribute";
        return false;
    }

    //new_param->name = std::string(name);
    new_param->description = std::string(description->GetText());
    new_param->options = option_list_t();

    auto option = std::make_shared<MalvinkParameters::ParamValue>();
    option.set_from_xml(type_str,

                        auto option = parameter_list->FirstChildElement("options")->
                                      FirstChildElement("option");

    while (option) {
    auto new_option = std::make_shared<ParameterOption>();
        if (new_param->type == Type::FLOAT) {
            new_option->value.value.as_float = option->FloatAttribute("value");
        } else if (new_param->type == Type::UINT32) {
            new_option->value.value.as_uint32 = option->UnsignedAttribute("value");
        }

        const char *option_name = option->Attribute("name");
        if (!option_name) {
            LogErr() << "Could not find option name attribute";
            return false;
        }

        new_option->name = std::string(option_name);
        new_param->options.push_back(new_option);

        if (filter_possible) {
            auto exclusions = option->FirstChildElement("exclusions");
            if (exclusions) {
                auto exclude = exclusions->FirstChildElement("exclude");
                while (exclude) {
                    std::string exclude_param_name = exclude->GetText();

                    // Check if param exists
                    if (_current_settings.find(name) != _current_settings.end()) {
                        if (_current_settings.find(name)->second.value.as_uint32 == new_option->value.value.as_uint32) {
                            // LogDebug() << "found current setting: " << name << ": " << _current_settings.find(name)->second.value.as_uint32;
                            exclusion_parameter_names.push_back(exclude_param_name);
                        }
                    }

                    exclude = exclude->NextSiblingElement("exclude");
                }
            }
        }

        option = option->NextSiblingElement("option");
    }

    parameters.insert(std::pair<std::string, std::shared_ptr<Parameter>>(std::string(name), new_param));
#endif

#if 0
    // Finally remove what should be excluded.
    for (auto &excluded : exclusion_parameter_names) {
    // LogDebug() << "excluded: " << excluded;
    parameters.erase(excluded);
    }
#endif

    return true;
}

void CameraDefinition::update_setting(const std::string &name,
                                      const MavlinkParameters::ParamValue &value)
{
    UNUSED(name);
    UNUSED(value);
#if 0
    if (_current_settings.find(name) == _current_settings.end()) {
        _current_settings.insert(
            std::pair<std::string, ParameterValue>
            (name, value));
    } else {
        _current_settings[name] = value;
    }
#endif
}


} // namespace dronecore
