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
    std::cout << content;
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
    std::vector<std::string> exclusion_parameter_names {};

    parameters.clear();
    auto parameter_list = _doc.FirstChildElement("mavlinkcamera")->
                          FirstChildElement("parameters")->
                          FirstChildElement("parameter");

    while (parameter_list) {
        auto description = parameter_list->FirstChildElement("description");

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
        new_param->options = std::vector<std::shared_ptr<ParameterOption>>();
        if (std::strcmp(type_str, "float") == 0) {
            new_param->type = Type::FLOAT;
        } else if (std::strcmp(type_str, "uint32") == 0) {
            new_param->type = Type::UINT32;
        } else {
            LogErr() << "Unknown type: " << type_str << ", ignoring for now...";
            parameter_list = parameter_list->NextSiblingElement("parameter");
            continue;
        }

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
        parameter_list = parameter_list->NextSiblingElement("parameter");
    }

    // Finally remove what should be excluded.
    for (auto &excluded : exclusion_parameter_names) {
        // LogDebug() << "excluded: " << excluded;
        parameters.erase(excluded);
    }

    return true;
}

void CameraDefinition::update_setting(const std::string &name, const ParameterValue &value)
{
    if (_current_settings.find(name) == _current_settings.end()) {
        _current_settings.insert(
            std::pair<std::string, ParameterValue>
            (name, value));
    } else {
        _current_settings[name] = value;
    }
}


} // namespace dronecore
